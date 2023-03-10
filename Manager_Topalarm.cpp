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
    UI_name <<common->md->ui->topalarm_label_1
            <<common->md->ui->topalarm_label_2
            <<common->md->ui->topalarm_label_3
            <<common->md->ui->topalarm_label_4;
}
void Manager_Topalarm::step()
{ 
    Common* common = Common::instance();

    for(auto it=common->md->dm.devices.begin();it !=common->md->dm.devices.end() ;++it)

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
        for(auto i :UI_name)
            i->setText("");
        common->md->efx->stop();
        return;
        }
    bool is_null =1;
    for(auto i :UI_name)
    {
        if(i->text()!="" ||common->md->dm.devices.size()==0)
            is_null=0;
    }
    if(is_null||manager_is_mute)
        common->md->efx->stop();
    uint32_t current_time = Common::get_time_ms();
    if(Common::get_elapsed_time(current_time, last_query_time) > (uint32_t)common->Alarmloop_interval*1000)
    {
        qDebug()<<"Manager_Topalarm";
        fflog_out(common->log,"Info :: Manager_Topalarm");
        last_query_time = current_time;
        manager_is_mute=1;
        for(int i =0;i<common->md->dm.devices.size();++i)
        {
            auto j=common->md->dm.devices.begin();
            topalarm(UI_name[i],j->second);
            UI_set(UI_name[i],j->second);
            ++j;
        }
       if(!manager_is_mute)
           common->md->efx->play();
       else
           common->md->efx->stop();        
    }
}
void Manager_Topalarm::topalarm(mc_btn_topalart *label ,Device devices)
{
    Common* common = Common::instance();
    std::string querystr = "vmd_id MATCH '";
    querystr.append(common->vmd_id);
    querystr.append("' AND patient_id MATCH '");
    querystr.append(common->patient_id);
    querystr.append("' AND channel_id MATCH '");
    querystr.append(devices.channel_id);
    querystr.append("' AND source_timestamp.sec > ");
    querystr.append(QString::number(time(NULL)-6).toStdString());
    dds::sub::cond::QueryCondition topalarm_cond(
                dds::sub::Query(common->topalarm_reader, querystr),
                dds::sub::status::DataState(
                dds::sub::status::SampleState::not_read(),
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
    dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> topalarm_samples = common->topalarm_reader.select().condition(topalarm_cond).read();
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
    for(auto i=top_patient_alarm.begin(); i!=top_patient_alarm.end();)
    {
        if(i == top_patient_alarm.end()||top_patient_alarm.size()==0) break;
        std::string querystr = "patient_id MATCH '";
        querystr.append(common->patient_id);
        querystr.append("' AND alarm_description MATCH '");
        querystr.append(i->second.alarm_description);
        querystr.append("' AND channel_id MATCH '");
        querystr.append(devices.channel_id);
        querystr.append("'");
        dds::sub::cond::QueryCondition cond2(
                    dds::sub::Query(common->topalarm_reader, querystr),
                    dds::sub::status::DataState(
                    dds::sub::status::SampleState::any(),
                    dds::sub::status::ViewState::any(),
                    dds::sub::status::InstanceState::alive()));
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> patient_samples = common->topalarm_reader.select().condition(cond2).read();
        bool is_erase=0;
        if(i->second.sec>=m &&patient_samples.length() != 0)
        {/*
            std::string dummy;
            std::string sql = "SELECT handled_time FROM _ WHERE (data_source='HandledAlarm')";
            sql.append(" AND patient_id='");
            sql.append(i->second.patient_id);
            sql.append("' AND channel_id='");
            sql.append(devices.channel_id);
            sql.append("' AND alarm_description='");
            sql.append(i->second.alarm_description);
            sql.append("'");
        cbl::ResultSet results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
        int error=0;while (dummy!="IP200"&&error<5)
            {
            results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
            qDebug()<<QString::fromStdString(dummy);
            fflog_out(common->log,dummy.c_str());error++;
            }               bool is_results=0;
            for(auto& result: results)
            {
                is_results=1;
                if(i == top_patient_alarm.end()||top_patient_alarm.size()==0) break;
                uint64_t sec = result.valueAtIndex(0).asInt();
                if(i->second.sec<sec)
                {
                    manager_is_mute=0;
                    label->set_mute_sheet(0);
                    break;
                }
                else
                {
                    label->set_mute_sheet(1);
                    break;
                }
            }
            if(!is_results)
            {
                manager_is_mute=0;
                label->set_mute_sheet(0);
            }*/
         }
        else
        {
            i = top_patient_alarm.erase(i);
            is_erase=1;
        }
        if(!is_erase) ++i;
        if(i == top_patient_alarm.end()||top_patient_alarm.size()==0) break;
        }
    for(auto i=top_technical_alarm.begin(); i!=top_technical_alarm.end();)
    {
        if(i == top_technical_alarm.end()||top_technical_alarm.size()==0) break;
        std::string querystr = "patient_id MATCH '";
        querystr.append(common->patient_id);
        querystr.append("' AND alarm_description MATCH '");
        querystr.append(i->second.alarm_description);
        querystr.append("' AND channel_id MATCH '");
        querystr.append(devices.channel_id);
        querystr.append("'");
        dds::sub::cond::QueryCondition cond2(
                    dds::sub::Query(common->topalarm_reader_2, querystr),
                    dds::sub::status::DataState(
                    dds::sub::status::SampleState::any(),
                    dds::sub::status::ViewState::any(),
                    dds::sub::status::InstanceState::alive()));
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> patient_samples = common->topalarm_reader_2.select().condition(cond2).read();
        bool is_erase=0;
        if(i->second.sec>=m &&patient_samples.length() != 0)
        {/*
            std::string dummy;
            std::string sql = "SELECT handled_time FROM _ WHERE (data_source='HandledAlarm')";
            sql.append(" AND patient_id='");
            sql.append(i->second.patient_id);
            sql.append("' AND channel_id='");
            sql.append(devices.channel_id);
            sql.append("' AND alarm_code='");
            sql.append(i->second.alarm_code);
            sql.append("'");
        cbl::ResultSet results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
        int error=0;while (dummy!="IP200"&&error<5)
            {
            results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
            qDebug()<<QString::fromStdString(dummy);
            fflog_out(common->log,dummy.c_str());error++;
            }               bool is_results=0;
            for(auto& result: results)
            {
                is_results=1;
                if(i == top_technical_alarm.end()||top_technical_alarm.size()==0) break;
                uint64_t sec = result.valueAtIndex(0).asInt();
                if(i->second.sec>=sec)
                {
                    manager_is_mute=0;
                    label->set_mute_sheet(0);
                    break;
                }
                else
                {
                    label->set_mute_sheet(1);
                    break;
                }
            }
            if(!is_results)
                {
                manager_is_mute=0;
                label->set_mute_sheet(0);
                }*/
        }
        else
        {
            i = top_technical_alarm.erase(i);
            is_erase=1;
        }
        if(!is_erase) ++i;
        if(i == top_technical_alarm.end()||top_technical_alarm.size()==0) break;
    }
    //?????????????????????????????????????????? ???????????????????????? map???????????? ????????????


}
void Manager_Topalarm::UI_set(mc_btn_topalart *label ,Device devices)
{
    Common* common = Common::instance();
    if(top_patient_alarm.size()>0) //????????????????????????
    {
        auto i=top_patient_alarm.begin();
        std::string msg = i->second.alarm_description;
        msg.append(" (");
        msg.append(i->second.model);
        msg.append(")");
        label->setText(msg.c_str());
        label->setalarm(1,i->second);
        label->circle->show();
        label->is_check=0;
        label->circle->setGeometry(label->width()-45,label->height()/2-21,42,42);
        label->circle_time=i->second.sec;
        std::string dummy;
        std::string sql = "SELECT handled_time FROM _ WHERE (data_source='HandledAlarm')";
        sql.append(" AND patient_id='");
        sql.append(i->second.patient_id);
        sql.append("' AND channel_id='");
        sql.append(devices.channel_id);
        sql.append("' AND alarm_code='");
        sql.append(i->second.alarm_code);
        sql.append("'");
        cbl::ResultSet results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
        int error=0;while (dummy!="IP200"&&error<5)
            {
            results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
            qDebug()<<QString::fromStdString(dummy);
            fflog_out(common->log,dummy.c_str());error++;
            }           bool is_results=0;
        for(auto& result: results)
        {
            is_results=1;
            uint64_t sec = result.valueAtIndex(0).asInt();
            if(i->second.sec>=sec)
            {
                manager_is_mute=0;
                label->set_mute_sheet(0);
                break;
            }
            else
            {
                label->set_mute_sheet(1);
                break;
            }
        }
        if(!is_results)
        {
            manager_is_mute=0;
            label->set_mute_sheet(0);
        }
    }
    else if (top_technical_alarm.size()>0)
    {
        auto i=top_technical_alarm.begin();
        std::string msg = i->second.alarm_description;
        msg.append(" (");
        msg.append(i->second.model);
        msg.append(")");
        label->setText(msg.c_str());
        label->setalarm(0,i->second);
        label->circle->show();
        label->is_check=0;
        label->circle->setGeometry(label->width()-45,label->height()/2-21,42,42);
        label->circle_time=i->second.sec;
        std::string dummy;
        std::string sql = "SELECT handled_time FROM _ WHERE (data_source='HandledAlarm')";
        sql.append(" AND patient_id='");
        sql.append(i->second.patient_id);
        sql.append("' AND channel_id='");
        sql.append(devices.channel_id);
        sql.append("' AND alarm_code='");
        sql.append(i->second.alarm_code);
        sql.append("'");
        cbl::ResultSet results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
        int error=0;while (dummy!="IP200"&&error<5)
            {
            results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
            qDebug()<<QString::fromStdString(dummy);
            fflog_out(common->log,dummy.c_str());error++;
            }        bool is_results=0;
        for(auto& result: results)
        {
            is_results=1;
            uint64_t sec = result.valueAtIndex(0).asInt();
            if(i->second.sec>=sec)
            {
                manager_is_mute=0;
                label->set_mute_sheet(0);
                break;
            }
            else
            {
                label->set_mute_sheet(1);
                break;
            }
        }
        if(!is_results)
        {
            manager_is_mute=0;
            label->set_mute_sheet(0);

        }
    }
    else
    {
        label->setText("");
        label->set_mute_sheet(0);
    }
    std::string querystr = "action MATCH '";
    querystr.append("check");
    querystr.append("' AND patient_id MATCH '");
    querystr.append(common->patient_id);
    querystr.append("' AND user_id MATCH '");
    querystr.append(devices.model);
    querystr.append("' AND exec_timestamp.sec > ");    
    querystr.append(QString::number(label->circle_time).toStdString());
    dds::sub::cond::QueryCondition useractions_cond(
                dds::sub::Query(common->useractions_reader, querystr),
                dds::sub::status::DataState(
                dds::sub::status::SampleState::not_read(),
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
    dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> useractions_samples = common->useractions_reader.select().condition(useractions_cond).read();
    for (auto sample :useractions_samples)
    {
        label->circle->hide();
        label->is_check=1;
        bool is_allcheck=1;
        for(int i =0;i<common->md->dm.devices.size();++i)
        {
            if(!UI_name[i]->is_check)
                is_allcheck=0;
        }
        if(is_allcheck)
        {
            dds::core::xtypes::DynamicData sample(common->useractions_type);
            sample.value<std::string>("user_id", "matecares");
            sample.value<std::string>("patient_id", common->patient_id);
            if(common->is_server)
                sample.value<std::string>("function_id", "header_alarm_NS");
            else
                sample.value<std::string>("function_id", "header_alarm_BS");
            sample.value<std::string>("action", "checkALL");
            sample.value<std::string>("exec_code", "IM000");
            sample.value<std::string>("exec_msg", "checkALL");
            rti::core::xtypes::LoanedDynamicData loaned_member = sample.loan_value("exec_timestamp");
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            loaned_member.get().value("sec", (int32_t)ts.tv_sec);
            loaned_member.get().value("nanosec", (uint32_t)ts.tv_nsec);
            loaned_member.return_loan();
            common->useractions_writer.write(sample);
        }
    }
}
