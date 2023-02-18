#include "Tab_Devices_Widget.h"
#include "ui_Tab_Devices_Widget.h"
#include <QButtonGroup>
#include "Common.h"

class TableItem
{
public:
    std::string item;
    std::string value;
    std::string memo;
};

class HistoryItem
{
public:
    std::string date;
    std::string timeval;
    std::string item;
    std::string orig_val;
    std::string cur_val;
    std::string code;
};

Tab_Devices_Widget::Tab_Devices_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab_Devices_Widget)
{
    Common* common = Common::instance();
    common->devices_page = this;
    ui->setupUi(this);
    ui->deviceTemplate_pushButton->hide();
    active = 0;
    dev_index = 0;

    ui->current_settings_table->setShowGrid(0);
    ui->current_settings_table_2->setShowGrid(0);
    ui->history_table->setShowGrid(0);
    ui->history_table_2->setShowGrid(0);
    ui->current_settings_table->setSelectionMode(QAbstractItemView::NoSelection);
    ui->current_settings_table_2->setSelectionMode(QAbstractItemView::NoSelection);
    ui->history_table->setSelectionMode(QAbstractItemView::NoSelection);
    ui->history_table_2->setSelectionMode(QAbstractItemView::NoSelection);
    ui->current_settings_table->setColumnWidth(0, 400);
    ui->current_settings_table->setColumnWidth(1, 230);
    ui->current_settings_table->setColumnWidth(2, 60);
    ui->current_settings_table_2->setColumnWidth(0, 400);
    ui->current_settings_table_2->setColumnWidth(1, 230);
    ui->current_settings_table_2->setColumnWidth(2, 30);
    ui->history_table->setColumnWidth(0, 135);
    ui->history_table->setColumnWidth(1, 110);
    ui->history_table->setColumnWidth(2, 380);
    ui->history_table_2->setColumnWidth(0, 135);
    ui->history_table_2->setColumnWidth(1, 110);
    ui->history_table_2->setColumnWidth(2, 380);
    connect(ui->current_settings_table,SIGNAL(cellClicked),this,SLOT(table_return()));
}
void Tab_Devices_Widget::table_return()
{

}

void Tab_Devices_Widget::update_devices(std::map<std::string, Device>* btns)
{
    Common* common = Common::instance();
    for(int i=0;i<(int)devices.size();i++)
    {
        ui->devices_horizontalLayout->removeWidget(devices[i]);
        delete devices[i];
    }
    devices.clear();
    Device* dev = NULL;
    QButtonGroup *qButtonGroup = new QButtonGroup(this);
    qButtonGroup->setExclusive(true);
    int i=0;
    for(auto it=btns->begin();it!=btns->end();it++)
    {
        std::string label = it->second.model;
        QPushButton* btn = new QPushButton(label.c_str(), this);
        btn->setSizePolicy(ui->deviceTemplate_pushButton->sizePolicy());
        btn->setMinimumSize(ui->deviceTemplate_pushButton->minimumSize());
        btn->setMaximumSize(ui->deviceTemplate_pushButton->maximumSize());
        btn->setCheckable(true);

        btn->setAutoExclusive(true);

        btn->setStyleSheet(common->css.Device_ButtonStyle);

        qButtonGroup->addButton(btn);
        //philo
        btn->setProperty("index", i);
        connect(btn, SIGNAL(pressed()), this, SLOT(btn_clicked()));
        ui->devices_horizontalLayout->addWidget(btn);
        devices.push_back(btn);
        if(i == dev_index)
        {
            btn->setDown(true);
            btn->setStyleSheet(common->css.Select_Device_ButtonStyle);
            dev = &it->second;
        }
        i++;
    }
    if(active && dev)
    {
        Common* common = Common::instance();
        ui->history_table->clearContents();
        std::string querystr;
        int row;
        uint8_t lpo_enabled = 0;
        if(common->patient_id.size() > 0)
        {
            querystr = "vmd_id MATCH '";
            querystr.append(common->vmd_id);
            querystr.append("' AND channel_id MATCH '");
            querystr.append(dev->channel_id);
            querystr.append("' AND patient_id MATCH '");
            querystr.append(common->patient_id);
            querystr.append("'");
            dds::sub::cond::QueryCondition cond2(
                        dds::sub::Query(common->chansettingshis_reader, querystr),
                        dds::sub::status::DataState(
                        dds::sub::status::SampleState::any(),
                        dds::sub::status::ViewState::any(),
                        dds::sub::status::InstanceState::alive()));
            dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples2 = common->chansettingshis_reader.select().condition(cond2).read();
            std::multimap<time_t, HistoryItem> his;
   //         printf("chanhis sample size=%d\n", (int)samples2.length());
            int num_filtered = 0;
            for(auto& sample : samples2)
            {
                HistoryItem i;
                dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
                rti::core::xtypes::LoanedDynamicData loaned_member = data.loan_value("cur_source_timestamp");
                time_t sec = (time_t)loaned_member.get().value<int32_t>("sec");
                loaned_member.return_loan();
                loaned_member = data.loan_value("org_source_timestamp");
                time_t org_sec = (time_t)loaned_member.get().value<int32_t>("sec");
                loaned_member.return_loan();
                if(sec == org_sec)
                {
                    num_filtered++;
                    continue;
                }
                struct tm t;
                localtime_r(&sec, &t);
                char buf[256];
                sprintf(buf, "%d-%d-%d", t.tm_year+1900, t.tm_mon+1, t.tm_mday);
                i.date = buf;
                sprintf(buf, "%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
                i.timeval = buf;
                i.item = sample.data().value<std::string>("description");
                i.orig_val = sample.data().value<std::string>("org_value_unit");
                i.cur_val = sample.data().value<std::string>("cur_value_unit");
                if(i.orig_val.compare("null") == 0)
                    i.orig_val.clear();
                if(i.cur_val.compare("null") == 0)
                    i.cur_val.clear();
                i.code = sample.data().value<std::string>("code");
                his.emplace(sec, i);
            }
            auto iit = his.rbegin();
            for(;iit!=his.rend();iit++)
            {
                if(iit->second.orig_val.size() == 0 && iit->second.cur_val.size() == 0)
                    break;
            }
            if(iit!=his.rend() && iit->second.code.compare("55")==0)
                lpo_enabled = 1;
 //           printf("num filtered = %d\n", num_filtered);
            ui->history_table->setRowCount(his.size()+1);
            row = 0;
            for(auto it=his.rbegin();it!=his.rend();it++)
            {
                int col = 0;
                QTableWidgetItem* item0 = new QTableWidgetItem(it->second.date.c_str());
                item0->setFlags(Qt::ItemIsEnabled);
                ui->history_table->setItem(row, col++, item0);

                QTableWidgetItem* item1 = new QTableWidgetItem(it->second.timeval.c_str());
                item1->setFlags(Qt::ItemIsEnabled);
                ui->history_table->setItem(row, col++, item1);

                QTableWidgetItem* item2 = new QTableWidgetItem(it->second.item.c_str());
                item2->setFlags(Qt::ItemIsEnabled);
                ui->history_table->setItem(row, col++, item2);

                QTableWidgetItem* item3 = new QTableWidgetItem(it->second.orig_val.c_str());
                item3->setFlags(Qt::ItemIsEnabled);
                ui->history_table->setItem(row, col++, item3);

                QTableWidgetItem* item4 = new QTableWidgetItem(it->second.cur_val.c_str());
                item4->setFlags(Qt::ItemIsEnabled);
                ui->history_table->setItem(row, col++, item4);
                if(row % 2 ==1)
                    {
                    item0->setBackground(QBrush(QColor(11, 42, 78)));
                    item1->setBackground(QBrush(QColor(11, 42, 78)));
                    item2->setBackground(QBrush(QColor(11, 42, 78)));
                    item3->setBackground(QBrush(QColor(11, 42, 78)));
                    item4->setBackground(QBrush(QColor(11, 42, 78)));
                }
                row++;
            }
        }

        querystr = "vmd_id MATCH '";
        querystr.append(common->vmd_id);
        querystr.append("' AND channel_id MATCH '");
        querystr.append(dev->channel_id);
        querystr.append("'");
        dds::sub::cond::QueryCondition cond(
                    dds::sub::Query(common->chansettings_reader, querystr),
                    dds::sub::status::DataState(
                    dds::sub::status::SampleState::any(),
                    dds::sub::status::ViewState::any(),
                    dds::sub::status::InstanceState::alive()));
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples = common->chansettings_reader.select().condition(cond).read();
        ui->current_settings_table->clearContents();
        if(samples.length()>14)
            ui->current_settings_table->setRowCount(samples.length());
        else
            ui->current_settings_table->setRowCount(samples.length()+1);
        std::map<std::string, TableItem> device_section;
        std::multimap<std::string, TableItem> hl_section;
        std::map<std::string, TableItem> mode_section;
        for(auto& sample : samples)
        {
            std::string limit_type = sample.data().value<std::string>("limit_type");
            std::string code = sample.data().value<std::string>("code");
            if(((code.compare("F0")==0 && (limit_type.compare("HIGH")==0 || limit_type.compare("LOW")==0)) || code.compare("C7")==0) && !lpo_enabled)
                continue;
            TableItem i;
            i.item = sample.data().value<std::string>("description");
            char buf[128];
            if(limit_type.compare("MODE") == 0)
                sprintf(buf, "");
            else
            {
                common->utils->mdcValueFormatter("DeviceSettings", code, sample.data().value<float>("value"), sample.data().value<std::string>("unit"), buf);
            }
            i.value = buf;
            if(limit_type.compare("DEVICE") == 0)
                sprintf(buf, "");
            else if(limit_type.compare("HIGH") == 0)
                sprintf(buf, "HL");
            else if(limit_type.compare("LOW") == 0)
                sprintf(buf, "LL");
            else
                sprintf(buf, "");
            i.memo = buf;
            if(limit_type.compare("DEVICE") == 0)
                device_section.emplace(code, i);
            else if(limit_type.compare("HIGH") == 0)
                hl_section.emplace(code+"h", i);
            else if(limit_type.compare("LOW") == 0)
                hl_section.emplace(code+"l", i);
            else
                mode_section.emplace(code, i);
        }
        row = 0;
        for(auto it=device_section.begin();it!=device_section.end();it++)
        {
            int col = 0;
            QTableWidgetItem* item1 = new QTableWidgetItem(it->second.item.c_str());
            item1->setFlags(Qt::ItemIsEnabled);
            ui->current_settings_table->setItem(row, col++, item1);
            QTableWidgetItem* item2 = new QTableWidgetItem(it->second.value.c_str());
            item2->setFlags(Qt::ItemIsEnabled);
            ui->current_settings_table->setItem(row, col++, item2);
            QTableWidgetItem* item3 = new QTableWidgetItem(it->second.memo.c_str());
            item3->setFlags(Qt::ItemIsEnabled);
            ui->current_settings_table->setItem(row, col++, item3);
            if(row % 2 ==1)
                {
                item1->setBackground(QBrush(QColor(11, 42, 78)));
                item2->setBackground(QBrush(QColor(11, 42, 78)));
                item3->setBackground(QBrush(QColor(11, 42, 78)));
            }
            row++;
        }
        for(auto it=hl_section.begin();it!=hl_section.end();it++)
        {
            int col = 0;
            QTableWidgetItem* item1 = new QTableWidgetItem(it->second.item.c_str());
            item1->setFlags(Qt::ItemIsEnabled);
            ui->current_settings_table->setItem(row, col++, item1);
            QTableWidgetItem* item2 = new QTableWidgetItem(it->second.value.c_str());
            item2->setFlags(Qt::ItemIsEnabled);
            ui->current_settings_table->setItem(row, col++, item2);
            QTableWidgetItem* item3 = new QTableWidgetItem(it->second.memo.c_str());
            item3->setFlags(Qt::ItemIsEnabled);
            ui->current_settings_table->setItem(row, col++, item3);
            if(row % 2 ==1)
                {
                item1->setBackground(QBrush(QColor(11, 42, 78)));
                item2->setBackground(QBrush(QColor(11, 42, 78)));
                item3->setBackground(QBrush(QColor(11, 42, 78)));
            }
            row++;
        }
        for(auto it=mode_section.begin();it!=mode_section.end();it++)
        {
            int col = 0;
            QTableWidgetItem* item1 = new QTableWidgetItem(it->second.item.c_str());
            item1->setFlags(Qt::ItemIsEnabled);
            ui->current_settings_table->setItem(row, col++, item1);
            QTableWidgetItem* item2 = new QTableWidgetItem(it->second.value.c_str());
            item2->setFlags(Qt::ItemIsEnabled);
            ui->current_settings_table->setItem(row, col++, item2);
            QTableWidgetItem* item3 = new QTableWidgetItem(it->second.memo.c_str());
            item3->setFlags(Qt::ItemIsEnabled);
            ui->current_settings_table->setItem(row, col++, item3);
            if(row % 2 ==1)
                {
                item1->setBackground(QBrush(QColor(11, 42, 78)));
                item2->setBackground(QBrush(QColor(11, 42, 78)));
                item3->setBackground(QBrush(QColor(11, 42, 78)));
            }
            row++;
        }

        ui->history_table->clearContents();
        if(common->patient_id.size() > 0)
        {
            querystr = "vmd_id MATCH '";
            querystr.append(common->vmd_id);
            querystr.append("' AND channel_id MATCH '");
            querystr.append(dev->channel_id);
            querystr.append("' AND patient_id MATCH '");
            querystr.append(common->patient_id);
            querystr.append("'");
            dds::sub::cond::QueryCondition cond2(
                        dds::sub::Query(common->chansettingshis_reader, querystr),
                        dds::sub::status::DataState(
                        dds::sub::status::SampleState::any(),
                        dds::sub::status::ViewState::any(),
                        dds::sub::status::InstanceState::alive()));
            dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples2 = common->chansettingshis_reader.select().condition(cond2).read();
            std::multimap<time_t, HistoryItem> his;
     //       printf("chanhis sample size=%d\n", (int)samples2.length());
            int num_filtered = 0;
            for(auto& sample : samples2)
            {
                HistoryItem i;
                dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
                rti::core::xtypes::LoanedDynamicData loaned_member = data.loan_value("cur_source_timestamp");
                time_t sec = (time_t)loaned_member.get().value<int32_t>("sec");
                loaned_member.return_loan();
                loaned_member = data.loan_value("org_source_timestamp");
                time_t org_sec = (time_t)loaned_member.get().value<int32_t>("sec");
                loaned_member.return_loan();
                if(sec == org_sec)
                {
                    num_filtered++;
                    continue;
                }
                struct tm t;
                localtime_r(&sec, &t);
                char buf[256];
                sprintf(buf, "%d-%d-%d", t.tm_year+1900, t.tm_mon+1, t.tm_mday);
                i.date = buf;
                sprintf(buf, "%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
                i.timeval = buf;
                i.item = sample.data().value<std::string>("description");
                i.orig_val = sample.data().value<std::string>("org_value_unit");
                i.cur_val = sample.data().value<std::string>("cur_value_unit");
                if(i.orig_val.compare("null") == 0)
                    i.orig_val.clear();
                if(i.cur_val.compare("null") == 0)
                    i.cur_val.clear();
                his.emplace(sec, i);
            }
 //           printf("num filtered = %d\n", num_filtered);
            if(his.size()>14)
                ui->history_table->setRowCount(his.size());
            else
                ui->history_table->setRowCount(his.size()+1);
            row = 0;
            for(auto it=his.rbegin();it!=his.rend();it++)
            {
                int col = 0;
                QTableWidgetItem* item0 = new QTableWidgetItem(it->second.date.c_str());
                item0->setFlags(Qt::ItemIsEnabled);
                ui->history_table->setItem(row, col++, item0);

                QTableWidgetItem* item1 = new QTableWidgetItem(it->second.timeval.c_str());
                item1->setFlags(Qt::ItemIsEnabled);
                ui->history_table->setItem(row, col++, item1);

                QTableWidgetItem* item2 = new QTableWidgetItem(it->second.item.c_str());
                item2->setFlags(Qt::ItemIsEnabled);
                ui->history_table->setItem(row, col++, item2);

                QTableWidgetItem* item3 = new QTableWidgetItem(it->second.orig_val.c_str());
                item3->setFlags(Qt::ItemIsEnabled);
                ui->history_table->setItem(row, col++, item3);

                QTableWidgetItem* item4 = new QTableWidgetItem(it->second.cur_val.c_str());
                item4->setFlags(Qt::ItemIsEnabled);
                ui->history_table->setItem(row, col++, item4);
                if(row % 2 ==1)
                    {
                    item0->setBackground(QBrush(QColor(11, 42, 78)));
                    item1->setBackground(QBrush(QColor(11, 42, 78)));
                    item2->setBackground(QBrush(QColor(11, 42, 78)));
                    item3->setBackground(QBrush(QColor(11, 42, 78)));
                    item4->setBackground(QBrush(QColor(11, 42, 78)));
                }
                row++;
            }
        }
    }
}

void Tab_Devices_Widget::btn_clicked()
{
    printf("agooda btn_clicked\n");
    QPushButton* s = (QPushButton*)sender();
    dev_index = s->property("index").value<int>();
    for(int i=0;i<(int)devices.size();i++)
        devices[i]->setDown(false);
    s->setDown(true);
}

void Tab_Devices_Widget::showEvent(QShowEvent *event)
{
    active = 1;
    if(devices.size() > 0)
        devices[0]->setDown(true);
    dev_index = 0;
}

void Tab_Devices_Widget::hideEvent(QHideEvent *event)
{
    active = 0;
}


Tab_Devices_Widget::~Tab_Devices_Widget()
{
    delete ui;
}
