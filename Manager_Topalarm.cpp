#include "Manager_Topalarm.h"
#include "MainDialog.h"


void Manager_Topalarm::init()
{
    Common* common = Common::instance();
    last_query_time = 0;
}
void Manager_Topalarm::step()
{ 

    Common* common = Common::instance();
    if(common->patient_id.size()==0)
        {
        common->md->ui->topalarm_label->setText("");
        common->md->efx->stop();
        return;
        }
    if(common->md->ui->topalarm_label->text()=="" || common->md->is_alarmSound_index==0)
        common->md->efx->stop();
    if(common->md->ui->topalarm_label->text()=="" && common->md->is_alarmSound_index==0)
        common->md->is_alarmSound_index=1;
    uint32_t current_time = Common::get_time_ms();
    if(Common::get_elapsed_time(current_time, last_query_time) > (uint32_t)common->Alarmloop_interval*1000)
    {
    qDebug()<<"Manager_Topalarm";
        fflog_out(common->log,"Manager_Topalarm");
        last_query_time = current_time;
        common->md->ui->topalarm_label->setText("");
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
                loaned_member.return_loan();
                std::string channel_id=data.value<std::string>("channel_id").c_str();
                std::string alarm_description=data.value<std::string>("alarm_description").c_str();
                std::string patient_id=data.value<std::string>("patient_id").c_str();

                alarm.patient_id = patient_id;
                alarm.alarm_description = alarm_description;
                alarm.sec=sec;
                alarm.channel_id=channel_id;

                bool is_exist=0;
                for(auto i=top_patient_alarm.begin(); i!=top_patient_alarm.end();++i)
                {
                    if(i->patient_id == patient_id && i->alarm_description == alarm_description)
                        {
                        i->sec=sec;
                        is_exist=1;
                        break;
                        }
                }
                if(!is_exist)
                    top_patient_alarm.push_back(alarm);
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
                loaned_member.return_loan();
                std::string channel_id=data.value<std::string>("channel_id").c_str();
                std::string alarm_description=data.value<std::string>("alarm_description").c_str();
                std::string patient_id=data.value<std::string>("patient_id").c_str();

                alarm.patient_id = patient_id;
                alarm.alarm_description = alarm_description;
                alarm.sec=sec;
                alarm.channel_id=channel_id;
                bool is_exist=0;
                for(auto i=top_technical_alarm.begin(); i!=top_technical_alarm.end();++i)
                {
                    if(i->patient_id == common->patient_id && i->alarm_description == alarm_description)
                        {
                        i->sec=sec;
                        is_exist=1;
                        break;
                        }
                }
                if(!is_exist)
                    top_technical_alarm.push_back(alarm);

            }
        uint64_t m=time(NULL)-6;
        if(top_patient_alarm.size()>0)
            for(auto i=top_patient_alarm.begin(); i!=top_patient_alarm.end(); i++)
            {
                std::string querystr = "patient_id MATCH '";
                querystr.append(common->patient_id);
                querystr.append("' AND channel_id MATCH '");
                querystr.append(i->channel_id);
                querystr.append("' AND alarm_description MATCH '");
                querystr.append(i->alarm_description);
                querystr.append("'");
                dds::sub::cond::QueryCondition cond2(
                            dds::sub::Query(common->topalarm_reader, querystr),
                            dds::sub::status::DataState(
                            dds::sub::status::SampleState::any(),
                            dds::sub::status::ViewState::any(),
                            dds::sub::status::InstanceState::alive()));
                dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> patient_samples = common->topalarm_reader.select().condition(cond2).read();
                if(i->sec<m ||patient_samples.length() == 0)
                    {
                    top_patient_alarm.erase(i);
                    if(i == top_patient_alarm.end()) break;
                    }

            }
        if(top_technical_alarm.size()>0)
            for(auto i=top_technical_alarm.begin(); i!=top_technical_alarm.end(); i++)
            {
                std::string querystr = "patient_id MATCH '";
                querystr.append(common->patient_id);
                querystr.append("' AND channel_id MATCH '");
                querystr.append(i->channel_id);
                querystr.append("' AND alarm_description MATCH '");
                querystr.append(i->alarm_description);
                querystr.append("'");
                dds::sub::cond::QueryCondition cond2(
                            dds::sub::Query(common->topalarm_reader_2, querystr),
                            dds::sub::status::DataState(
                            dds::sub::status::SampleState::any(),
                            dds::sub::status::ViewState::any(),
                            dds::sub::status::InstanceState::alive()));
                dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> patient_samples = common->topalarm_reader_2.select().condition(cond2).read();
                if(i->sec<m ||patient_samples.length() == 0)
                    {
                    top_technical_alarm.erase(i);
                    if(i == top_technical_alarm.end()) break;
                    }
            }

        if(top_patient_alarm.size()>0)
            for(auto i=top_patient_alarm.begin(); i!=top_patient_alarm.end(); i++)
            {
                std::string msg = i->alarm_description;
                msg.append(" (");
                msg.append(i->channel_id);
                msg.append(")");
                common->md->ui->topalarm_label->setText(msg.c_str());
                if(common->md->ui->topalarm_label->text()=="")
                    common->md->efx->stop();
                else if (common->md->is_alarmSound_index)
                    common->md->efx->play();
                break;
            }
        else if (top_technical_alarm.size()>0)
            for(auto i=top_technical_alarm.begin(); i!=top_technical_alarm.end(); i++)
            {
                std::string msg = i->alarm_description;
                msg.append(" (");
                msg.append(i->channel_id);
                msg.append(")");
                common->md->ui->topalarm_label->setText(msg.c_str());
                if(common->md->ui->topalarm_label->text()=="")
                    common->md->efx->stop();
                else if (common->md->is_alarmSound_index)
                    common->md->efx->play();
                break;

            }
    }

}

