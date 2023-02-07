#include <QScrollBar>
#include "Tab_Mapping_Widget.h"
#include "qevent.h"
#include "ui_Tab_Devices_Widget.h"
#include "ui_Tab_Mapping_Widget.h"
#include "Common.h"
#include "MainDialog.h"
#include "Tab_Observations_mainPage_Widget.h"
#include "Tab_Observations_historyPage_Widget.h"
#include "ui_Tab_Observations_mainPage_Widget.h"
#include "Tab_Utilities_alarmHistoryPage_Widget.h"
#include "ui_Tab_Utilities_alarmHistoryPage_Widget.h"
#include "ui_Tab_Utilities_exporterPage_Widget.h"
#include "ui_Tab_Utilities_Widget.h"
#include "ui_Dialog_forcerelease.h"


#define CARD_SPACING 37
#define CARD_WIDTH 241
#define CARD_HEIGHT 137

extern uint8_t* mc_mapping_active;

void Tab_Mapping_Widget::update_devices()
{
    Common* common = Common::instance();
    ui->medicalDevice_tableWidget->clearContents();
    if(mc_mapping_active && *mc_mapping_active == 0)
        return;
    ui->medicalDevice_tableWidget->setRowCount(common->md->dm.devices.size());
    int row = 0;
    for(auto it=common->md->dm.devices.begin();it!=common->md->dm.devices.end();it++)
    {
        int col = 0;
        QTableWidgetItem* item1 = new QTableWidgetItem(it->second.manufacturer.c_str());
        item1->setFlags(Qt::ItemIsEnabled);
        ui->medicalDevice_tableWidget->setItem(row, col++, item1);
        QTableWidgetItem* item2 = new QTableWidgetItem(it->second.model.c_str());
        item2->setFlags(Qt::ItemIsEnabled);
        ui->medicalDevice_tableWidget->setItem(row, col++, item2);
        QTableWidgetItem* item3 = new QTableWidgetItem(it->second.desc.c_str());
        item3->setFlags(Qt::ItemIsEnabled);
        ui->medicalDevice_tableWidget->setItem(row, col++, item3);
        row++;
        if(row % 2 ==0)
            {
            item1->setBackground(QBrush(QColor(11, 42, 78)));
            item2->setBackground(QBrush(QColor(11, 42, 78)));
            item3->setBackground(QBrush(QColor(11, 42, 78)));
        }
    }
}

void Tab_Mapping_Widget::update_patients()
{
    Common* common = Common::instance();
    if((mc_mapping_active && *mc_mapping_active == 0) || common->patient_id.size() > 0)
        return;
    for(int i=0;i<(int)cards.size();i++)
        delete cards[i];
    cards.clear();
    int r = 0;
    int c = 0;
    for(auto it=common->md->pm.patients.begin();it!=common->md->pm.patients.end();it++)
    {
        Manager_Patient_Card* card = new Manager_Patient_Card(ui->scrollAreaWidgetContents);
        card->setGeometry(CARD_SPACING+(CARD_WIDTH+CARD_SPACING)*c,
                          CARD_SPACING+(CARD_HEIGHT+CARD_SPACING)*r,
                          CARD_WIDTH, CARD_HEIGHT);
        card->set_bed(it->second.bed);
        card->set_mrn(it->second.mrn);
        card->set_name(it->second.family_name);
        if(it->second.gender.compare("female") == 0)
            card->set_is_male(0);
        else
            card->set_is_male(1);
        Common::set_inactive_colors(card);
        if(!it->second.bound)
        {
            card->show();
            connect(card, SIGNAL(clicked()), this, SLOT(patient_clicked()));
        }
        else
        {
            card->hide();
            c--;
        }
        c++;
        if(c >= 6)
        {
            r++;
            c = 0;
        }
        cards.push_back(card);
    }

/*    for(auto it=common->md->pm.patients.begin();it!=common->md->pm.patients.end();it++)
    {
        Manager_Patient_Card* card = new Manager_Patient_Card(ui->scrollAreaWidgetContents);
        card->setGeometry(CARD_SPACING+(CARD_WIDTH+CARD_SPACING)*c,
                          CARD_SPACING+(CARD_HEIGHT+CARD_SPACING)*r,
                          CARD_WIDTH, CARD_HEIGHT);
        card->set_bed(it->second.bed);
        card->set_mrn(it->second.mrn);
        card->set_name(it->second.family_name);
        if(it->second.gender.compare("female") == 0)
            card->set_is_male(0);
        else
            card->set_is_male(1);
        Common::set_inactive_colors(card);
        card->show();
        connect(card, SIGNAL(clicked()), this, SLOT(patient_clicked()));
        c++;
        if(c >= 6)
        {
            r++;
            c = 0;
        }
        cards.push_back(card);
    }
*/    ui->scrollAreaWidgetContents->adjustSize();
    /*
    QTableWidget* tableWidget = ui->patientList_tableWidget;
    tableWidget->clearContents();
    tableWidget->setRowCount(common->md->pm.patients.size());
    int row = 0;
    for(auto it=common->md->pm.patients.begin();it!=common->md->pm.patients.end();it++)
    {
        int col = 0;
        QTableWidgetItem* item1 = new QTableWidgetItem(it->second.bed.c_str());
        item1->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        tableWidget->setItem(row, col++, item1);
        QTableWidgetItem* item2 = new QTableWidgetItem(it->second.mrn.c_str());
        item2->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        tableWidget->setItem(row, col++, item2);
        QTableWidgetItem* item3 = new QTableWidgetItem(it->second.family_name.c_str());
        item3->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        tableWidget->setItem(row, col++, item3);
        row++;
    }*/  
}

void Tab_Mapping_Widget::patient_clicked()
{
    if(selected_patient != -1)
        return;
    Common* common = Common::instance();
    if(common->md->dm.devices.size() == 0) return;
    int i;
    for(i=0;i<(int)cards.size();i++)
    {
        if(cards[i] == sender())
            break;
    }
    if(i==(int)cards.size())
        return;
    selected_patient = i;
    update();
    on_itemSelectionChanged();
}

Tab_Mapping_Widget::Tab_Mapping_Widget(QWidget *parent) :
    QWidget(parent),
    frd(this),
    ui(new Ui::Tab_Mapping_Widget),
    worker_timer(this)
{
    ui->setupUi(this);
    connect(ui->returnbtn, SIGNAL(clicked()), this, SLOT(on_returnbtn_clicked()));


    Common* common = Common::instance();
    common->mapping_tab = this;
    common->md->ui->main_stackedWidget->setCurrentIndex(0);
    ui->medicalDevice_tableWidget->setColumnCount(3);
    ui->medicalDevice_tableWidget->horizontalHeader()->setFixedHeight(40);
    if(common->is_server==1)
    {
        ui->list_scrollarea->setStyleSheet("background-color: rgb(3, 9, 12)");
        ui->scrollAreaWidgetContents->setStyleSheet("background-color: rgb(3, 9, 12)");
    }
    ui->releasebtn->setEnabled(false);
    selected_patient = -1;
    connect(&worker_timer, SIGNAL(timeout()), this, SLOT(worker()));
    worker_timer.start(16);
    Common::set_inactive_colors(ui->mappingPatient_widget);
    Common::set_inactive_colors(ui->medicalDevices_widget);
    Common::set_inactive_colors(ui->widget_2);
    Common::set_inactive_colors(ui->horizontalWidget);
    Common::set_inactive_colors(ui->list_highlight);
}

Tab_Mapping_Widget::~Tab_Mapping_Widget()
{
    delete ui;
}

void Tab_Mapping_Widget::worker()
{
    if(velocity == 0)
        return;
    ui->list_scrollarea->verticalScrollBar()->setValue(ui->list_scrollarea->verticalScrollBar()->value()-
                                                       velocity);
    if(velocity > 0.0)
    {
        velocity -= 0.1;
        if(velocity < 0.0)
            velocity = 0.0;
    }
    else
    {
        velocity += 0.1;
        if(velocity > 0.0)
            velocity = 0.0;
    }
}

void Tab_Mapping_Widget::select_patient(int i)
{
    printf("select patient i=%d\n", i);
    if(selected_patient != -1)
        return;
    Common* common = Common::instance();
    /*
    if(common->md->dm.devices.size() == 0)
    {
        printf("no devices\n");
        return;
    }*/
    selected_patient = i;
    on_itemSelectionChanged();
}

void Tab_Mapping_Widget::on_itemSelectionChanged()
{
    printf("item selection changed\n");
    Common* common = Common::instance();
    ui->releasebtn->setEnabled(true);
    for(int k=0;k<(int)cards.size();k++)
    {
        if(k==selected_patient)
            cards[k]->set_selected(1);
        else
            cards[k]->set_selected(0);
    }
    int currentRow = selected_patient;
    int i=0;
    auto it=common->md->pm.patients.begin();
    for(;it!=common->md->pm.patients.end();it++)
    {
        if(i==currentRow)
            break;
        i++;
    }
    if(it->second.gender.compare("female") == 0)
        ui->pic_label->setPixmap(QPixmap(":/icons/female.png"));
    else
        ui->pic_label->setPixmap(QPixmap(":/icons/male.png"));
    ui->bid_label->setText(it->second.bed.c_str());
    ui->mrn_label->setText(it->second.mrn.c_str());
    ui->familyName_label->setText(it->second.family_name.c_str());
    ui->birthday_label->setText(it->second.bod.c_str());
    if(it->second.gender.compare("male") == 0)
        ui->sex_label->setText("男");
    else if(it->second.gender.compare("female") == 0)
        ui->sex_label->setText("女");
    else if(it->second.gender.compare("other") == 0)
        ui->sex_label->setText("其他");
    else
        ui->sex_label->setText("未知");
    common->md->ui->mRNLineEdit->setText(it->second.mrn.c_str());
    common->md->ui->bIDLineEdit->setText(it->second.bed.c_str());
    common->md->ui->familyNameLineEdit->setText(it->second.family_name.c_str());
    common->patient_id = it->second.id;
    mapping_UI_reset();

    emit bindingChanged();
    common->md->ui->nav->set_enabled(1);
    common->bed_id = it->second.bed;
    common->room_id = it->second.room;
    dds::core::xtypes::DynamicData sample(common->mds_type);
    sample.value<std::string>("patient_id", common->patient_id);
    sample.value<std::string>("vmd_id", common->vmd_id);
    sample.value<std::string>("room_id", it->second.room);
    sample.value<std::string>("bed_id", it->second.bed);
    common->mds_writer.write(sample);

    std::string dummy;
    std::string sql = "SELECT meta(_).id FROM _ WHERE data_source='MDSConnectivity' AND vmd_id='";
    sql.append(common->vmd_id);
    sql.append("'");
    cbl::ResultSet results = common->cbl->queryDocuments(common->db, sql, dummy);
    for(auto& result: results)
    {
        std::string id = result.valueAtIndex(0).asstring();
        cbl::Document d = common->db.getDocument(id);
        common->db.deleteDocument(d);
    }

    rapidjson::Document d;
    d.SetObject();
    d.AddMember("data_source", "MDSConnectivity", d.GetAllocator());
    d.AddMember("vmd_id", rapidjson::Value().SetString(common->vmd_id.c_str(), d.GetAllocator()), d.GetAllocator());
    d.AddMember("patient_id", rapidjson::Value().SetString(common->patient_id.c_str(), d.GetAllocator()), d.GetAllocator());
    d.AddMember("room_id", rapidjson::Value().SetString(it->second.room.c_str(), d.GetAllocator()), d.GetAllocator());
    d.AddMember("bed_id", rapidjson::Value().SetString(it->second.bed.c_str(), d.GetAllocator()), d.GetAllocator());
    d.AddMember("source_timestamp", time(NULL), d.GetAllocator());
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    std::string dummy2;
    common->cbl->saveMutableDocument(common->db, buffer.GetString(), dummy2, dummy);
    common->observation_widget_page->changeToPrevPage();
    if(common->md->pm.omit_observation_redirect)
    {
        common->md->pm.omit_observation_redirect = 0;
        return;
    }
    common->md->ui->nav->set_current_tab(3);


}

void Tab_Mapping_Widget::perform_release(std::string employeeID)
{
    Common* common = Common::instance();
    hide_windows();

/*
    dds::core::xtypes::DynamicData sample(common->mds_type);
    sample.value<std::string>("patient_id", "");
    sample.value<std::string>("vmd_id", common->vmd_id);
    dds::core::InstanceHandle instance_handle = common->mds_writer->lookup_instance(sample);
    if(!instance_handle.is_nil())
        common->mds_writer->dispose_instance(instance_handle);*/

    /*
    auto it = common->md->pm.patients.begin();
    for(;it!=common->md->pm.patients.end();it++)
    {
        if(it->second.id.compare(common->patient_id) == 0)
            break;
    }
    if(it==common->md->pm.patients.end())
        return;*/

    if(employeeID.size() > 0)
    {
        dds::core::xtypes::DynamicData sample(common->mds_type);
        sample.value<std::string>("patient_id", "");
        sample.value<std::string>("vmd_id", common->vmd_id);
        sample.value<std::string>("room_id", common->room_id);
        sample.value<std::string>("bed_id", common->bed_id);
        common->mds_writer.write(sample);
        auto bit = common->md->pm.bound_patients.find(common->patient_id);
        if(bit != common->md->pm.bound_patients.end())
            common->md->pm.bound_patients.erase(bit);
    }

    std::string dummy;
    std::string sql = "SELECT meta(_).id FROM _ WHERE data_source='MDSConnectivity' AND vmd_id='";
    sql.append(common->vmd_id);
    sql.append("'");
    cbl::ResultSet results = common->cbl->queryDocuments(common->db, sql, dummy);
    for(auto& result: results)
    {
        std::string id = result.valueAtIndex(0).asstring();
        cbl::Document d = common->db.getDocument(id);
        common->db.deleteDocument(d);
    }
    if(employeeID.size() > 0)
    {
        common->commit_user_action(employeeID.c_str(), "Mapping", "Release");
    }
    common->md->ui->mRNLineEdit->setText("");
    common->md->ui->bIDLineEdit->setText("");
    common->md->ui->familyNameLineEdit->setText("");
    common->patient_id.clear();
    common->bed_id.clear();
    common->exporter_page->handle_stop();
    //common->item_checkstate.clear();
    //common->device_checkstate.clear();
    selected_patient = -1;
    if(common->is_server)
        common->monitor_page->selected_patient = -1;
    for(int i=0;i<(int)cards.size();i++)
        cards[i]->set_selected(0);
    ui->pic_label->setPixmap(QPixmap(""));
    ui->bid_label->setText("");
    ui->mrn_label->setText("");
    ui->familyName_label->setText("");
    ui->sex_label->setText("");
    ui->birthday_label->setText("");
    ui->releasebtn->setEnabled(false);
    common->md->ui->nav->update();
    emit bindingChanged();
    if(common->is_server==1)
        {
        common->md->ui->main_stackedWidget->setCurrentIndex(0);
        common->vmd_id.clear();
        }
}

void Tab_Mapping_Widget::force_release()
{
    Common* common = Common::instance();
    hide_windows();

    *mc_mapping_active = 0;
    frd.ui->label->setText("Patient has moved. Forcing release.");
    frd.clearText();
    frd.exec();
    std::string employeeID = frd.getText();
    *mc_mapping_active = 1;
    perform_release(employeeID);

}

void Tab_Mapping_Widget::on_releasebtn_clicked()
{
    Common* common = Common::instance();
    *mc_mapping_active = 0;
    common->rd.ok = 0;
    common->rd.clearText();
    common->rd.exec();
    std::string employeeID = common->rd.getText();
    *mc_mapping_active = 1;
    if(common->rd.ok && employeeID.size() > 0)
        perform_release(employeeID);

}

void Tab_Mapping_Widget::showEvent(QShowEvent *event)
{
    if(mc_mapping_active)
        *mc_mapping_active = 1;
    Common* common = Common::instance();
}

void Tab_Mapping_Widget::hideEvent(QHideEvent *event)
{
    if(mc_mapping_active)
        *mc_mapping_active = 0;
}

void Tab_Mapping_Widget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->globalPosition().x() > ui->list_scrollarea->mapToGlobal(ui->list_scrollarea->pos()).x() &&
       event->globalPosition().x() < ui->list_scrollarea->mapToGlobal(ui->list_scrollarea->pos()).x()+ui->list_scrollarea->width() &&
       event->globalPosition().y() > ui->list_scrollarea->mapToGlobal(ui->list_scrollarea->pos()).y() &&
       event->globalPosition().y() < ui->list_scrollarea->mapToGlobal(ui->list_scrollarea->pos()).y()+ui->list_scrollarea->height())
    {
        ui->list_scrollarea->verticalScrollBar()->setValue(ui->list_scrollarea->verticalScrollBar()->value()-
                                                           (event->globalPosition().y()-last_pos.y()));
        if(Common::distance(press_pos.x(), press_pos.y(), event->globalPosition().x(), event->globalPosition().y()) > STATIONARY_CHECK_DISTANCE)
            press_is_stationary = 0;
        last_velocity = event->globalPosition().y()-last_pos.y();
        last_pos = event->globalPosition();
    }
}

void Tab_Mapping_Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->globalPosition().x() > ui->list_scrollarea->mapToGlobal(ui->list_scrollarea->pos()).x() &&
       event->globalPosition().x() < ui->list_scrollarea->mapToGlobal(ui->list_scrollarea->pos()).x()+ui->list_scrollarea->width() &&
       event->globalPosition().y() > ui->list_scrollarea->mapToGlobal(ui->list_scrollarea->pos()).y() &&
       event->globalPosition().y() < ui->list_scrollarea->mapToGlobal(ui->list_scrollarea->pos()).y()+ui->list_scrollarea->height())
    {
        last_pos = event->globalPosition();
        press_pos = last_pos;
        press_is_stationary = 1;
    }
}

void Tab_Mapping_Widget::mouseReleaseEvent(QMouseEvent *event)
{
    if(!press_is_stationary)
    {
        velocity = last_velocity*VELOCITY_MULTIPLIER;
    }
}

void Tab_Mapping_Widget::on_returnbtn_clicked()
{
    force_return();
}

void Tab_Mapping_Widget::force_return()
{
    Common* common = Common::instance();
    hide_windows();

    common->md->ui->main_stackedWidget->setCurrentIndex(0);
    common->monitor_page->selected_patient=-1;
    common->md->ui->mRNLineEdit->setText("");
    common->md->ui->bIDLineEdit->setText("");
    common->md->ui->familyNameLineEdit->setText("");
    common->patient_id.clear();
    common->bed_id.clear();
    common->vmd_id.clear();
    common->exporter_page->handle_stop();
    selected_patient = -1;
    common->monitor_page->selected_patient = -1;
    for(int i=0;i<(int)cards.size();i++)
        cards[i]->set_selected(0);
    ui->pic_label->setPixmap(QPixmap(""));
    ui->bid_label->setText("");
    ui->mrn_label->setText("");
    ui->familyName_label->setText("");
    ui->sex_label->setText("");
    ui->birthday_label->setText("");
    ui->releasebtn->setEnabled(false);
    common->md->ui->nav->update();
    emit bindingChanged();
}
void Tab_Mapping_Widget::mapping_UI_reset()
{
Common* common = Common::instance();

common->observation_main_page->clear_points();
common->alarm_page->ui->queryResult_tableWidget->clearContents();
common->alarm_page_2->ui->queryResult_tableWidget->clearContents();
common->alarm_page_3->ui->queryResult_tableWidget->clearContents();
common->alarm_page->ui->queryResult_tableWidget->setRowCount(0);
common->alarm_page_2->ui->queryResult_tableWidget->setRowCount(0);
common->alarm_page_3->ui->queryResult_tableWidget->setRowCount(0);

common->observation_main_page->ui->device1_tableWidget->clearContents();
common->observation_main_page->ui->device2_tableWidget->clearContents();
common->observation_main_page->ui->device3_tableWidget->clearContents();
common->observation_main_page->ui->device4_tableWidget->clearContents();
common->exporter_page->ui->log_textBrowser->setText("");
common->exporter_page->ui->fileName_label->setText("");
common->exporter_page->ui->send_pushButton->setEnabled(false);
common->exporter_page->ui->send_pushButton->setStyleSheet(common->css.Enabled_ButtonStyle);
common->history_page->clear_selection();
common->history_page->reset_time_jumper();
common->devices_page->ui->history_table->clearContents();
common->devices_page->ui->history_table->setRowCount(0);
}
void Tab_Mapping_Widget::hide_windows()
{
    Common* common = Common::instance();
if(!common->exporter_page->jt.isHidden())
    common->exporter_page->jt.hide();
if(!common->rd.isHidden())
    common->rd.hide();
if(!common->observation_main_page->lsd.isHidden())
    common->observation_main_page->lsd.hide();

}
