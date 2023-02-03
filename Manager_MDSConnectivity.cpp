#include "Manager_MDSConnectivity.h"
#include "MainDialog.h"
#include "ui_Dialog_forcerelease.h"
#include <map>
#include <QJsonValue>


void Manager_MDSConnectivity::init()
{
    Common* common = Common::instance();
    cond = new dds::sub::cond::ReadCondition(
                common->mdsm_reader,
                dds::sub::status::DataState(
                dds::sub::status::SampleState::any(),
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
    std::string querystr3 = "alarm_state MATCH ''";
    patient_alarm_cond = new dds::sub::cond::QueryCondition(
                dds::sub::Query(common->topalarm_reader, querystr3),
                dds::sub::status::DataState(
                dds::sub::status::SampleState::any(),//not_read
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
    technical_alarm_cond = new dds::sub::cond::QueryCondition(
                dds::sub::Query(common->topalarm_reader_2, querystr3),
                dds::sub::status::DataState(
                dds::sub::status::SampleState::any(),//not_read
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
    last_query_time = 0;
    last_query_time1 = 0;
}
void Manager_MDSConnectivity::step()
{
    Common* common = Common::instance();
    uint32_t current_time1 = Common::get_time_ms();
    if(Common::get_elapsed_time(current_time1, last_query_time1) > 60000)
    {
        last_query_time1 = current_time1;
        qDebug()<<"running time ="<<time_running<<"m=======================================================";
        ++time_running;
    }
    if(!common->is_server)
    {
        if(common->patient_id.size()==0)     return;
        uint32_t current_time = Common::get_time_ms();//bs 防呆重複綁定同一個病人
        std::string querystr = "patient_id MATCH '";
            querystr.append(common->patient_id);
            querystr.append("'");
        dds::sub::cond::QueryCondition cond(
                    dds::sub::Query(common->mdsm_reader, querystr),
                    dds::sub::status::DataState(
                    dds::sub::status::SampleState::any(),
                    dds::sub::status::ViewState::any(),
                    dds::sub::status::InstanceState::alive()));
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples = common->mdsm_reader.select().condition(cond).read();
        if(Common::get_elapsed_time(current_time, last_query_time) > (uint32_t)common->patients_query_interval*5000)
            {
            last_query_time = current_time;
            if(samples.length()>1)
                {
                common->mapping_tab->hide_windows();
                common->rd.close();
                common->mapping_tab->frd.ui->label->setText("Duplicate binding detected. Forcing release.");
                common->mapping_tab->frd.clearText();
                common->mapping_tab->frd.exec();
                std::string employeeID = common->mapping_tab->frd.getText();
                common->md->ui->nav->set_current_tab(0);
                common->mapping_tab->perform_release(employeeID);
                }
            }
        return;
    }
    if(common->patient_id.size()>0)     return;
    mdsconnectivity.clear();
    uint32_t current_time = Common::get_time_ms();
    if(Common::get_elapsed_time(current_time, last_query_time) > (uint32_t)common->MDSloop_interval*1000)
        {
        common->observation_puller.start(true);
    qDebug()<<"Manager_MDSConnectivity";
        fflog_out(common->log,"Manager_MDSConnectivity");
        last_query_time = current_time;
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples = common->mdsm_reader.select().condition(*cond).read();
        for(auto& sample : samples)//MDS
            if(sample.info().valid())
            {
                dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
                MDSConnectivity mdsc;
                mdsc.vmd_id = data.value<std::string>("vmd_id") ;
                mdsc.patient_id= data.value<std::string>("patient_id") ;
                mdsc.room_id= data.value<std::string>("room_id") ;
                mdsc.bed_id= data.value<std::string>("bed_id") ;
//                mdsc.source_timestamp = data.value<uint64_t>("source_timestamp") ;
                std::string querystr = "patient_id MATCH '";
                querystr.append(data.value<std::string>("patient_id"));
                querystr.append("'");
                dds::sub::cond::QueryCondition cond2(
                            dds::sub::Query(common->patient_reader, querystr),
                            dds::sub::status::DataState(
                            dds::sub::status::SampleState::any(),
                            dds::sub::status::ViewState::any(),
                            dds::sub::status::InstanceState::alive()));
                dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> patient_samples = common->patient_reader.select().condition(cond2).read();
                if(patient_samples.length() > 0)
                    mdsconnectivity.emplace(mdsc.bed_id, mdsc);
            }
        for(int i=0; i<common->monitor_page->Set_bed.size(); i++)
            {
            auto item = mdsconnectivity.find(common->monitor_page->Set_bed[i].toString().toStdString());
            if (item !=  mdsconnectivity.end())
                {
                continue;
                }
            MDSConnectivity null;
            null.patient_id = "" ;
            null.vmd_id= "" ;
            null.room_id= "" ;
            null.bed_id= common->monitor_page->Set_bed[i].toString().toStdString();
            mdsconnectivity.emplace(null.bed_id, null);
            }
        //收集警告
/*        std::string dummy; //警告訊息從資料庫拿
        std::string sql = "SELECT meta().id,alarm_no,channel_id,vmd_id,patient_id,alarm_code,alarm_description,alarm_priority,alarm_state,source_timestamp.sec,source_timestamp.nanosec FROM _ WHERE data_source='NumericDeviceSelection'";
        cbl::ResultSet results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
        std::string sql1 = "SELECT meta().id,alarm_no,channel_id,vmd_id,patient_id,alarm_code,alarm_description,alarm_priority,alarm_state,source_timestamp.sec,source_timestamp.nanosec FROM _ WHERE data_source='NumericDeviceSelection'";
        cbl::ResultSet results1 = common->cbl->queryDocuments(common->display_items_db, sql1, dummy);*/

        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> topalarm_samples = common->topalarm_reader.select().condition(*common->md->mdsm.patient_alarm_cond).read();
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> topalarm_samples2 = common->topalarm_reader_2.select().condition(*common->md->mdsm.technical_alarm_cond).read();
        for (auto sample :topalarm_samples)
        {
            Alarm alarm;
            dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
            rti::core::xtypes::LoanedDynamicData loaned_member = data.loan_value("source_timestamp");
            uint64_t sec = (time_t)loaned_member.get().value<int32_t>("sec");
            uint64_t nanosec = (time_t)loaned_member.get().value<uint32_t>("nanosec");
            loaned_member.return_loan();
            std::string alarm_no=data.value<std::string>("alarm_no").c_str();
            std::string channel_id=data.value<std::string>("channel_id").c_str();
            std::string vmd_id=data.value<std::string>("vmd_id").c_str();
            std::string model=data.value<std::string>("model").c_str();
            std::string patient_id=data.value<std::string>("patient_id").c_str();
            std::string alarm_priority=data.value<std::string>("alarm_priority").c_str();
            std::string alarm_code=data.value<std::string>("alarm_code").c_str();
            std::string alarm_description=data.value<std::string>("alarm_description").c_str();
            std::string alarm_state=data.value<std::string>("alarm_state").c_str();

            alarm.patient_id = patient_id;
            alarm.alarm_description = alarm_description;
            alarm.sec=sec;
            alarm.nanosec=nanosec;
            alarm.vmd_id=vmd_id;
            alarm.model=model;
            alarm.alarm_priority=alarm_priority;
            alarm.alarm_code=alarm_code;
            alarm.alarm_no=alarm_no;
            alarm.channel_id=channel_id;
            alarm.alarm_state=alarm_state;
            bool is_exist=0;
            for(auto i=common->md->mdsm.patient_alarm.begin(); i!=common->md->mdsm.patient_alarm.end();++i)
            {
                if(i->patient_id == patient_id && i->alarm_description == alarm_description)
                    {
                    i->sec=sec;
                    is_exist=1;
                    break;
                    }
            }
            if(!is_exist)
                common->md->mdsm.patient_alarm.push_back(alarm);
        }

        for (auto sample :topalarm_samples2)
        {
            Alarm alarm;

/*            std::string alarm_no=sample.valueAtIndex(0).asstring(); //警告訊息從資料庫拿
            std::string channel_id=sample.valueAtIndex(1).asstring();
            std::string vmd_id=sample.valueAtIndex(2).asstring();
            std::string patient_id=sample.valueAtIndex(3).asstring();
            std::string alarm_code=sample.valueAtIndex(4).asstring();
            std::string alarm_description=sample.valueAtIndex(5).asstring();
            std::string alarm_priority=sample.valueAtIndex(6).asstring();
            std::string alarm_state=sample.valueAtIndex(7).asstring();
            uint64_t sec = sample.valueAtIndex(8).asInt();
            uint64_t nanosec = sample.valueAtIndex(9).asInt(); */
            dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
            rti::core::xtypes::LoanedDynamicData loaned_member = data.loan_value("source_timestamp");
            uint64_t sec = (time_t)loaned_member.get().value<int32_t>("sec");
            uint64_t nanosec = (time_t)loaned_member.get().value<uint32_t>("nanosec");
            loaned_member.return_loan();
            std::string alarm_no=data.value<std::string>("alarm_no").c_str();
            std::string channel_id=data.value<std::string>("channel_id").c_str();
            std::string vmd_id=data.value<std::string>("vmd_id").c_str();
            std::string model=data.value<std::string>("model").c_str();
            std::string patient_id=data.value<std::string>("patient_id").c_str();
            std::string alarm_priority=data.value<std::string>("alarm_priority").c_str();
            std::string alarm_code=data.value<std::string>("alarm_code").c_str();
            std::string alarm_description=data.value<std::string>("alarm_description").c_str();
            std::string alarm_state=data.value<std::string>("alarm_state").c_str();

            alarm.patient_id = patient_id;
            alarm.alarm_description = alarm_description;
            alarm.sec=sec;
            alarm.nanosec=nanosec;
            alarm.vmd_id=vmd_id;
            alarm.model=model;
            alarm.alarm_priority=alarm_priority;
            alarm.alarm_code=alarm_code;
            alarm.alarm_no=alarm_no;
            alarm.channel_id=channel_id;
            alarm.alarm_state=alarm_state;
            bool is_exist=0;
            for(auto i=common->md->mdsm.technical_alarm.begin(); i!=common->md->mdsm.technical_alarm.end();++i)
            {
                if(i->patient_id == patient_id && i->alarm_description == alarm_description)
                    {
                    i->sec=sec;
                    is_exist=1;
                    break;
                    }
            }
            if(!is_exist)
                common->md->mdsm.technical_alarm.push_back(alarm);
        }
        //刪除過期警告
/*      std::string dummy;
        std::string sql2 = "SELECT meta().alarm_no,channel_id,vmd_id,patient_id,alarm_code,alarm_description,alarm_priority,alarm_state,source_timestamp.sec,source_timestamp.nanosec FROM _ WHERE data_source='NumericDeviceSelection'";
        cbl::ResultSet results2 = common->cbl->queryDocuments(common->display_items_db, sql2, dummy); */

        if(common->md->mdsm.patient_alarm.size() >0)
        {
        for(auto i=common->md->mdsm.patient_alarm.begin(); i!=common->md->mdsm.patient_alarm.end();)
        {
            std::string querystr = "patient_id MATCH '";
            querystr.append(i->patient_id);
            querystr.append("' AND channel_id MATCH '");
            querystr.append(i->channel_id);
            querystr.append("' AND alarm_code MATCH '");
            querystr.append(i->alarm_code);
            querystr.append("' AND alarm_state MATCH ''");
            dds::sub::cond::QueryCondition cond2(
                        dds::sub::Query(common->topalarm_reader, querystr),
                        dds::sub::status::DataState(
                        dds::sub::status::SampleState::any(),
                        dds::sub::status::ViewState::any(),
                        dds::sub::status::InstanceState::alive()));
            dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> patient_samples = common->topalarm_reader.select().condition(cond2).read();
            if(patient_samples.length() == 0)
            {
                common->md->mdsm.patient_alarm.erase(i); break;
                if(i == common->md->mdsm.patient_alarm.end()) break;
            }
            std::string dummy;
            std::string sql = "SELECT handled_time FROM _ WHERE data_source='HandledAlarm'";
            sql.append(" AND patient_id='");
            sql.append(i->patient_id);
            sql.append("' AND channel_id='");
            sql.append(i->channel_id);
            sql.append("' AND alarm_description='");
            sql.append(i->alarm_description);
            sql.append("'");
            cbl::ResultSet results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
            bool is_erase=0;
            for(auto& result: results)
            {
                uint64_t sec = result.valueAtIndex(0).asInt();
                if(sec > i->sec)
                {
                    i = common->md->mdsm.patient_alarm.erase(i);
                    is_erase=1;break;
                }
            }
            if(!is_erase) ++i;
            if(i == common->md->mdsm.patient_alarm.end()) break;

        }
        }
        if(common->md->mdsm.technical_alarm.size() >0)
        {
            for(auto i=common->md->mdsm.technical_alarm.begin(); i!=common->md->mdsm.technical_alarm.end();)
            {
                std::string querystr = "patient_id MATCH '";
                querystr.append(i->patient_id);
                querystr.append("' AND channel_id MATCH '");
                querystr.append(i->channel_id);
                querystr.append("' AND alarm_code MATCH '");
                querystr.append(i->alarm_code);
                querystr.append("' AND alarm_state MATCH ''");
                dds::sub::cond::QueryCondition cond2(
                            dds::sub::Query(common->topalarm_reader_2, querystr),
                            dds::sub::status::DataState(
                            dds::sub::status::SampleState::any(),
                            dds::sub::status::ViewState::any(),
                            dds::sub::status::InstanceState::alive()));
                dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> patient_samples = common->topalarm_reader_2.select().condition(cond2).read();
                if(patient_samples.length() == 0)
                    {
                    common->md->mdsm.technical_alarm.erase(i);break;
                    if(i == common->md->mdsm.technical_alarm.end()) break;
                    }
                std::string dummy;
                std::string sql = "SELECT handled_time FROM _ WHERE data_source='HandledAlarm'";
                sql.append(" AND patient_id='");
                sql.append(i->patient_id);
                sql.append("' AND channel_id='");
                sql.append(i->channel_id);
                sql.append("' AND alarm_description='");
                sql.append(i->alarm_description);
                sql.append("'");
                cbl::ResultSet results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
                bool is_erase=0;
                for(auto& result: results)
                {
                    uint64_t sec = result.valueAtIndex(0).asInt();
                    if(sec > i->sec)
                    {
                        common->md->mdsm.technical_alarm.erase(i);
                        is_erase=1;break;
                    }
                }
                if(!is_erase) ++i;
                if(i == common->md->mdsm.technical_alarm.end()) break;
            }
        }
        //
        common->monitor_page->update_MDS();
        }

}
