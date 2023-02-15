#include "Manager_MDSConnectivity_Card.h"
#include "Common.h"
#include "qpainter.h"
#include "MainDialog.h"


Manager_MDSConnectivity_Card::Manager_MDSConnectivity_Card(QWidget *parent)
    : QWidget{parent}
{
    setAutoFillBackground(true);
//    setStyleSheet("background:rgb(11, 42, 78);color:rgb(255, 255, 255);");
    title = new QLabel(this);
    title->resize(355, 65);
//    title->resize(320, 60);
    title->setStyleSheet("background:rgb(11, 42, 78);color:rgb(252, 233, 79);");
    title->setFont(QFont("Arial[Mono]",18));
    title->hide();

    circle = new QLabel(this);
    circle->setGeometry(310,0,42,42);
    circle->hide();
    QPixmap *pixmap = new QPixmap(":/icons/bluecircle.png");
    pixmap->scaled(circle->size(), Qt::KeepAspectRatio);
    circle->setScaledContents(true);
    circle->setPixmap(*pixmap);

    circle->installEventFilter(this);
    title->installEventFilter(this);



}

bool Manager_MDSConnectivity_Card::eventFilter(QObject *watched, QEvent *event)
{
    Common* common = Common::instance();
    if(circle == watched || title == watched) {
        if(QEvent::MouseButtonPress == event->type()) {            //鼠标點擊
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event); // 事件转换
            if(mouseEvent->button() == Qt::LeftButton)
            {
                //這邊要寫 把藍色或紅色九宮格變回藍色 以及變更告知警告已處理
                if(is_Equipment_alarm || !has_checked)
                {
                    common->msg.exec();
                    return true;
                }
                /*if(is_patient_alarm)
                    for(auto i=common->md->mdsm.patient_alarm.begin(); i!=common->md->mdsm.patient_alarm.end(); i++)
                    {
                        if(i->patient_id!=id)
                            continue;
                        dds::core::xtypes::DynamicData sample(common->topalarm_type);
                        sample.value<std::string>("alarm_no", i->alarm_no);
                        sample.value<std::string>("channel_id", i->channel_id);
                        sample.value<std::string>("patient_id", id);
                        sample.value<std::string>("vmd_id", vmd);
                        sample.value<std::string>("alarm_code", i->alarm_code);
                        sample.value<std::string>("alarm_description", i->alarm_description);
                        sample.value<std::string>("alarm_priority", i->alarm_priority);
                        sample.value<std::string>("alarm_state", "handled");
                        common->topalarm_writer.write(sample);

                        std::string Uid ="";
                        std::string dummy;
                        std::string sql = "SELECT meta().id FROM _ WHERE (data_source='HandledAlarm')";
                        sql.append(" AND patient_id='");
                        sql.append(i->patient_id);
                        sql.append("' AND model='");
                        sql.append(i->model);
                        sql.append("' AND alarm_code='");
                        sql.append(i->alarm_code);
                        sql.append("'");
        cbl::ResultSet results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
        while (dummy!="IP200")
            {
            results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
            qDebug()<<QString::fromStdString(dummy);
            fflog_out(common->log,dummy.c_str());
            }                           for(auto& result: results)
                            Uid = result.valueAtIndex(0).asstring();

                        rapidjson::Document d;
                        d.SetObject();
                        d.AddMember("data_source", "HandledAlarm", d.GetAllocator());
                        d.AddMember("alarm_no", rapidjson::Value().SetString(i->alarm_no.c_str(), d.GetAllocator()), d.GetAllocator());
                        d.AddMember("channel_id", rapidjson::Value().SetString(i->channel_id.c_str(), d.GetAllocator()), d.GetAllocator());
                        d.AddMember("patient_id", rapidjson::Value().SetString(i->patient_id.c_str(), d.GetAllocator()), d.GetAllocator());
                        d.AddMember("vmd_id", rapidjson::Value().SetString(vmd.c_str(), d.GetAllocator()), d.GetAllocator());
                        d.AddMember("alarm_code", rapidjson::Value().SetString(i->alarm_code.c_str(), d.GetAllocator()), d.GetAllocator());
                        d.AddMember("alarm_description", rapidjson::Value().SetString(i->alarm_description.c_str(), d.GetAllocator()), d.GetAllocator());
                        d.AddMember("alarm_priority", rapidjson::Value().SetString(i->alarm_priority.c_str(), d.GetAllocator()), d.GetAllocator());
                        d.AddMember("alarm_state", rapidjson::Value().SetString("handled", d.GetAllocator()), d.GetAllocator());
                        d.AddMember("handled_time",time(NULL), d.GetAllocator());
                        rapidjson::Value val;
                        val.SetObject();
                        val.AddMember("sec", i->sec, d.GetAllocator());
                        val.AddMember("nanosec", i->nanosec, d.GetAllocator());
                        d.AddMember("source_timestamp", val, d.GetAllocator());
                        rapidjson::StringBuffer buffer;
                        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                        d.Accept(writer);
                        common->cbl->saveMutableDocument(common->display_items_db, buffer.GetString(), Uid, dummy);
                        break;
                    }
                else
                    for(auto i=common->md->mdsm.technical_alarm.begin(); i!=common->md->mdsm.technical_alarm.end(); i++)
                    {
                        if(i->patient_id!=id)
                            continue;
                        dds::core::xtypes::DynamicData sample(common->techalert_type);
                        sample.value<std::string>("alarm_no", i->alarm_no);
                        sample.value<std::string>("channel_id", i->channel_id);
                        sample.value<std::string>("patient_id", id);
                        sample.value<std::string>("vmd_id", vmd);
                        sample.value<std::string>("alarm_code", i->alarm_code);
                        sample.value<std::string>("alarm_description", i->alarm_description);
                        sample.value<std::string>("alarm_priority", i->alarm_priority);
                        sample.value<std::string>("alarm_state", "handled");
                        common->techalert_writer.write(sample);

                        std::string Uid ="";
                        std::string dummy;
                        std::string sql = "SELECT meta().id FROM _ WHERE (data_source='HandledAlarm')";
                        sql.append(" AND patient_id='");
                        sql.append(i->patient_id);
                        sql.append("' AND model='");
                        sql.append(i->model);
                        sql.append("' AND alarm_code='");
                        sql.append(i->alarm_code);
                        sql.append("'");

        cbl::ResultSet results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
        while (dummy!="IP200")
            {
            results = common->cbl-> queryDocuments(common->display_items_db, sql, dummy);
            qDebug()<<QString::fromStdString(dummy);
            fflog_out(common->log,dummy.c_str());
            }                           for(auto& result: results)
                            Uid = result.valueAtIndex(0).asstring();

                        rapidjson::Document d;
                        d.SetObject();
                        d.AddMember("data_source", "HandledAlarm", d.GetAllocator());
                        d.AddMember("alarm_no", rapidjson::Value().SetString(i->alarm_no.c_str(), d.GetAllocator()), d.GetAllocator());
                        d.AddMember("channel_id", rapidjson::Value().SetString(i->channel_id.c_str(), d.GetAllocator()), d.GetAllocator());
                        d.AddMember("patient_id", rapidjson::Value().SetString(i->patient_id.c_str(), d.GetAllocator()), d.GetAllocator());
                        d.AddMember("vmd_id", rapidjson::Value().SetString(vmd.c_str(), d.GetAllocator()), d.GetAllocator());
                        d.AddMember("alarm_code", rapidjson::Value().SetString(i->alarm_code.c_str(), d.GetAllocator()), d.GetAllocator());
                        d.AddMember("alarm_description", rapidjson::Value().SetString(i->alarm_description.c_str(), d.GetAllocator()), d.GetAllocator());
                        d.AddMember("alarm_priority", rapidjson::Value().SetString(i->alarm_priority.c_str(), d.GetAllocator()), d.GetAllocator());
                        d.AddMember("alarm_state", rapidjson::Value().SetString("handled", d.GetAllocator()), d.GetAllocator());
                        d.AddMember("handled_time",time(NULL), d.GetAllocator());
                        rapidjson::Value val;
                        val.SetObject();
                        val.AddMember("sec", i->sec, d.GetAllocator());
                        val.AddMember("nanosec", i->nanosec, d.GetAllocator());
                        d.AddMember("source_timestamp", val, d.GetAllocator());
                        rapidjson::StringBuffer buffer;
                        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                        d.Accept(writer);
                        common->cbl->saveMutableDocument(common->display_items_db, buffer.GetString(), Uid, dummy);
                        break;
                    }*/
            }
                return true;
        }
        else if(QEvent::Enter == event->type()) {            //鼠标进入
            if (title->isHidden()) { //已经隐藏就显示出来
                title->show();
                QPoint point = circle->pos();
                point.rx() = 0;
                point.ry() = 236-65;
                title->move(point);
                title->raise();//显示最顶层
                return true;
            }
        }
        else if (QEvent::Leave == event->type()) { //鼠标离开
            if (!title->isHidden()) {
                if(!circle->geometry().contains(this->mapFromGlobal(QCursor::pos())) //判断鼠标是否在控件上
                   &&!title->geometry().contains(this->mapFromGlobal(QCursor::pos())) )
                {
                    title->hide();
                    return true;
                }
            }
        }

        }

    return QWidget::eventFilter(watched, event);
}

void Manager_MDSConnectivity_Card::update_but()
{
    if(id == "") return;
    CustomButton *releasebtn = new CustomButton(this);
    releasebtn->setGeometry(width()-41,height()-41,60,60);
    if(male)
        releasebtn->set_is_male(1);
    else
        releasebtn->set_is_male(0);
    connect(releasebtn, SIGNAL(signalButtonClicked()), this, SLOT(on_releasebtn_clicked()));
}

void Manager_MDSConnectivity_Card::paintEvent(QPaintEvent *event)
{
    Common* common = Common::instance();
    QPainter painter(this);
    QPen bed_pen(QColor(255,255,255));
    QPen title_pen(QColor(170,170,170));
    QPen value_pen(QColor(210,210,210));
    QFont bed_font ;
    QFont title_font ;
    QFont value_font ;
    bool Show_pen =1;
    bed_font.setPixelSize(32);
    bed_font.setFamily("Arial [Mono]");
    title_font.setPixelSize(28);
    title_font.setFamily("Arial [Mono]");
    value_font.setPixelSize(72);
    value_font.setFamily("Arial [Mono]");
    int title_left_x =7;
    int title_right_x =235;
    int title_above_y =70;
    int title_under_y =220;
    int title_value_interval=58;
    if(id == "")
        {

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(107, 107, 107));
        painter.drawRect(0,0,width(),height());
        painter.setFont(bed_font);
        painter.setPen(bed_pen);
        Common::draw_text(painter, 2,18, Qt::AlignLeft | Qt::AlignVCenter, QString::fromStdString(bed));
        return;
        }
    painter.setPen(Qt::NoPen);
    std::string dummy;
    std::string sql = "SELECT checked FROM _ WHERE data_source='NumericDeviceSelection' AND patient_id='";
    sql.append(id);
    sql.append("' AND meta(_).expiration IS NOT VALUED AND expired=0");
    cbl::ResultSet results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
    while (dummy!="IP200")
        {
        results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
        qDebug()<<QString::fromStdString(dummy);
        fflog_out(common->log,dummy.c_str());
        }       for(auto& result: results)
    {
        uint8_t checked = result.valueAtIndex(0).asUnsigned();
        if (checked)
            has_checked=1;
    }
    if ( RR=="")
    {
        std::string querystr2 = "vmd_id MATCH '";
        querystr2.append(vmd);
        querystr2.append("'");
        dds::sub::cond::QueryCondition cond2(
                    dds::sub::Query(common->devcon_reader, querystr2),
                    dds::sub::status::DataState(
                    dds::sub::status::SampleState::any(),
                    dds::sub::status::ViewState::any(),
                    dds::sub::status::InstanceState::alive()));
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> devcon_samples = common->devcon_reader.select().condition(cond2).read();

        painter.setBrush(QColor(200,37,37));
        circle->setStyleSheet("background:rgb(200,37,37);color:rgb(248, 208, 4);");
        circle->show();
        is_Equipment_alarm =1;
        if(devcon_samples.length() == 1)
        {
            title->setText("Medical Equipment \nDisconnection");
            common->msg.setText("Please click \nMedical Equipment");

        }
        else if (devcon_samples.length() == 0)
        {
            title->setText("MetaCares Box Error");
            common->msg.setText("Please click \nMetaCares Box");
        }
        else
        {
            title->setText("Devices connecting...");
            common->msg.setText("Please wait");
        }
        Show_pen=0;
        title->setWordWrap(true);
    }
    else if(!has_checked)
        {
            painter.setBrush(QColor(200,37,37));
            circle->setStyleSheet("background:rgb(200,37,37);color:rgb(248, 208, 4);");
            circle->show();
            title->setText("Medical Equipment \nUnselected");
            common->msg.setText("Please click \nDisplay setting");
            Show_pen=0;
            title->setWordWrap(true);
        }
    else if(common->md->mdsm.patient_alarm.size() >0)
        for(auto i=common->md->mdsm.patient_alarm.begin(); i!=common->md->mdsm.patient_alarm.end(); i++)
        {
            if(i->patient_id!=id)
                continue;
            painter.setBrush(QColor(252, 233, 79));
            bed_pen.setColor(QColor(1,42,68));
            title_pen.setColor(QColor(61, 102, 128));
            value_pen.setColor(QColor(11, 42, 78));
            circle->setStyleSheet("background:rgb(252, 233, 79);color:rgb(248, 208, 4);");
            //circle->show();
            circle_time=time(0);
            title->setText(QString::fromStdString(i->alarm_description));
            title->setWordWrap(true);
            is_patient_alarm = 1;
            is_Equipment_alarm =0;
            break;
        }
    else if(common->md->mdsm.technical_alarm.size() >0)
        for(auto i=common->md->mdsm.technical_alarm.begin(); i!=common->md->mdsm.technical_alarm.end(); i++)
        {
            if(i->patient_id!=id)
                continue;
            painter.setBrush(QColor(252, 233, 79));
            bed_pen.setColor(QColor(1,42,68));
            title_pen.setColor(QColor(61, 102, 128));
            value_pen.setColor(QColor(11, 42, 78));
            circle->setStyleSheet("background:rgb(252, 233, 79);color:rgb(248, 208, 4);");
            //circle->show();
            circle_time=time(0);
            title->setText(QString::fromStdString(i->alarm_description));
            title->setWordWrap(true);
            is_patient_alarm = 0;
            is_Equipment_alarm =0;

            break;
        }
    else
    {
        painter.setBrush(QColor(11, 42, 78));
        QPixmap *pixmap = new QPixmap(":/icons/yellowcircle.png");
        pixmap->scaled(circle->size(), Qt::KeepAspectRatio);
        circle->setScaledContents(true);
        circle->setPixmap(*pixmap);
        title->hide();
        if(circle_time!=0)
            circle->show();
    }
    painter.drawRect(0,0,width(),height());
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    if(circle_time!=0)
    {
        std::string querystr = "action MATCH '";
        querystr.append("checkALL");
        querystr.append("' AND patient_id MATCH '");
        querystr.append(id);
/*多設備四個燈號皆取消要在想        querystr.append("' AND user_id MATCH '");
        querystr.append(devices.model);*/
        querystr.append("' AND exec_timestamp.sec > ");
        querystr.append(QString::number(circle_time).toStdString());
        dds::sub::cond::QueryCondition useractions_cond(
                    dds::sub::Query(common->useractions_reader, querystr),
                    dds::sub::status::DataState(
                    dds::sub::status::SampleState::not_read(),
                    dds::sub::status::ViewState::any(),
                    dds::sub::status::InstanceState::alive()));
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> useractions_samples = common->useractions_reader.select().condition(useractions_cond).read();
         for (auto sample :useractions_samples)
             circle->hide();
    }




    painter.setFont(bed_font);
    painter.setPen(bed_pen);
    if(bed!="")
        Common::draw_text(painter, 2,18, Qt::AlignLeft | Qt::AlignVCenter, QString::fromStdString(bed));


    painter.scale(0.7, 0.7);
    if(Show_pen)
    {
        painter.setFont(title_font);
        painter.setPen(title_pen);
        Common::draw_text(painter, title_left_x, title_above_y, Qt::AlignLeft | Qt::AlignVCenter, "RR (1/min)");
        Common::draw_text(painter, title_right_x, title_above_y, Qt::AlignLeft | Qt::AlignVCenter, "I:E");
        Common::draw_text(painter, title_left_x, title_under_y, Qt::AlignLeft | Qt::AlignVCenter, "MV (L/min)");
        Common::draw_text(painter, title_right_x, title_under_y, Qt::AlignLeft | Qt::AlignVCenter, "VT (mL)");
        painter.setFont(value_font);
        painter.setPen(value_pen);
        if(RR!="")
        Common::draw_text(painter, title_left_x, title_above_y+title_value_interval, Qt::AlignLeft | Qt::AlignVCenter, QString::fromStdString(RR));
        if(MV!="")
        Common::draw_text(painter, title_right_x, title_above_y+title_value_interval, Qt::AlignLeft | Qt::AlignVCenter, QString::fromStdString(IE));
        if(IE!="")
        Common::draw_text(painter, title_left_x, title_under_y+title_value_interval, Qt::AlignLeft | Qt::AlignVCenter, QString::fromStdString(MV));
        if(VT!="")
        Common::draw_text(painter, title_right_x, title_under_y+title_value_interval, Qt::AlignLeft | Qt::AlignVCenter, QString::fromStdString(VT));
    }



}

void Manager_MDSConnectivity_Card::on_releasebtn_clicked()
{
    Common* common = Common::instance();
    common->rd.ok = 0;
    common->rd.clearText();
    std::string local_bed =bed;
    std::string local_vmd =vmd;

    common->rd.exec();
    std::string employeeID = common->rd.getText();
    if(common->rd.ok && employeeID.size() > 0)
        perform_release(employeeID,local_bed,local_vmd);
}
void Manager_MDSConnectivity_Card::perform_release(std::string employeeID,std::string local_bed ,std::string local_vmd)
{
    Common* common = Common::instance();
    if(employeeID.size() > 0)
    {
        dds::core::xtypes::DynamicData sample(common->mds_type);
        sample.value<std::string>("patient_id", "");
        sample.value<std::string>("vmd_id", local_vmd);
        sample.value<std::string>("room_id", "");
        sample.value<std::string>("bed_id", local_bed);
        common->mds_writer.write(sample);
    }
    if(employeeID.size() > 0)
    {
        common->commit_user_action(employeeID.c_str(), "Monitor", "Release");
    }

}
//================================== card funtion
void Manager_MDSConnectivity_Card::mouseMoveEvent(QMouseEvent *event)
{

}

void Manager_MDSConnectivity_Card::mousePressEvent(QMouseEvent *event)
{
    if(id!="" && RR!="")
        {
        emit clicked();
        }
}

void Manager_MDSConnectivity_Card::mouseReleaseEvent(QMouseEvent *event)
{

}

void Manager_MDSConnectivity_Card::set_is_male(uint8_t is_male)
{
    male = is_male;
    update();
}

uint8_t Manager_MDSConnectivity_Card::is_male()
{
    return male;
}
void Manager_MDSConnectivity_Card::set_bed(std::string bed)
{
    this->bed = bed;
    update();
}

std::string Manager_MDSConnectivity_Card::get_bed()
{
    return bed;
}

void Manager_MDSConnectivity_Card::set_id(std::string id)
{
    this->id = id;
    update();
}
std::string Manager_MDSConnectivity_Card::get_id()
{
    return id;
}

void Manager_MDSConnectivity_Card::set_RR(std::string RR)
{
    this->RR = RR;
    update();
}
void Manager_MDSConnectivity_Card::set_IE(std::string IE)
{
    this->IE = IE;
    update();
}
void Manager_MDSConnectivity_Card::set_MV(std::string MV)
{
    this->MV = MV;
    update();
}
void Manager_MDSConnectivity_Card::set_VT(std::string VT)
{
    this->VT = VT;
    update();
}
std::string Manager_MDSConnectivity_Card::get_RR()
{
    return RR;
}
std::string Manager_MDSConnectivity_Card::get_IE()
{
    return IE;
}
std::string Manager_MDSConnectivity_Card::get_MV()
{
    return MV;
}
std::string Manager_MDSConnectivity_Card::get_VT()
{
    return VT;
}
void Manager_MDSConnectivity_Card::set_selected(uint8_t selected)
{
    this->selected = selected;
    update();
}

uint8_t Manager_MDSConnectivity_Card::is_selected()
{
    return selected;
}
void Manager_MDSConnectivity_Card::set_vmd(std::string vmd)
{
    this->vmd = vmd;
    update();
}
std::string Manager_MDSConnectivity_Card::get_vmd()
{
    return vmd;
}

void Manager_MDSConnectivity_Card::set_alarm(std::map<std::string, int> patient_alarm)
{
    this->patient_alarm = patient_alarm;
}
//=========================================but funtion
CustomButton::CustomButton(QWidget *parent) :QPushButton(parent)
{
}
void CustomButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.scale(0.7, 0.7);
    painter.setBrush(QColor(11, 42, 78));
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    if(male)
        painter.drawPixmap(rect(), QPixmap(":/icons/smallmale.png"));
    else
        painter.drawPixmap(rect(), QPixmap(":/icons/smallfemale.png"));

}

void CustomButton::mouseMoveEvent(QMouseEvent *event)
{

}

void CustomButton::mousePressEvent(QMouseEvent *event)
{
    emit signalButtonClicked();
}

void CustomButton::mouseReleaseEvent(QMouseEvent *event)
{

}
void CustomButton::set_is_male(uint8_t is_male)
{
    male = is_male;
    update();
}

uint8_t CustomButton::is_male()
{
    return male;
}


