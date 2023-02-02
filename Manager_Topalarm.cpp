#include "Manager_Topalarm.h"
#include "MainDialog.h"


void Manager_Topalarm::init()
{
    Common* common = Common::instance();
    last_query_time = 0;
    common->md->ui->topalarm_label_1->setText("");
    common->md->ui->topalarm_label_2->setText("");
    common->md->ui->topalarm_label_3->setText("");
    common->md->ui->topalarm_label_4->setText("");
}
void Manager_Topalarm::step()
{ 

    Common* common = Common::instance();
    if(common->md->dm.devices.size()>=4)
        common->md->ui->topalarm_label_4->show();
    else
        common->md->ui->topalarm_label_4->hide();

    if(common->md->dm.devices.size()>=3)
        common->md->ui->topalarm_label_3->show();
    else
        common->md->ui->topalarm_label_3->hide();

    if(common->md->dm.devices.size()>=2)
        common->md->ui->topalarm_label_2->show();
    else
        common->md->ui->topalarm_label_2->hide();


    if(common->patient_id.size()==0)
        {
        common->md->ui->topalarm_label_1->setText("");
        common->md->efx->stop();
        return;
        }
    if(common->md->ui->topalarm_label_1->text()=="" || common->md->is_alarmSound_index==0)
        common->md->efx->stop();
    if(common->md->ui->topalarm_label_1->text()=="" && common->md->is_alarmSound_index==0)
        common->md->is_alarmSound_index=1;
    uint32_t current_time = Common::get_time_ms();
    if(Common::get_elapsed_time(current_time, last_query_time) > (uint32_t)common->Alarmloop_interval*1000)
    {
    qDebug()<<"Manager_Topalarm";
        fflog_out(common->log,"Manager_Topalarm");
        last_query_time = current_time;
        common->md->ui->topalarm_label_1->setText("");
        std::string querystr = "vmd_id MATCH '";
        querystr.append(common->vmd_id);
        querystr.append("' AND patient_id MATCH '");
        querystr.append(common->patient_id);
        querystr.append("'");
        dds::sub::cond::QueryCondition topalarm_cond(
                    dds::sub::Query(common->topalarm_reader, querystr),
                    dds::sub::status::DataState(
                    dds::sub::status::SampleState::not_read(),
                    dds::sub::status::ViewState::any(),
                    dds::sub::status::InstanceState::alive()));
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> topalarm_samples = common->topalarm_reader.select().condition(topalarm_cond).read();
        if(topalarm_samples.length() > 0)
            for (auto sample :topalarm_samples)
            {
                TOPAlarm alarm;
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
                for(auto i=top_patient_alarm.begin(); i!=top_patient_alarm.end();++i)
                {
                    if(i->second.patient_id == patient_id && i->second.alarm_description == alarm_description)
                        {
                        i->second.sec=sec;
                        is_exist=1;
                        break;
                        }
                }
                if(!is_exist)
                    top_patient_alarm.emplace(QString::fromStdString(alarm.alarm_priority).toInt(),alarm);
            }
        dds::sub::cond::QueryCondition topalarm_cond1(
                    dds::sub::Query(common->topalarm_reader_2, querystr),
                    dds::sub::status::DataState(
                    dds::sub::status::SampleState::any(),
                    dds::sub::status::ViewState::any(),
                    dds::sub::status::InstanceState::alive()));
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> topalarm_samples2 = common->topalarm_reader_2.select().condition(topalarm_cond1).read();
            for (auto sample :topalarm_samples2)
            {
                TOPAlarm alarm;
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
                for(auto i=top_technical_alarm.begin(); i!=top_technical_alarm.end();++i)
                {
                    if(i->second.patient_id == common->patient_id && i->second.alarm_description == alarm_description)
                        {
                        i->second.sec=sec;
                        is_exist=1;
                        break;
                        }
                }
                if(!is_exist)
                    top_technical_alarm.emplace(QString::fromStdString(alarm.alarm_priority).toInt(),alarm);

            }
        uint64_t m=time(NULL)-6;
        if(top_patient_alarm.size()>0)
            for(auto i=top_patient_alarm.begin(); i!=top_patient_alarm.end();)
            {
                if(i == top_patient_alarm.end()||top_patient_alarm.size()==0) break;
                std::string querystr = "patient_id MATCH '";
                querystr.append(common->patient_id);
                querystr.append("' AND model MATCH '");
                querystr.append(i->second.model);
                querystr.append("' AND alarm_description MATCH '");
                querystr.append(i->second.alarm_description);
                querystr.append("'");
                dds::sub::cond::QueryCondition cond2(
                            dds::sub::Query(common->topalarm_reader, querystr),
                            dds::sub::status::DataState(
                            dds::sub::status::SampleState::any(),
                            dds::sub::status::ViewState::any(),
                            dds::sub::status::InstanceState::alive()));
                dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> patient_samples = common->topalarm_reader.select().condition(cond2).read();
                std::string dummy;
                std::string sql = "SELECT handled_time FROM _ WHERE (data_source='HandledAlarm')";
                sql.append(" AND patient_id='");
                sql.append(i->second.patient_id);
                sql.append("' AND channel_id='");
                sql.append(i->second.channel_id);
                sql.append("' AND alarm_description='");
                sql.append(i->second.alarm_description);
                sql.append("'");
                cbl::ResultSet results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
                bool is_erase=0;
                for(auto& result: results)
                {
                    if(i == top_patient_alarm.end()||top_patient_alarm.size()==0) break;
                    uint64_t sec = result.valueAtIndex(0).asInt();
                    if(i->second.sec<m ||patient_samples.length() == 0 ||i->second.sec<sec)
                        {
                        i = top_patient_alarm.erase(i);
                        is_erase=1;break;
                        }

                }
                if(!is_erase) ++i;
                if(i == top_patient_alarm.end()||top_patient_alarm.size()==0) break;
            }
        if(top_technical_alarm.size()>0)
            for(auto i=top_technical_alarm.begin(); i!=top_technical_alarm.end();)
            {
                if(i == top_technical_alarm.end()||top_technical_alarm.size()==0) break;
                std::string querystr = "patient_id MATCH '";
                querystr.append(common->patient_id);
                querystr.append("' AND model MATCH '");
                querystr.append(i->second.model);
                querystr.append("' AND alarm_description MATCH '");
                querystr.append(i->second.alarm_description);
                querystr.append("'");
                dds::sub::cond::QueryCondition cond2(
                            dds::sub::Query(common->topalarm_reader_2, querystr),
                            dds::sub::status::DataState(
                            dds::sub::status::SampleState::any(),
                            dds::sub::status::ViewState::any(),
                            dds::sub::status::InstanceState::alive()));
                dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> patient_samples = common->topalarm_reader_2.select().condition(cond2).read();
                std::string dummy;
                std::string sql = "SELECT handled_time FROM _ WHERE (data_source='HandledAlarm')";
                sql.append(" AND patient_id='");
                sql.append(i->second.patient_id);
                sql.append("' AND channel_id='");
                sql.append(i->second.channel_id);
                sql.append("' AND alarm_code='");
                sql.append(i->second.alarm_code);
                sql.append("'");
                cbl::ResultSet results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
                bool is_erase=0;
                for(auto& result: results)
                {
                    if(i == top_technical_alarm.end()||top_technical_alarm.size()==0) break;
                    uint64_t sec = result.valueAtIndex(0).asInt();
                    if(i->second.sec<m ||patient_samples.length() == 0 ||i->second.sec<sec)
                        {
                        i = top_technical_alarm.erase(i);
                        is_erase=1;break;
                        }

                }
                if(!is_erase) ++i;
                if(i == top_technical_alarm.end()||top_technical_alarm.size()==0) break;
            }
        //這裡新增不同設備判斷排序方式

        if(top_patient_alarm.size()>0) //處理新的訊息 顯示或處於靜音時間
            for(auto i=top_patient_alarm.begin(); i!=top_patient_alarm.end(); i++)
            {
                std::string msg = i->second.alarm_description;
                msg.append(" (");
                msg.append(i->second.model);
                msg.append(")");
                common->md->ui->topalarm_label_1->setText(msg.c_str());
                common->md->ui->topalarm_label_1->setalarm(1,i->second);
                if(common->md->ui->topalarm_label_1->text()=="")
                    common->md->efx->stop();
                else if (common->md->is_alarmSound_index)
                    common->md->efx->play();
                break;
            }
        else if (top_technical_alarm.size()>0)
            for(auto i=top_technical_alarm.begin(); i!=top_technical_alarm.end(); i++)
            {
                std::string msg = i->second.alarm_description;
                msg.append(" (");
                msg.append(i->second.model);
                msg.append(")");
                common->md->ui->topalarm_label_1->setText(msg.c_str());
                common->md->ui->topalarm_label_1->setalarm(0,i->second);
                if(common->md->ui->topalarm_label_1->text()=="")
                    common->md->efx->stop();
                else if (common->md->is_alarmSound_index)
                    common->md->efx->play();
                break;

            }
    }

}

