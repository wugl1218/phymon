#include "Tab_Observations_metricItemsDisplayConfigPage_Widget.h"
#include "mc_selection_entry.h"
#include "ui_Tab_Observations_metricItemsDisplayConfigPage_Widget.h"
#include "Common.h"
#include "MainDialog.h"

#define MC_ENTRY_HEIGHT 64
#define MC_SCROLLBAR_WIDTH 15
#define MC_ENTRY_WIDTH 805
#define MC_ENTRY_MAX 21

Tab_Observations_metricItemsDisplayConfigPage_Widget::Tab_Observations_metricItemsDisplayConfigPage_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab_Observations_metricItemsDisplayConfigPage_Widget)
{
    ui->setupUi(this);
    has_init = 0;
    Common* common = Common::instance();
    common->device_settings_page = this;
    int filesize;
    char* cfg = (char*)Common::alloc_from_file("../config/device_settings.json", &filesize);
    if(cfg == NULL)
    {
        printf("Unable to read device_settings.json.\n");
        exit(1);
    }
    rapidjson::Document d;
    d.Parse(cfg);
    free(cfg);
    for(auto it=d.MemberBegin();it!=d.MemberEnd();it++)
    {
        std::vector<std::string> e;
        for(auto it2=it->value.GetArray().Begin();it2!=it->value.GetArray().End();it2++)
            e.push_back(it2->GetString());
        supported_settings.emplace(it->name.GetString(), e);
    }
    ui->deviceMetric_tabWidget->setTabVisible(0, false);
    ui->deviceMetric_tabWidget->setTabVisible(1, false);
    ui->deviceMetric_tabWidget->setTabVisible(2, false);
    ui->deviceMetric_tabWidget->setTabVisible(3, false);

    checkboxes[0] = ui->checkBox_0;
    checkboxes[1] = ui->checkBox_1;
    checkboxes[2] = ui->checkBox_2;
    checkboxes[3] = ui->checkBox_3;
    num_devices = 0;

    leftscrollarea[0] = ui->tab0_leftscrollarea;
    rightscrollarea[0] = ui->tab0_rightscrollarea;
    leftscrollarea[1] = ui->tab1_leftscrollarea;
    rightscrollarea[1] = ui->tab1_rightscrollarea;
    leftscrollarea[2] = ui->tab2_leftscrollarea;
    rightscrollarea[2] = ui->tab2_rightscrollarea;
    leftscrollarea[3] = ui->tab3_leftscrollarea;
    rightscrollarea[3] = ui->tab3_rightscrollarea;
    //
    //connect(ui->devices_tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(toggleEnableItemStatus(int,int)));
    //connect(ui->devices_tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(toggleDeviceTabEnable(int,int)));

    ui->tab0_selectall->hide();
    ui->tab1_selectall->hide();
    ui->tab2_selectall->hide();
    ui->tab3_selectall->hide();

    connect(ui->restoreAllbtn, SIGNAL(clicked()), this, SLOT(on_restoreAll_pushButton_clicked()));
    connect(ui->restorebtn, SIGNAL(clicked()), this, SLOT(on_restore_pushButton_clicked()));
    connect(ui->savebtn, SIGNAL(clicked()), this, SLOT(on_save_pushButton_clicked()));

}

Tab_Observations_metricItemsDisplayConfigPage_Widget::~Tab_Observations_metricItemsDisplayConfigPage_Widget()
{
    delete ui;
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::update_devices()
{
    Common* common = Common::instance();
    if(common->patient_id.size() == 0)
        return;
    std::map<std::string, uint8_t> current_checked;
    if(has_init)
    {
        for(int r=0;r<num_devices;r++)
        {
            uint8_t checked = 0;
            if(checkboxes[r]->checkState() == Qt::Checked)
                checked = 1;
            current_checked.emplace(checkboxes[r]->text().toStdString(), checked);
        }
    }
    else
    {
        has_init = 1;
        std::string dummy;
        std::string sql = "SELECT model,checked FROM _ WHERE data_source='NumericDeviceSelection' AND patient_id='";
        sql.append(common->patient_id);
        sql.append("' AND meta(_).expiration IS NOT VALUED and expired=0");
        cbl::ResultSet results= common->cbl->queryDocuments(common->display_items_db, sql, dummy);
        int error=0;while (dummy!="IP200"&&error<5)
            {
            results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
            qDebug()<<QString::fromStdString(dummy);
            fflog_out(common->log,dummy.c_str());error++;
            }
        for(auto& result: results)
        {
            std::string model = result.valueAtIndex(0).asstring();
            uint8_t checked = result.valueAtIndex(1).asUnsigned();
            current_checked.emplace(model, checked);
        }
    }
    common->device_checkstate.clear();
    for(int i=0;i<4;i++)
        checkboxes[i]->hide();
    num_devices = common->md->dm.devices.size();
    nosave_checked =0;
    for(int i=0;i<num_devices;i++)
    {
        if(checkboxes[i]->checkState() == Qt::Checked)
            nosave_checked = 1;
        checkboxes[i]->show();
    }
    if (!nosave_checked)
    {
        ui->restoreAllbtn->setEnabled(false);
        ui->restorebtn->setEnabled(false);
    }
    else
    {
        ui->restoreAllbtn->setEnabled(true);
        ui->restorebtn->setEnabled(true);
    }
    uint8_t device_changed = 0;
    int row = 0;
    for(auto it=common->md->dm.devices.begin();it!=common->md->dm.devices.end();it++)
    {
        auto it2 = current_checked.find(it->second.model);
        uint8_t checked;
        if(it2 != current_checked.end())
        {
            if(it2->second)
            {
                checkboxes[row]->setCheckState(Qt::Checked);
                checked = 1;
            }
            else
            {
                checkboxes[row]->setCheckState(Qt::Unchecked);
                checked = 0;
            }
        }
        else
        {
            checkboxes[row]->setCheckState(Qt::Checked);
            checked = 1;
        }
        common->device_checkstate.emplace(it->second.model, checked);

        checkboxes[row]->setText(it->second.model.c_str());
        ui->deviceMetric_tabWidget->setTabVisible(row, true);
        if(checkboxes[row]->checkState() == Qt::Checked)
            ui->deviceMetric_tabWidget->setTabEnabled(row, true);
        else
            ui->deviceMetric_tabWidget->setTabEnabled(row, false);

        if(it->second.model.compare(tab_names[row]) != 0)
        {
            device_changed = 1;
            std::map<std::string, mc_checkstate> item_visibility;
            std::string dummy;
            std::string sql = "SELECT mdc_code,visibility,display_index FROM _ WHERE data_source='Obs' AND model='";
            sql.append(it->second.model);
            sql.append("' AND patient_id='");
            sql.append(common->patient_id);
            sql.append("' AND meta(_).expiration IS NOT VALUED AND expired=0");
            cbl::ResultSet results2= common->cbl->queryDocuments(common->display_items_db, sql, dummy);
            int error=0;while (dummy!="IP200"&&error<5)
                {
                results2 = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
                qDebug()<<QString::fromStdString(dummy);
                fflog_out(common->log,dummy.c_str());error++;
                }
            for(auto& result: results2)
            {
                std::string item = result.valueAtIndex(0).asstring();
                mc_checkstate cs;
                cs.checked = result.valueAtIndex(1).asUnsigned();
                cs.order = result.valueAtIndex(2).asInt();
                item_visibility.emplace(item, cs);
            }

            std::string str3 =it->second.model;
            ui->deviceMetric_tabWidget->setTabText(row,str3.c_str());
            tab_names[row] = it->second.model;
            for(int i=0;i<(int)left_lines[row].size();i++)
                delete left_lines[row][i];
            left_lines[row].clear();
            for(int i=0;i<(int)right_lines[row].size();i++)
                delete right_lines[row][i];
            right_lines[row].clear();

            auto it2 = supported_settings.find(it->second.model);
            if(it2 != supported_settings.end())
            {
                std::multimap<int, std::string> right_ordering;
                std::multimap<int, std::string> left_ordering;
                QWidget* left_scrollarea = leftscrollarea[row];
                QWidget* right_scrollarea = rightscrollarea[row];
                for(int i=0;i<(int)it2->second.size();i++)
                {
                    auto fit = item_visibility.find(it2->second[i]);
                    if(fit == item_visibility.end())
                    {
                        auto sit = common->special_items.find(it->second.model);
                        if(sit != common->special_items.end())
                        {
                            int k;
                            for(k=0;k<(int)sit->second.size();k++)
                            {
                                if(sit->second[k].compare(it2->second[i]) == 0)
                                {
                                    right_ordering.emplace(k, it2->second[i]);
                                    break;
                                }
                            }
                            if(k == (int)sit->second.size())
                                left_ordering.emplace(9999, it2->second[i]);
                        }
                        else
                            left_ordering.emplace(9999, it2->second[i]);
                    }
                    else if(fit->second.checked)
                    {
                        right_ordering.emplace(fit->second.order, fit->first);
                    }
                    else
                    {
                        left_ordering.emplace(fit->second.order, fit->first);
                    }
                }
                for(auto j=right_ordering.begin();j!=right_ordering.end();j++)
                {
                    mc_selection_entry* e = new mc_selection_entry(right_scrollarea);
                    e->setGeometry(0, MC_ENTRY_HEIGHT*right_lines[row].size(),
                                      MC_ENTRY_WIDTH-MC_SCROLLBAR_WIDTH, MC_ENTRY_HEIGHT);
                    right_lines[row].push_back(e);
                    e->set_type(MC_ENTRY_RIGHT);
                    e->set_text(j->second.c_str());
                    e->table_index = right_lines[row].size()-1;
                    e->tab_index = row;
                    e->show();
                    connect(e, SIGNAL(button_press(int)), this, SLOT(on_entry_pressed(int)));
                }
                for(auto j=left_ordering.begin();j!=left_ordering.end();j++)
                {
                    mc_selection_entry* e = new mc_selection_entry(left_scrollarea);
                    e->setGeometry(0, MC_ENTRY_HEIGHT*left_lines[row].size(),
                                     MC_ENTRY_WIDTH-MC_SCROLLBAR_WIDTH, MC_ENTRY_HEIGHT);
                    left_lines[row].push_back(e);
                    e->set_type(MC_ENTRY_LEFT);
                    e->set_text(j->second.c_str());
                    e->table_index = left_lines[row].size()-1;
                    e->tab_index = row;
                    e->show();
                    connect(e, SIGNAL(button_press(int)), this, SLOT(on_entry_pressed(int)));
                }
                left_scrollarea->adjustSize();
                right_scrollarea->adjustSize();
                if(it->second.model.compare("Savina") == 0 || it->second.model.compare("Savina 300") == 0)
                    apply_savina_ordering(&right_lines[row], it->second.model, row);
            }
        }
        row++;
    }
    for(;row<4;row++)
        ui->deviceMetric_tabWidget->setTabVisible(row, false);

    if(device_changed)
    {
        common->item_checkstate.clear();
        int i=0;
        for(auto it=common->md->dm.devices.begin();it!=common->md->dm.devices.end();it++)
        {
            for(int k=0;k<(int)left_lines[i].size();k++)
            {
                mc_checkstate cs;
                cs.checked = 0;
                cs.order = k;
                common->item_checkstate.emplace(it->second.model + "," + ((mc_selection_entry*)left_lines[i][k])->get_text(), cs);
            }
            for(int k=0;k<(int)right_lines[i].size();k++)
            {
                mc_checkstate cs;
                cs.checked = 1;
                cs.order = k;
                common->item_checkstate.emplace(it->second.model + "," + ((mc_selection_entry*)right_lines[i][k])->get_text(), cs);
            }
            i++;
        }
    }
}

static void mc_find_and_swap(int index, std::string name, std::vector<QWidget*>* lines)
{
    if(((mc_selection_entry*)(*lines)[index])->get_text().compare(name) == 0)
    {
        ((mc_selection_entry*)(*lines)[index])->set_type(MC_ENTRY_STATIC);
        return;
    }
    for(int i=0;i<(int)lines->size();i++)
    {
        if(index == i)
            continue;
        if(((mc_selection_entry*)(*lines)[i])->get_text().compare(name) == 0)
        {
            QWidget* t = (*lines)[i];
            (*lines)[i] = (*lines)[index];
            (*lines)[index] = t;
            ((mc_selection_entry*)t)->set_type(MC_ENTRY_STATIC);
            break;
        }
    }
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::apply_savina_ordering(std::vector<QWidget*>* lines, std::string model, int row)
{
    //新增Obs置頂欄位

    QWidget* parent = rightscrollarea[row];
    mc_selection_entry* mv = new mc_selection_entry(parent);
    (*lines).emplace(lines->begin(), mv);
    mv->set_type(MC_ENTRY_STATIC);
    mv->set_text("MV");
    mv->show();
    mc_selection_entry* rsi = new mc_selection_entry(parent);
    (*lines).emplace(lines->begin(), rsi);
    rsi->set_type(MC_ENTRY_STATIC);
    rsi->set_text("RSI");
    rsi->show();
//    mc_selection_entry* ipart = new mc_selection_entry(parent);
//    (*lines).emplace(lines->begin(), ipart);
//    ipart->set_type(MC_ENTRY_STATIC);
//    ipart->set_text("I:E I-part");
//    ipart->show();
//    mc_selection_entry* epart = new mc_selection_entry(parent);
//    (*lines).emplace(lines->begin(), epart);
//    epart->set_type(MC_ENTRY_STATIC);
//    epart->set_text("I:E E-part");
//    epart->show();

    Common* common = Common::instance();
    std::vector<std::string> items;
    if(model.compare("Savina") == 0)
        items = common->special_items["Savina"];
    else
        items = common->special_items["Savina 300"];
    for(int i=0;i<(int)items.size();i++)
        mc_find_and_swap(i, items[i], lines);
    refresh_geometry(lines);
    parent->adjustSize();
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::refresh_geometry(std::vector<QWidget*>* lines)
{
    for(int i=0;i<(int)lines->size();i++)
    {
        (*lines)[i]->setGeometry(0, MC_ENTRY_HEIGHT*i,
                                 MC_ENTRY_WIDTH-MC_SCROLLBAR_WIDTH, MC_ENTRY_HEIGHT);
        ((mc_selection_entry*)(*lines)[i])->table_index = i;
    }
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::on_entry_pressed(int type)
{
    mc_selection_entry* e = (mc_selection_entry*)sender();
    int tab = e->tab_index;
    int i = e->table_index;
    swap_entries(type, e);
    refresh_geometry(&right_lines[tab]);
    refresh_geometry(&left_lines[tab]);
    rightscrollarea[tab]->adjustSize();
    leftscrollarea[tab]->adjustSize();
    Common* common = Common::instance();
    HistoryEntry en;
    en.cmd = HIS_TYPE_ITEM_CHK;
    en.param_str[0] = checkboxes[tab]->text().toStdString();
    en.param[0] = type;
    en.param[1] = i;
    undo_history.push_front(en);
    if((int)undo_history.size() >= common->max_undo_history_entries)
        undo_history.pop_back();
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::swap_entries(int source_type, mc_selection_entry* e)
{
    int i = e->table_index;
    int tab = e->tab_index;
    std::string label = e->get_text();
    if(source_type == MC_ENTRY_LEFT)
    {

        int all_right_lines=0;
        for(int i=0;i<4;++i)
        {
            all_right_lines += right_lines[i].size();
        }
        qDebug()<<all_right_lines;
        if(all_right_lines >= MC_ENTRY_MAX)
        {
            Common* common = Common::instance();
            QString qstr="Select up to "+QString::number(MC_ENTRY_MAX);
            common->msg.setText(qstr);
            common->msg.exec();
            return;
        }
        delete e;
        left_lines[tab].erase(left_lines[tab].begin() + i);
        e = new mc_selection_entry(rightscrollarea[tab]);
        right_lines[tab].push_back(e);
        e->table_index = right_lines[tab].size() - 1;
        e->tab_index = tab;
        e->set_type(MC_ENTRY_RIGHT);
        e->set_text(label.c_str());
        e->show();
        connect(e, SIGNAL(button_press(int)), this, SLOT(on_entry_pressed(int)));
    }
    else
    {
        delete e;
        right_lines[tab].erase(right_lines[tab].begin() + i);
        e = new mc_selection_entry(leftscrollarea[tab]);
        left_lines[tab].push_back(e);
        e->table_index = left_lines[tab].size() - 1;
        e->tab_index = tab;
        e->set_type(MC_ENTRY_LEFT);
        e->set_text(label.c_str());
        e->show();
        connect(e, SIGNAL(button_press(int)), this, SLOT(on_entry_pressed(int)));
    }
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::select_all_pressed(int tab)
{
    Common* common = Common::instance();
    int num_swapped = 0;
    while(left_lines[tab].size() > 0)
    {
        int all_right_lines=0;
        for(int i=0;i<4;++i)
        {
            all_right_lines += right_lines[i].size();
        }
        if(all_right_lines >= MC_ENTRY_MAX)
        {
            QString qstr="Select up to "+QString::number(MC_ENTRY_MAX);
            common->msg.setText(qstr);
            common->msg.exec();
            break;
        }
        swap_entries(MC_ENTRY_LEFT, (mc_selection_entry*)left_lines[tab][0]);
        refresh_geometry(&right_lines[tab]);
        refresh_geometry(&left_lines[tab]);
        rightscrollarea[tab]->adjustSize();
        leftscrollarea[tab]->adjustSize();
        num_swapped++;
    }
    HistoryEntry en;
    en.cmd = HIS_TYPE_SELECTALL;
    en.param_str[0] = checkboxes[tab]->text().toStdString();
    en.param[0] = num_swapped;
    undo_history.push_front(en);
    if((int)undo_history.size() >= common->max_undo_history_entries)
        undo_history.pop_back();
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::clear_pressed(int tab)
{
    int static_count = 0;
    for(int i=0;i<(int)right_lines[tab].size();i++)
    {
        if(((mc_selection_entry*)right_lines[tab][i])->get_type() == MC_ENTRY_STATIC)
            static_count++;
    }
    int num_swapped = 0;
    while((int)right_lines[tab].size() > static_count)
    {
        swap_entries(MC_ENTRY_RIGHT, (mc_selection_entry*)right_lines[tab][static_count]);
        refresh_geometry(&right_lines[tab]);
        refresh_geometry(&left_lines[tab]);
        rightscrollarea[tab]->adjustSize();
        leftscrollarea[tab]->adjustSize();
        num_swapped++;
    }
    Common* common = Common::instance();
    HistoryEntry en;
    en.cmd = HIS_TYPE_CLEAR;
    en.param_str[0] = checkboxes[tab]->text().toStdString();
    en.param[0] = num_swapped;
    undo_history.push_front(en);
    if((int)undo_history.size() >= common->max_undo_history_entries)
        undo_history.pop_back();
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::on_item_toggle(bool toggled)
{
    /*
    Common* common = Common::instance();
    QPushButton* btn = (QPushButton*)sender();
    QString str = ui->deviceMetric_tabWidget->tabText(ui->deviceMetric_tabWidget->currentIndex());
    HistoryEntry e;
    e.cmd = HIS_TYPE_ITEM_CHK;
    e.param_str[0] = str.toStdString();
    e.param_str[1] = btn->text().toStdString();
    e.param = (int)toggled;
    undo_history.push_front(e);
    if((int)undo_history.size() >= common->max_undo_history_entries)
        undo_history.pop_back();*/
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::On_widgetShowed()
{

}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::toggleEnableItemStatus(int row,int column)
{
    /*
    Common* common = Common::instance();
    if(column != 0) return; //只處理column 0的觸發事件
    HistoryEntry e;
    e.cmd = HIS_TYPE_DEVICE_CHK;
    QTableWidgetItem *enableItem = static_cast<QTableWidget*>(this->sender())->item(row, 0);
    if(enableItem->checkState() == Qt::Unchecked)
    {
        enableItem->setCheckState(Qt::Checked);
        e.param = 1;
    }
    else
    {
        enableItem->setCheckState(Qt::Unchecked);
        e.param = 0;
    }
    QTableWidgetItem *modelItem = static_cast<QTableWidget*>(this->sender())->item(row, 1);
    e.param_str[0] = modelItem->text().toStdString();
    undo_history.push_front(e);
    if((int)undo_history.size() >= common->max_undo_history_entries)
        undo_history.pop_back();*/
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::toggleDeviceTabEnable(int row,int column)
{
    /*
    if(column != 0) return; //只處理column 0的觸發事件
    if(this->sender() != ui->devices_tableWidget) return; //只處理devices_tableWidget的觸發事件
    _toggleDeviceTabEnable(row);*/
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::_toggleDeviceTabEnable(int row)
{
    ui->deviceMetric_tabWidget->setTabEnabled(row, !ui->deviceMetric_tabWidget->isTabEnabled(row));
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::restoreButtonsStatus(QScrollArea* tab)
{
    QList<QPushButton*> btnList = tab->findChildren<QPushButton*>();
    for(QPushButton* btn : btnList)
    {
        btn->setChecked(btn->statusTip() == "1" ? true : false);
    }
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::on_save_pushButton_clicked()
{
    Common* common = Common::instance();
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    std::string dummy;
    std::string sql = "SELECT meta().id,data_source,model,checked,source_timestamp.sec,source_timestamp.nanosec FROM _ WHERE data_source='NumericDeviceSelection' AND patient_id='";
    sql.append(common->patient_id);
    sql.append("' AND expired=0");
    sql.append(" AND meta().expiration IS NOT VALUED");
    cbl::ResultSet results= common->cbl->queryDocuments(common->display_items_db, sql, dummy);
    int error=0;while (dummy!="IP200"&&error<5)
        {
        results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
        qDebug()<<QString::fromStdString(dummy);
        fflog_out(common->log,dummy.c_str());error++;
        }
    for(auto& result: results)
    {
        std::string id = result.valueAtIndex(0).asstring();
        std::string model = result.valueAtIndex(2).asstring();
        uint8_t checked = result.valueAtIndex(3).asUnsigned();
        int64_t sec = result.valueAtIndex(4).asInt();
        int64_t nanosec = result.valueAtIndex(5).asInt();

        rapidjson::Document d;
        d.SetObject();
        d.AddMember("data_source", "WaveDeviceSelection", d.GetAllocator());
        d.AddMember("model", rapidjson::Value().SetString(model.c_str(), d.GetAllocator()), d.GetAllocator());
        d.AddMember("checked", checked, d.GetAllocator());
        d.AddMember("patient_id", rapidjson::Value().SetString(common->patient_id.c_str(), d.GetAllocator()), d.GetAllocator());
        d.AddMember("expired", 1, d.GetAllocator());
        rapidjson::Value v;
        v.SetObject();
        v.AddMember("sec", sec, d.GetAllocator());
        v.AddMember("nanosec", nanosec, d.GetAllocator());
        d.AddMember("source_timestamp", v, d.GetAllocator());
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        d.Accept(writer);
        common->cbl->saveMutableDocument(common->display_items_db, buffer.GetString(), id, dummy);

        int64_t now = time(NULL);
        now+=48*60*60;
        now*=1000;
        common->cbl->setDocExpiration(common->display_items_db, id, now, dummy);
    }

    sql = "SELECT meta().id,data_source,model,mdc_code,visibility,display_index,source_timestamp.sec,source_timestamp.nanosec FROM _ WHERE data_source='Obs' AND patient_id='";
    sql.append(common->patient_id);
    sql.append("' AND expired=0");
    sql.append(" AND meta().expiration IS NOT VALUED");
    cbl::ResultSet resultsx= common->cbl->queryDocuments(common->display_items_db, sql, dummy);
    error=0;while (dummy!="IP200"&&error<5)
        {
        resultsx = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
        qDebug()<<QString::fromStdString(dummy);
        fflog_out(common->log,dummy.c_str());error++;
        }
    for(auto& result: resultsx)
    {
        std::string id = result.valueAtIndex(0).asstring();
        std::string model = result.valueAtIndex(2).asstring();
        std::string mdc_code = result.valueAtIndex(3).asstring();
        uint8_t visibility = result.valueAtIndex(4).asUnsigned();
        int display_index = result.valueAtIndex(5).asInt();
        int64_t sec = result.valueAtIndex(6).asInt();
        int64_t nanosec = result.valueAtIndex(7).asInt();

        rapidjson::Document d;
        d.SetObject();
        d.AddMember("data_source", "Obs", d.GetAllocator());
        d.AddMember("model", rapidjson::Value().SetString(model.c_str(), d.GetAllocator()), d.GetAllocator());
        d.AddMember("mdc_code", rapidjson::Value().SetString(mdc_code.c_str(), d.GetAllocator()), d.GetAllocator());
        d.AddMember("visibility", visibility, d.GetAllocator());
        d.AddMember("display_index", display_index, d.GetAllocator());
        d.AddMember("patient_id", rapidjson::Value().SetString(common->patient_id.c_str(), d.GetAllocator()), d.GetAllocator());
        d.AddMember("expired", 1, d.GetAllocator());
        rapidjson::Value v;
        v.SetObject();
        v.AddMember("sec", sec, d.GetAllocator());
        v.AddMember("nanosec", nanosec, d.GetAllocator());
        d.AddMember("source_timestamp", v, d.GetAllocator());
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        d.Accept(writer);
        common->cbl->saveMutableDocument(common->display_items_db, buffer.GetString(), id, dummy);

        int64_t now = time(NULL);
        now+=48*60*60;
        now*=1000;
        common->cbl->setDocExpiration(common->display_items_db, id, now, dummy);
    }

    common->device_checkstate.clear();
    common->item_checkstate.clear();
    undo_history.clear();
    for(int r=0;r<num_devices;r++)
    {
        std::string empty;
        uint8_t checked = 0;
        if(checkboxes[r]->checkState() == Qt::Checked)
            checked = 1;
        common->device_checkstate.emplace(checkboxes[r]->text().toStdString(), checked);
        rapidjson::Document d;
        d.SetObject();
        d.AddMember("data_source", "NumericDeviceSelection", d.GetAllocator());
        d.AddMember("model", rapidjson::Value().SetString(checkboxes[r]->text().toStdString().c_str(), d.GetAllocator()), d.GetAllocator());
        d.AddMember("checked", checked, d.GetAllocator());
        d.AddMember("patient_id", rapidjson::Value().SetString(common->patient_id.c_str(), d.GetAllocator()), d.GetAllocator());
        d.AddMember("expired", 0, d.GetAllocator());

        rapidjson::Value v;
        v.SetObject();
        v.AddMember("sec", ts.tv_sec, d.GetAllocator());
        v.AddMember("nanosec", ts.tv_nsec, d.GetAllocator());
        d.AddMember("source_timestamp", v, d.GetAllocator());
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        d.Accept(writer);
        common->cbl->saveMutableDocument(common->display_items_db, buffer.GetString(), empty, dummy);
    }
    int i=0;
    for(auto it=common->md->dm.devices.begin();it!=common->md->dm.devices.end();it++)
    {
        for(int k=0;k<(int)left_lines[i].size();k++)
        {
            std::string empty;
            mc_checkstate cs;
            cs.checked = 0;
            cs.order = k;
            common->item_checkstate.emplace(it->second.model+","+((mc_selection_entry*)left_lines[i][k])->get_text(), cs);
            rapidjson::Document d;
            d.SetObject();
            d.AddMember("data_source", "Obs", d.GetAllocator());
            d.AddMember("model", rapidjson::Value().SetString(it->second.model.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("mdc_code", rapidjson::Value().SetString(((mc_selection_entry*)left_lines[i][k])->get_text().c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("visibility", cs.checked, d.GetAllocator());
            d.AddMember("display_index", cs.order, d.GetAllocator());
            d.AddMember("patient_id", rapidjson::Value().SetString(common->patient_id.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("expired", 0, d.GetAllocator());
            common->unused_line_color_map(it->second.model,(mc_selection_entry*)left_lines[i][k])->get_text())
            d.AddMember("color", "", d.GetAllocator());

            rapidjson::Value v;
            v.SetObject();
            v.AddMember("sec", ts.tv_sec, d.GetAllocator());
            v.AddMember("nanosec", ts.tv_nsec, d.GetAllocator());
            d.AddMember("source_timestamp", v, d.GetAllocator());
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            d.Accept(writer);
            common->cbl->saveMutableDocument(common->display_items_db, buffer.GetString(), empty, dummy);
        }
        for(int k=0;k<(int)right_lines[i].size();k++)
        {
            std::string empty;
            mc_checkstate cs;
            cs.checked = 1;
            cs.order = k;
            common->item_checkstate.emplace(it->second.model+","+((mc_selection_entry*)right_lines[i][k])->get_text(), cs);
            if(((mc_selection_entry*)right_lines[i][k])->get_text().compare("MV") == 0 ||
               ((mc_selection_entry*)right_lines[i][k])->get_text().compare("RSI") == 0 ||
               ((mc_selection_entry*)right_lines[i][k])->get_text().compare("I:E Ratio") == 0)
                continue;
            //if(((mc_selection_entry*)right_lines[i][k])->get_type() == MC_ENTRY_STATIC)
                //continue;
            rapidjson::Document d;
            d.SetObject();
            d.AddMember("data_source", "Obs", d.GetAllocator());
            d.AddMember("model", rapidjson::Value().SetString(it->second.model.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("mdc_code", rapidjson::Value().SetString(((mc_selection_entry*)right_lines[i][k])->get_text().c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("visibility", cs.checked, d.GetAllocator());
            d.AddMember("display_index", cs.order, d.GetAllocator());
            d.AddMember("patient_id", rapidjson::Value().SetString(common->patient_id.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("expired", 0, d.GetAllocator());
            QColor qcolor =common->use_line_color_list(it->second.model,((mc_selection_entry*)right_lines[i][k])->get_text());
            QString colorRgb = QString::sprintf("rgb(%d, %d, %d)", qcolor.red(), qcolor.green(), qcolor.blue());
            d.AddMember("color", colorRgb.toStdString(), d.GetAllocator());

            rapidjson::Value v;
            v.SetObject();
            v.AddMember("sec", ts.tv_sec, d.GetAllocator());
            v.AddMember("nanosec", ts.tv_nsec, d.GetAllocator());
            d.AddMember("source_timestamp", v, d.GetAllocator());
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            d.Accept(writer);
            common->cbl->saveMutableDocument(common->display_items_db, buffer.GetString(), empty, dummy);
        }
        i++;
    }
/*
    for(int r=0;r<ui->devices_tableWidget->rowCount();r++)
    {
        QTableWidgetItem* item = ui->devices_tableWidget->item(r, 0);
        uint8_t checked = 0;
        if(item->checkState() == Qt::Checked)
            checked = 1;
        QTableWidgetItem* item2 = ui->devices_tableWidget->item(r, 1);
        std::string model = item2->text().toStdString();
        common->device_checkstate.emplace(model, checked);
        rapidjson::Document d;
        d.SetObject();
        d.AddMember("data_source", "NumericDeviceSelection", d.GetAllocator());
        d.AddMember("model", rapidjson::Value().SetString(model.c_str(), d.GetAllocator()), d.GetAllocator());
        d.AddMember("checked", checked, d.GetAllocator());
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        d.Accept(writer);
        common->cbl->saveMutableDocument(common->db, buffer.GetString(), dummy, dummy);
    }
    int i=0;
    for(auto it=common->md->dm.devices.begin();it!=common->md->dm.devices.end();it++)
    {
        for(int k=0;k<(int)btns[i].size();k++)
        {
            common->item_checkstate.emplace(it->second.model + "," + btns[i][k]->text().toStdString(), (unsigned)btns[i][k]->isChecked());
            rapidjson::Document d;
            d.SetObject();
            d.AddMember("data_source", "Obs", d.GetAllocator());
            d.AddMember("model", rapidjson::Value().SetString(it->second.model.c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("mdc_code", rapidjson::Value().SetString(btns[i][k]->text().toStdString().c_str(), d.GetAllocator()), d.GetAllocator());
            d.AddMember("visibility", (unsigned)btns[i][k]->isChecked(), d.GetAllocator());
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            d.Accept(writer);
            common->cbl->saveMutableDocument(common->db, buffer.GetString(), dummy, dummy);
        }
        i++;
    }*/
    if (nosave_checked)
    {
        has_checked =1;
        emit changeToPrevPage();
    }
    else
        has_checked =0;
    common->md->ui->nav->repaint();

}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::on_restore_pushButton_clicked()
{
    while(1)
    {
        auto it = undo_history.begin();
        if(it==undo_history.end())
            return;
        //e.param[0] = tab;
        //e.param[1] = (int)checked;
        if(it->cmd == HIS_TYPE_DEVICE_CHK)
        {
            int i=0;
            for(;i<num_devices;i++)
            {
                if(checkboxes[i]->text().toStdString().compare(it->param_str[0]) == 0)
                    break;
            }
            if(i == num_devices)
            {
                undo_history.pop_front();
                continue;
            }
            if(it->param[1])
            {
                checkboxes[i]->setCheckState(Qt::Unchecked);
                ui->deviceMetric_tabWidget->setTabEnabled(i, false);
            }
            else
            {
                checkboxes[i]->setCheckState(Qt::Checked);
                ui->deviceMetric_tabWidget->setTabEnabled(i, true);
            }
        }
        else if(it->cmd == HIS_TYPE_ITEM_CHK)
        {
            int i=0;
            for(;i<num_devices;i++)
            {
                if(checkboxes[i]->text().toStdString().compare(it->param_str[0]) == 0)
                    break;
            }
            if(i == num_devices)
            {
                undo_history.pop_front();
                continue;
            }
            if(ui->deviceMetric_tabWidget->currentIndex() != i)
            {
                ui->deviceMetric_tabWidget->setCurrentIndex(i);
                return;
            }
            int tab = i;
            if(it->param[0] == MC_ENTRY_LEFT)
            {
                mc_selection_entry* e = (mc_selection_entry*)right_lines[tab][right_lines[tab].size()-1];
                int table_index = e->table_index;
                std::string label = e->get_text();
                delete e;
                right_lines[tab].erase(right_lines[tab].begin()+table_index);

                e = new mc_selection_entry(leftscrollarea[tab]);
                left_lines[tab].insert(left_lines[tab].begin() + it->param[1], e);
                e->tab_index = tab;
                e->set_type(MC_ENTRY_LEFT);
                e->set_text(label.c_str());
                e->show();
                connect(e, SIGNAL(button_press(int)), this, SLOT(on_entry_pressed(int)));
            }
            else if(it->param[0] == MC_ENTRY_RIGHT)
            {
                mc_selection_entry* e = (mc_selection_entry*)left_lines[tab][left_lines[tab].size()-1];
                int table_index = e->table_index;
                std::string label = e->get_text();
                delete e;
                left_lines[tab].erase(left_lines[tab].begin()+table_index);

                e = new mc_selection_entry(rightscrollarea[tab]);
                right_lines[tab].insert(right_lines[tab].begin() + it->param[1], e);
                e->tab_index = tab;
                e->set_type(MC_ENTRY_RIGHT);
                e->set_text(label.c_str());
                e->show();
                connect(e, SIGNAL(button_press(int)), this, SLOT(on_entry_pressed(int)));
            }
            refresh_geometry(&right_lines[tab]);
            refresh_geometry(&left_lines[tab]);
            rightscrollarea[tab]->adjustSize();
            leftscrollarea[tab]->adjustSize();
        }
        else if(it->cmd == HIS_TYPE_SELECTALL)
        {
            int i=0;
            for(;i<num_devices;i++)
            {
                if(checkboxes[i]->text().toStdString().compare(it->param_str[0]) == 0)
                    break;
            }
            if(i == num_devices)
            {
                undo_history.pop_front();
                continue;
            }
            if(ui->deviceMetric_tabWidget->currentIndex() != i)
            {
                ui->deviceMetric_tabWidget->setCurrentIndex(i);
                return;
            }
            int num_swapped = it->param[0];
            int start_index = right_lines[i].size()-num_swapped;
            for(int k=0;k<num_swapped;k++)
            {
                swap_entries(MC_ENTRY_RIGHT, (mc_selection_entry*)right_lines[i][start_index]);
                refresh_geometry(&right_lines[i]);
                refresh_geometry(&left_lines[i]);
                rightscrollarea[i]->adjustSize();
                leftscrollarea[i]->adjustSize();
            }
        }
        else if(it->cmd == HIS_TYPE_CLEAR)
        {
            int i=0;
            for(;i<num_devices;i++)
            {
                if(checkboxes[i]->text().toStdString().compare(it->param_str[0]) == 0)
                    break;
            }
            if(i == num_devices)
            {
                undo_history.pop_front();
                continue;
            }
            if(ui->deviceMetric_tabWidget->currentIndex() != i)
            {
                ui->deviceMetric_tabWidget->setCurrentIndex(i);
                return;
            }
            int num_swapped = it->param[0];
            int start_index = left_lines[i].size()-num_swapped;
            for(int k=0;k<num_swapped;k++)
            {
                swap_entries(MC_ENTRY_LEFT, (mc_selection_entry*)left_lines[i][start_index]);
                refresh_geometry(&right_lines[i]);
                refresh_geometry(&left_lines[i]);
                rightscrollarea[i]->adjustSize();
                leftscrollarea[i]->adjustSize();
            }
        }
        /*
        if(it->cmd == HIS_TYPE_DEVICE_CHK)
        {
            int i=0;
            for(;i<ui->devices_tableWidget->rowCount();i++)
            {
                if(ui->devices_tableWidget->item(i, 1)->text().toStdString().compare(it->param_str[0]) == 0)
                    break;
            }
            if(i == ui->devices_tableWidget->rowCount())
            {
                undo_history.pop_front();
                continue;
            }
            if(it->param == 1)
                ui->devices_tableWidget->item(i,0)->setCheckState(Qt::Unchecked);
            else
                ui->devices_tableWidget->item(i,0)->setCheckState(Qt::Checked);

        }
        else if(it->cmd == HIS_TYPE_ITEM_CHK)
        {
            int i=0;
            for(;i<4;i++)
            {
                if(ui->deviceMetric_tabWidget->tabText(i).toStdString().compare(it->param_str[0]) == 0)
                    break;
            }
            if(i == 4)
            {
                undo_history.pop_front();
                continue;
            }
            if(ui->deviceMetric_tabWidget->currentIndex() != i)
            {
                ui->deviceMetric_tabWidget->setCurrentIndex(i);
                return;
            }
            for(int k=0;k<(int)btns[i].size();k++)
            {
                if(btns[i][k]->text().toStdString().compare(it->param_str[1]) == 0)
                {
                    if(it->param == 1)
                        btns[i][k]->setChecked(false);
                    else
                        btns[i][k]->setChecked(true);
                    undo_history.pop_front();
                    break;
                }
            }
        }*/
        undo_history.pop_front();
        break;
    }
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::on_restoreAll_pushButton_clicked()
{
    has_init = 0;
    undo_history.clear();
    for(int i=0;i<4;i++)
        tab_names[i].clear();
    update_devices();
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::showEvent(QShowEvent *event)
{
    on_restoreAll_pushButton_clicked();
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::checkbox_event(int tab, bool checked)
{
    Common* common = Common::instance();
    ui->deviceMetric_tabWidget->setTabEnabled(tab, checked);
    HistoryEntry e;
    e.cmd = HIS_TYPE_DEVICE_CHK;
    e.param_str[0] = checkboxes[tab]->text().toStdString();
    e.param[0] = tab;
    e.param[1] = (int)checked;
    undo_history.push_front(e);
    if((int)undo_history.size() >= common->max_undo_history_entries)
        undo_history.pop_back();
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::on_checkBox_0_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked)
        checkbox_event(0, true);
    else
        checkbox_event(0, false);
    update_devices();
}



void Tab_Observations_metricItemsDisplayConfigPage_Widget::on_checkBox_1_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked)
        checkbox_event(1, true);
    else
        checkbox_event(1, false);
    update_devices();
}


void Tab_Observations_metricItemsDisplayConfigPage_Widget::on_checkBox_2_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked)
        checkbox_event(2, true);
    else
        checkbox_event(2, false);
    update_devices();
}


void Tab_Observations_metricItemsDisplayConfigPage_Widget::on_checkBox_3_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked)
        checkbox_event(3, true);
    else
        checkbox_event(3, false);
    update_devices();
}

void Tab_Observations_metricItemsDisplayConfigPage_Widget::on_tab0_selectall_clicked()
{
    select_all_pressed(0);
}


void Tab_Observations_metricItemsDisplayConfigPage_Widget::on_tab0_clear_clicked()
{
    clear_pressed(0);
}


void Tab_Observations_metricItemsDisplayConfigPage_Widget::on_tab1_selectall_clicked()
{
    select_all_pressed(1);
}


void Tab_Observations_metricItemsDisplayConfigPage_Widget::on_tab1_clear_clicked()
{
    clear_pressed(1);
}


void Tab_Observations_metricItemsDisplayConfigPage_Widget::on_tab2_selectall_clicked()
{
    select_all_pressed(2);
}


void Tab_Observations_metricItemsDisplayConfigPage_Widget::on_tab2_clear_clicked()
{
    clear_pressed(2);
}


void Tab_Observations_metricItemsDisplayConfigPage_Widget::on_tab3_selectall_clicked()
{
    select_all_pressed(3);
}


void Tab_Observations_metricItemsDisplayConfigPage_Widget::on_tab3_clear_clicked()
{
    clear_pressed(3);
}

