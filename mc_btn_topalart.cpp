#include "mc_btn_topalart.h"
#include "MainDialog.h"
#include "ui_MainDialog.h"

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>

mc_btn_topalart::mc_btn_topalart(QWidget *parent): QLabel(parent)
{
    circle = new QLabel(this);
    circle->hide();
    QPixmap *pixmap = new QPixmap(":/icons/yellowcircle.png");
    pixmap->scaled(circle->size(), Qt::KeepAspectRatio);
    circle->setScaledContents(true);
    circle->setPixmap(*pixmap);
    circle->installEventFilter(this);
}

bool mc_btn_topalart::eventFilter(QObject *watched, QEvent *event)
{
    Common* common = Common::instance();
    if(circle == watched) {
            if(QEvent::MouseButtonPress == event->type()) {            //鼠标點擊
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event); // 事件转换
                if(mouseEvent->button() == Qt::LeftButton)
                {
                    circle->hide();
                    dds::core::xtypes::DynamicData sample(common->useractions_type);
                    sample.value<std::string>("user_id", "matecares");
                    sample.value<std::string>("patient_id", common->patient_id);
                    if(common->is_server)
                        sample.value<std::string>("function_id", "header_alarm_NS");
                    else
                        sample.value<std::string>("function_id", "header_alarm_BS");
                    sample.value<std::string>("action", "check");
                    sample.value<std::string>("exec_code", "IM000");
                    sample.value<std::string>("exec_msg", "check");
                    rti::core::xtypes::LoanedDynamicData loaned_member = sample.loan_value("exec_timestamp");
                    struct timespec ts;
                    clock_gettime(CLOCK_REALTIME, &ts);
                    loaned_member.get().value("sec", (int32_t)ts.tv_sec);
                    loaned_member.get().value("nanosec", (uint32_t)ts.tv_nsec);
                    loaned_member.return_loan();
                    common->useractions_writer.write(sample);
                    return true;
                }
                return true;
            }
        }
    return QWidget::eventFilter(watched, event);
}

void mc_btn_topalart::mousePressEvent(QMouseEvent *event)
{
    Common* common = Common::instance();
    if(alarm_description=="")return;
    emit clicked();
    if(common->is_server)
    {
        common->msg.setText("Please mute alarm at bed side");
        common->msg.exec();
        return;
    }
    common->md->mute.exec();
    mutetime= common->md->mute.get_mutetime();
    if(is_patient_alarm)
        {
            dds::core::xtypes::DynamicData sample(common->topalarm_type);
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
            common->topalarm_writer.write(sample);

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
            cbl::ResultSet results= common->cbl->queryDocuments(common->display_items_db, sql, dummy);
            while (dummy!="IP200")
                {
                results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
                qDebug()<<QString::fromStdString(dummy);
                fflog_out(common->log,dummy.c_str());
                }            for(auto& result: results)
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
            d.AddMember("alarm_state", "handled", d.GetAllocator());
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
            /* 不寫LDB
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
            d1.AddMember("alarm_state", "handled", d.GetAllocator());
            d1.AddMember("source_timestamp", val, d1.GetAllocator());
            rapidjson::Value val1;
            val1.SetObject();
            val1.AddMember("sec", time(0), d1.GetAllocator());
            val1.AddMember("nanosec", time(0)*1000, d1.GetAllocator());
            d1.AddMember("check_timestamp", val1, d1.GetAllocator());
            rapidjson::StringBuffer buffer1;
            rapidjson::Writer<rapidjson::StringBuffer> writer1(buffer1);
            d1.Accept(writer1);
            common->cbl->saveMutableDocument(common->db, buffer1.GetString(), Uid, dummy);*/
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
            cbl::ResultSet results= common->cbl->queryDocuments(common->display_items_db, sql, dummy);
            while (dummy!="IP200")
                {
                results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
                qDebug()<<QString::fromStdString(dummy);
                fflog_out(common->log,dummy.c_str());
                }
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
            d.AddMember("alarm_state","handled", d.GetAllocator());
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
            /* 不寫LDB
            rapidjson::Document d1;
            d1.SetObject();
            d1.AddMember("data_source", "TechnicalAlert", d1.GetAllocator());
            d1.AddMember("alarm_no", rapidjson::Value().SetString(alarm_no.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("channel_id", rapidjson::Value().SetString(channel_id.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("patient_id", rapidjson::Value().SetString(common->patient_id.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("vmd_id", rapidjson::Value().SetString(common->vmd_id.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("alarm_code", rapidjson::Value().SetString(alarm_code.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("alarm_description", rapidjson::Value().SetString(alarm_description.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("alarm_priority", rapidjson::Value().SetString(alarm_priority.c_str(), d1.GetAllocator()), d1.GetAllocator());
            d1.AddMember("alarm_state", "handled", d.GetAllocator());
            d1.AddMember("source_timestamp", val, d1.GetAllocator());
            rapidjson::Value val1;
            val1.SetObject();
            val1.AddMember("sec", time(0), d1.GetAllocator());
            val1.AddMember("nanosec", time(0)*1000, d1.GetAllocator());
            d1.AddMember("check_timestamp", val1, d1.GetAllocator());
            rapidjson::StringBuffer buffer1;
            rapidjson::Writer<rapidjson::StringBuffer> writer1(buffer1);
            d1.Accept(writer1);
            common->cbl->saveMutableDocument(common->db, buffer1.GetString(), Uid, dummy);*/
        }
    emit pressed();
}

void mc_btn_topalart::mouseReleaseEvent(QMouseEvent *event)
{
    emit released();
}

void mc_btn_topalart::setalarm(bool is_patient_alarm,TOPAlarm alarm)
{    this->sec=alarm.sec;

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
void mc_btn_topalart::set_mute_sheet(bool mute)
{
    Common* common = Common::instance();
    if(mute)
        this->setStyleSheet(common->css.topalarm_mute_background);
    else
        this->setStyleSheet(common->css.topalarm_background);

}
