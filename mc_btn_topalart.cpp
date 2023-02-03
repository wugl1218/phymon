#include "mc_btn_topalart.h"
#include "MainDialog.h"
#include "ui_MainDialog.h"

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>

mc_btn_topalart::mc_btn_topalart(QWidget *parent): QLabel(parent)
{
}

void mc_btn_topalart::mousePressEvent(QMouseEvent *event)
{
    Common* common = Common::instance();
    if(alarm_description=="")return;
    emit clicked();
   /* if(common->is_server)
    {
        common->msg.setText("Please go to bed side for muting");
        common->msg.exec();
        return;
    }*/
    mute.exec();
    mutetime= mute.get_mutetime();
    if(is_patient_alarm)
        {
            dds::core::xtypes::DynamicData sample(common->topalarm_type);
            sample.value<std::string>("alarm_no", alarm_no);
            sample.value<std::string>("channel_id", channel_id);
            sample.value<std::string>("patient_id", common->patient_id);
            sample.value<std::string>("vmd_id", common->vmd_id);
            sample.value<std::string>("model", model);
            sample.value<std::string>("alarm_code", alarm_code);
            sample.value<std::string>("alarm_description", alarm_description);
            sample.value<std::string>("alarm_priority", alarm_priority);
            sample.value<std::string>("alarm_state", "handled");
            sample.value<std::string>("model", model);
            rti::core::xtypes::LoanedDynamicData loaned_member = sample.loan_value("source_timestamp");
            loaned_member.get().value("sec", (int32_t)sec);
            loaned_member.get().value("nanosec", (uint32_t)nanosec);
            loaned_member.return_loan();
            common->techalert_writer.write(sample);

            std::string Uid ="";
            std::string dummy;
            std::string sql = "SELECT meta().id FROM _ WHERE (data_source='HandledAlarm')";
            sql.append(" AND patient_id='");
            sql.append(common->patient_id);
            sql.append("' AND channel_id='");
            sql.append(channel_id);
            sql.append("' AND alarm_no='");
            sql.append(alarm_no);
            sql.append("'");
            cbl::ResultSet results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
            for(auto& result: results)
                Uid = result.valueAtIndex(0).asstring();

            rapidjson::Document d;
            d.SetObject();
            d.AddMember("data_source", "HandledAlarm", d.GetAllocator());
            d.AddMember("alarm_no", rapidjson::Value().SetString(alarm_no.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("channel_id", rapidjson::Value().SetString(channel_id.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("patient_id", rapidjson::Value().SetString(common->patient_id.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("vmd_id", rapidjson::Value().SetString(common->vmd_id.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("alarm_code", rapidjson::Value().SetString(alarm_code.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("alarm_description", rapidjson::Value().SetString(alarm_priority.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("alarm_priority", rapidjson::Value().SetString(alarm_priority.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("alarm_state", rapidjson::Value().SetString("handled", d.GetAllocator()), d.GetAllocator());
            d.AddMember("handled_time",time(NULL)+mutetime*60, d.GetAllocator());
            rapidjson::Value val;
            val.SetObject();
            val.AddMember("sec", sec, d.GetAllocator());
            val.AddMember("nanosec", nanosec, d.GetAllocator());
            d.AddMember("source_timestamp", val, d.GetAllocator());
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            d.Accept(writer);
            common->cbl->saveMutableDocument(common->display_items_db, buffer.GetString(), Uid, dummy);

            rapidjson::Document d1;
            d1.SetObject();
            d1.AddMember("data_source", "PatientAlert", d1.GetAllocator());
            d1.AddMember("alarm_no", rapidjson::Value().SetString(alarm_no.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("channel_id", rapidjson::Value().SetString(channel_id.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("patient_id", rapidjson::Value().SetString(common->patient_id.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("vmd_id", rapidjson::Value().SetString(common->vmd_id.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("alarm_code", rapidjson::Value().SetString(alarm_code.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("alarm_description", rapidjson::Value().SetString(alarm_description.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("alarm_priority", rapidjson::Value().SetString(alarm_priority.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("alarm_state", rapidjson::Value().SetString("handled", d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("source_timestamp", val, d1.GetAllocator());
            rapidjson::Value val1;
            val1.SetObject();
            val1.AddMember("sec", time(0), d1.GetAllocator());
            val1.AddMember("nanosec", time(0)*1000, d1.GetAllocator());
            d1.AddMember("click_timestamp", val1, d1.GetAllocator());
            rapidjson::StringBuffer buffer1;
            rapidjson::Writer<rapidjson::StringBuffer> writer1(buffer1);
            d1.Accept(writer1);
            //common->cbl->saveMutableDocument(common->db, buffer1.GetString(), Uid, dummy);
        }
    else
        {
            dds::core::xtypes::DynamicData sample(common->techalert_type);
            sample.value<std::string>("alarm_no", alarm_no);
            sample.value<std::string>("channel_id", channel_id);
            sample.value<std::string>("patient_id", common->patient_id);
            sample.value<std::string>("vmd_id", common->vmd_id);
            sample.value<std::string>("alarm_code", alarm_code);
            sample.value<std::string>("alarm_description", alarm_description);
            sample.value<std::string>("alarm_priority", alarm_priority);
            sample.value<std::string>("alarm_state", "handled");
            sample.value<std::string>("model", model);
            rti::core::xtypes::LoanedDynamicData loaned_member = sample.loan_value("source_timestamp");
            loaned_member.get().value("sec", (int32_t)sec);
            loaned_member.get().value("nanosec", (uint32_t)nanosec);
            loaned_member.return_loan();
            common->techalert_writer.write(sample);

            std::string Uid ="";
            std::string dummy;
            std::string sql = "SELECT meta().id FROM _ WHERE (data_source='HandledAlarm')";
            sql.append(" AND patient_id='");
            sql.append(common->patient_id);
            sql.append("' AND channel_id='");
            sql.append(channel_id);
            sql.append("' AND alarm_code='");
            sql.append(alarm_code);
            sql.append("'");
            cbl::ResultSet results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
            for(auto& result: results)
                Uid = result.valueAtIndex(0).asstring();
            rapidjson::Document d;
            d.SetObject();
            d.AddMember("data_source", "HandledAlarm", d.GetAllocator());
            d.AddMember("alarm_no", rapidjson::Value().SetString(alarm_no.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("channel_id", rapidjson::Value().SetString(channel_id.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("patient_id", rapidjson::Value().SetString(common->patient_id.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("vmd_id", rapidjson::Value().SetString(common->vmd_id.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("alarm_code", rapidjson::Value().SetString(alarm_code.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("alarm_description", rapidjson::Value().SetString(alarm_description.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("alarm_priority", rapidjson::Value().SetString(alarm_priority.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("alarm_state", rapidjson::Value().SetString("handled", d.GetAllocator()), d.GetAllocator());
            d.AddMember("handled_time",time(NULL)+mutetime*60, d.GetAllocator());
            rapidjson::Value val;
            val.SetObject();
            val.AddMember("sec", sec, d.GetAllocator());
            val.AddMember("nanosec", nanosec, d.GetAllocator());
            d.AddMember("source_timestamp", val, d.GetAllocator());
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            d.Accept(writer);
            common->cbl->saveMutableDocument(common->display_items_db, buffer.GetString(), Uid, dummy);

            rapidjson::Document d1;
            d1.SetObject();
            d1.AddMember("data_source", "TechnicalAlert", d1.GetAllocator());
            d1.AddMember("alarm_no", rapidjson::Value().SetString(alarm_no.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("channel_id", rapidjson::Value().SetString(channel_id.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("patient_id", rapidjson::Value().SetString(common->patient_id.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("vmd_id", rapidjson::Value().SetString(common->vmd_id.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("alarm_code", rapidjson::Value().SetString(alarm_code.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("alarm_description", rapidjson::Value().SetString(alarm_priority.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("alarm_priority", rapidjson::Value().SetString(alarm_priority.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("alarm_state", rapidjson::Value().SetString("handled", d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("source_timestamp", val, d1.GetAllocator());
            rapidjson::Value val1;
            val1.SetObject();
            val1.AddMember("sec", time(0), d1.GetAllocator());
            val1.AddMember("nanosec", time(0)*1000, d1.GetAllocator());
            d1.AddMember("click_timestamp", val1, d1.GetAllocator());
            rapidjson::StringBuffer buffer1;
            rapidjson::Writer<rapidjson::StringBuffer> writer1(buffer1);
            d1.Accept(writer1);
            //common->cbl->saveMutableDocument(common->db, buffer1.GetString(), Uid, dummy);
        }
    alarm_description="";
    emit pressed();
}

void mc_btn_topalart::mouseReleaseEvent(QMouseEvent *event)
{
    emit released();
}

void mc_btn_topalart::setalarm(bool is_patient_alarm,TOPAlarm alarm)
{
    this->is_patient_alarm=is_patient_alarm;
    this->alarm_description=alarm.alarm_description;
    this->alarm_priority=alarm.alarm_priority;
    this->alarm_code=alarm.alarm_code;
    this->alarm_no=alarm.alarm_no;
    this->channel_id=alarm.channel_id;
    this->model=alarm.model;
    this->alarm_state=alarm.alarm_state;;
    this->sec=alarm.sec;
    this->nanosec=alarm.nanosec;
}
