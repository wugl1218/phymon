#include "mc_wavepanel.h"
#include "Common.h"
#include "MainDialog.h"
#include "Tab_Observations_mainPage_Widget.h"
#include "ui_Tab_Observations_mainPage_Widget.h"
#include "mc_btn_Clickable.h"
#include <stdlib.h>

#define LOWER_MAX       30
#define WAVE_TIMER      50     //no matter the value is, we can only read 25 data from topic
#define ADD_BTN_POS     MAX_WAVE + 1
#define LOOPS_NAME      "loops"


mc_wavepanel::mc_wavepanel(QWidget *parent)
    : QWidget{parent},
      menu(parent)
{
    m_DeviceName = "Savina";
    controls_on = 1;
    m_Timer.setInterval(WAVE_TIMER);
    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(UpdateWave()));
    m_Timer.start();
    m_RtLowerCount = m_ObLowerCount = 0;
    m_bDrawlayout = false;
    m_setup = false;
}
void mc_wavepanel::WriteNurseDB(stDisplayItems item, bool bDel)
{
    int visibility = bDel ? 0:1;
    Common* common = Common::instance();
    rapidjson::Document d;
    d.SetObject();
    d.AddMember("data_source", "RTO", d.GetAllocator());
    d.AddMember("patient_id", rapidjson::Value().SetString(common->patient_id.c_str(), d.GetAllocator()), d.GetAllocator());
    d.AddMember("model", rapidjson::Value().SetString(item.model.c_str(), d.GetAllocator()), d.GetAllocator());
    d.AddMember("mdc_code", rapidjson::Value().SetString(item.mdc_code.c_str(), d.GetAllocator()), d.GetAllocator());
    d.AddMember("display_desc", rapidjson::Value().SetString(item.display_desc.c_str(), d.GetAllocator()), d.GetAllocator());
    d.AddMember("y_max", rapidjson::Value().SetString(item.y_max.c_str(), d.GetAllocator()), d.GetAllocator());
    d.AddMember("y_min", rapidjson::Value().SetString(item.y_min.c_str(), d.GetAllocator()), d.GetAllocator());
    d.AddMember("y_step", rapidjson::Value().SetString(item.y_step.c_str(), d.GetAllocator()), d.GetAllocator());
    d.AddMember("visibility", visibility, d.GetAllocator());
    d.AddMember("display_index", item.display_index, d.GetAllocator());
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    std::string dummy;
    common->cbl->saveMutableDocument(common->display_items_db, buffer.GetString(), item.record_id, dummy);
    //qDebug()<<"*** WriteNurseDB saveMutableDocument error="<< dummy.c_str()<< " desc="<<item.display_desc.c_str()<<" record_id="<<item.record_id.c_str();
    if (bDel && item.record_id.length())
    {
        int64_t now = time(NULL);
        common->cbl->setDocExpiration(common->display_items_db, item.record_id, now * 1000, dummy);
        qDebug()<<"====dummy="<<dummy.c_str();
    }
}
static bool dbCompare(dbDisplayItems item1, dbDisplayItems item2)
{
    return item1.display_index < item2.display_index;
}
static bool stCompare(stDisplayItems item1, stDisplayItems item2)
{
    return item1.display_desc < item2.display_desc;
}
void mc_wavepanel::add_clicked()
{
    menu.clear_tabs();
    stDisplayItems item, loop_item;// Agooda 20230224 Qt can not use item = {0};
    std::vector<stDisplayItems> items = GetDisplayIntersec("Savina", "RTO");
    std::vector<stDisplayItems> unselect_items;
    CheckNurseDB();

    int loops = 0, loops_existed = false;
    if (m_DeviceName == "Savina" || m_DeviceName == "Savina 300")
        loops = 1;
    //qDebug()<<"====add_clicked items.size= "<<items.size();
    for (auto all: items)
    {
        bool existed = false;
        for (auto picked: m_nurse_items)
        {
            if (m_DeviceName == picked.model && picked.display_desc == LOOPS_NAME)
            {
                loops = (picked.visibility) ? 0:1;
                loop_item.record_id = picked.record_id;
                loops_existed = true;
            }
            if (all.mdc_code == picked.mdc_code && all.model == picked.model)
            {
                if (picked.visibility)
                    existed = true;
                else
                    all.record_id = picked.record_id;
                break;
            }
        }
        if (!existed)
            unselect_items.push_back(all);
    }
    if (loops)
    {
        if (loops_existed)
            unselect_items.push_back(loop_item);
        else
        {
            item.display_desc = "loops";
            item.model = m_DeviceName;
            unselect_items.push_back(item);
        }
    }
    std::sort(unselect_items.begin(), unselect_items.end(), stCompare);
    menu.add_tab(m_DeviceName.c_str(), &unselect_items, loops);
    menu.exec();
    printf("selected item=%s, index=%d\n", menu.selected_item.c_str(), menu.m_selected_index);
    if (menu.m_selected_index < 0 || menu.m_selected_index > (int)items.size())
        return;

    int repeat = 0;
    for (int i = 0; i < (int) m_nurse_items.size(); i++)
    {
       if (unselect_items[menu.m_selected_index].display_desc == m_nurse_items[i].display_desc)
           repeat++;
    }
    if (repeat < 2)
    {
        int j = menu.m_selected_index;
        if (m_nurse_items.size() > 1 && m_nurse_items[m_nurse_items.size() - 1].display_desc == LOOPS_NAME)
            unselect_items[j].display_index = m_nurse_items.size() - 1;     // insert before loops
        else if (unselect_items[j].display_desc == LOOPS_NAME)              // append tail
            unselect_items[j].display_index = MAX_WAVE;
        else
            unselect_items[j].display_index = m_nurse_items.size();
        qDebug()<<"===== add one index="<<unselect_items[j].display_index<<" "<<item.display_desc.c_str();
        WriteNurseDB(unselect_items[j]);
    }
    else
        qDebug()<<"====repeat:"<< unselect_items[menu.m_selected_index].display_desc.c_str();

    push_add_item(true, m_nurse_items.size() >= items.size());
}
void mc_wavepanel::mc_add_clicked(mc_wavepanel* wp)
{
    wp->add_clicked();
}
void mc_wavepanel::mc_del_clicked(int index)
{
    stDisplayItems item;
    size_t j = 0;
    if (index < 0)
        return;
    if (m_nurse_items.size() == 1)       //only one
    {
        item.display_desc = m_nurse_items[0].display_desc;
        item.mdc_code = m_nurse_items[0].mdc_code;
        item.record_id = m_nurse_items[0].record_id;
        item.model = m_nurse_items[0].model;
        qDebug()<<"===== del only one "<<item.display_desc.c_str();
        WriteNurseDB(item, true);
    }
    else if (index == MAX_WAVE)          //loops
    {
        j = m_nurse_items.size() - 1;
        item.display_desc = m_nurse_items[j].display_desc;
        item.mdc_code = m_nurse_items[j].mdc_code;
        item.record_id = m_nurse_items[j].record_id;
        item.model = m_nurse_items[j].model;
        qDebug()<<"===== del loops "<<item.display_desc.c_str();
        WriteNurseDB(item, true);
    }
    else if (index == (int) m_nurse_items.size() - 1)
    {
        item.display_desc = m_nurse_items[index].display_desc;
        item.mdc_code = m_nurse_items[index].mdc_code;
        item.record_id = m_nurse_items[index].record_id;
        item.model = m_nurse_items[index].model;
        qDebug()<<"===== del last one "<<item.display_desc.c_str();
        WriteNurseDB(item, true);
    }
    else
    {
        j = 0;
        for(size_t i = 0; i < m_nurse_items.size();i++)
        {
            if (m_nurse_items[i].display_desc == LOOPS_NAME)
                continue;
            item.display_index = j++;
            item.display_desc = m_nurse_items[i].display_desc;
            item.mdc_code = m_nurse_items[i].mdc_code;
            item.record_id = m_nurse_items[i].record_id;
            item.model = m_nurse_items[i].model;
            qDebug()<<"===== del middle one i="<<i<<" j="<<j<<" index="<<index<<" desc="<<item.display_desc.c_str();
            if (i == (size_t)index)
                WriteNurseDB(item, true);
            else
                WriteNurseDB(item);
        }
    }
    push_add_item(true);
    Common* common = Common::instance();

    if(m_nurse_items.size()%2==1)
    {
        common->observation_main_page->ui->loop_frame->setStyleSheet("background-color: rgb(7, 22, 40);");
        common->observation_main_page->ui->option_loop->setStyleSheet("background-color: rgb(3, 41, 86);");
    }
    else
    {
        common->observation_main_page->ui->loop_frame->setStyleSheet("background-color: rgb(11, 42, 78);");
        common->observation_main_page->ui->option_loop->setStyleSheet("background-color: rgb(9, 58, 115);");
    }
}
void mc_wavepanel::add_frame_control()
{
    bool loops = false;
    //CheckNurseDB();
    for (auto item: m_nurse_items)
        if (item.display_desc == LOOPS_NAME)
            loops = true;
    if (m_nurse_items.size() > m_WaveRtItems.size())
    {
        if (m_DeviceName == "Savina" || m_DeviceName == "Savina 300")
        {
            if (loops)
                m_add_frame->setHidden(1);
            else
                m_add_frame->setHidden(0);
        }
    }
    else if (m_nurse_items.size() >=  MAX_WAVE || (m_nurse_items.size() == MAX_WAVE - 2 && loops))
        m_add_frame->setHidden(1);
    else
        m_add_frame->setHidden(0);
}
void mc_wavepanel::push_add_item(bool bAddDel, bool bAllListed)
{
    bool loops = false;
    CheckNurseDB();
    for (auto item: m_nurse_items)
        if (item.display_desc == LOOPS_NAME)
            loops = true;
    qDebug()<<"=====m_nurse_items.size="<< m_nurse_items.size()<<" loops="<<loops<< " m_WaveRtItems="<<m_WaveRtItems.size();

    for (int i = 0; i < ADD_BTN_POS;i++)        //Clear all buttons
    {
        m_RTO_chart_list[i]->hide();
        m_RTO_option_list[i]->hide();
        m_RTO_minus_list[i]->hide();
        m_RTO_name_list[i]->hide();
    }
    for (int i = 0; i < m_nurse_items.size();i++)
    {
        //qDebug()<<"=====i="<<i<<" desc="<< m_nurse_items[i].display_desc.c_str();
        if (m_nurse_items[i].display_desc == LOOPS_NAME)
            continue;
        m_RTO_chart_list[i]->show();
        m_RTO_option_list[i]->show();
        if (bAddDel)
            m_RTO_minus_list[i]->show();
        m_RTO_name_list[i]->show();

    }
    for (int i = m_nurse_items.size(); i < m_nurse_items.size() && i < MAX_WAVE;i++)
    {
        m_RTO_chart_list[i]->hide();
        m_RTO_option_list[i]->hide();
       // m_RTO_minus_list[i]->hide();
        m_RTO_name_list[i]->hide();
    }
    if (loops)
        m_loop_frame->setHidden(0);
    else
        m_loop_frame->setHidden(1);
    if (bAddDel)
        m_loop_minus->setHidden(0);
    else
        m_loop_minus->setHidden(1);

    if (!m_nurse_items.size())
        m_add_frame->show();
    else
    {
        qDebug()<<"====== bAddDel="<<bAddDel<<" bAllListed="<<bAllListed;
        if (bAddDel)
        {
            if (bAllListed)
            {
                m_add_frame->hide();
                m_add_btn->hide();
            }
            else
            {
                m_add_frame->show();
                m_add_btn->show();
            }
        }
        else
        {
            m_add_frame->setHidden(0);
            m_add_btn->hide();
        }
    }
    if (bAddDel)
        m_setup = true;
    for (int i = 0; i < m_main_item->count();i++)       //clear all stretch
        m_main_item->setStretch(i,0);
    if (!m_nurse_items.size())                          // add_item only
        m_main_item->setStretch(ADD_BTN_POS,1);
//    else if (m_nurse_items.size() == 1 && m_nurse_items[0].display_desc == LOOPS_NAME)
//        m_main_item->setStretch(0,1);                   // loops only

    for(int t = 0;t < MAX_WAVE;t++)
    {
        m_rtchart_wave_list[t].clear();
        m_rtchart_time_list[t].clear();
    }
    for (int i = 0; i < (int)m_nurse_items.size() && i <MAX_WAVE;i++)
    {
        if (m_nurse_items[i].display_desc == LOOPS_NAME)
            continue;

        std::string temp = m_nurse_items[i].display_desc + "\n(" + m_nurse_items[i].model + ")";
        m_RTO_name_list[i]->setText(temp.c_str());
        m_RTO_name_list[i]->setStyleSheet("color:rgb(255,255,255);font-size:16pt");
        mc_chart *pChart = m_RTO_chart_list[i];
        pChart->clear_points(0);
        pChart->set_axis_visible(1);
        pChart->set_selection_width(40);
        pChart->set_selection_type(MC_SELECT_SERIES);
        pChart->set_series_width(0,2);
        pChart->set_view_range_max_y(m_nurse_items[i].y_max);
        pChart->set_view_range_min_y(m_nurse_items[i].y_min);
        pChart->set_scrollable(0);
        pChart->set_zoomable(0);
        pChart->set_num_labels_x(5);
        pChart->set_num_labels_y(5);
        pChart->set_series_color(0, QColor(255,255,255));
        if (m_nurse_items.size() == 2 && loops)
        {
            pChart->set_num_labels_x(3);
            m_wave_interval=9.23*1000;
        }
        else if (m_nurse_items.size()== 3 && loops)
        {
            pChart->set_num_labels_x(5);
            m_wave_interval=9.23*1000;
        }
        else if (m_nurse_items.size()== 4 && loops)
        {
            pChart->set_num_labels_x(5);
            m_wave_interval=10.5*1000;
        }
        else if (m_nurse_items.size()== 5 && loops)
        {
            pChart->set_num_labels_x(5);
            m_wave_interval=13.7*1000;
        }
        else if (m_nurse_items.size()== 1)
        {
            pChart->set_num_labels_x(3);
            m_wave_interval=9.23*1000;
        }
        else if (m_nurse_items.size()== 2)
        {
            pChart->set_num_labels_x(3);
            m_wave_interval=9.23*1000;
        }
        else if (m_nurse_items.size()== 3)
        {
            pChart->set_num_labels_x(5);
            m_wave_interval=9.23*1000;
        }
        else if (m_nurse_items.size()== 4)
        {
            pChart->set_num_labels_x(5);
            m_wave_interval=15.93*1000;
        }
        else if (m_nurse_items.size()== 5)
        {
            pChart->set_num_labels_x(5);
            m_wave_interval=20.31*1000;
        }
        else if (m_nurse_items.size()== 6)
        {
            pChart->set_num_labels_x(5);
            m_wave_interval=24.74*1000;
        }
        std::vector<float> vals;
        uint64_t time = 0.0;
        m_rtchart_wave_list[i]<<vals;
        m_rtchart_time_list[i]<<time;
    }
}
bool mc_wavepanel::IsRepeat(dbDisplayItems item)
{
    for (auto picked:m_nurse_items)
        if (item.display_desc == picked.display_desc)
            return true;
    return false;
}
std::vector<dbDisplayItems> mc_wavepanel::CheckNurseDB(bool bListAll)
{
    Common* common = Common::instance();
    dbDisplayItems item, loops_item;
    std::string dummy;//ppee

/*  std::string sql1 = "SELECT meta().id FROM _ WHERE data_source='RTO'";
    cbl::ResultSet results3 = common->cbl->queryDocuments(common->display_items_db, sql1,dummy);
    for(auto& result: results3)
    {
        common->cbl->purgeDocument(common->display_items_db,result.valueAtIndex(0).asstring(),dummy);
    }
*/
    std::string sql = "SELECT display_desc, y_max, y_min, model, y_step, display_index, visibility, mdc_code, meta().id FROM _ WHERE data_source='RTO' AND patient_id='";
    sql.append(common->patient_id);
    sql.append("' AND visibility = 1 AND meta().expiration IS NOT VALUED");
    cbl::ResultSet results2 = common->cbl->queryDocuments(common->display_items_db, sql, dummy);

    int error=0;
    while (dummy!="IP200"&&error<5)
    {
        results2 = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
        fflog_out(common->log,dummy.c_str());error++;
    }
    m_nurse_items.clear();
    bool loops = false;

    if (bListAll)
    {
        int i = 0;
        for(auto& result: results2)
        {
            qDebug()<<"=====["<<i<<"] desc="<<result.valueAtIndex(0).asstring().c_str();
            i++;
        }
        qDebug()<<"========Check Timer total="<<i;
        return m_nurse_items;
    }
    for(auto& result: results2)
    {
        item.display_desc = result.valueAtIndex(0).asstring();
        std::string temp = result.valueAtIndex(1).asstring();
        item.y_max = atoi(temp.c_str());
        temp = result.valueAtIndex(2).asstring();
        item.y_min = atoi(temp.c_str());
        item.model = result.valueAtIndex(3).asstring();
        item.y_step = result.valueAtIndex(4).asInt();
        item.display_index = result.valueAtIndex(5).asInt();
        item.visibility = result.valueAtIndex(6).asInt();
        item.mdc_code = result.valueAtIndex(7).asstring();
        item.record_id = result.valueAtIndex(8).asstring();
        if (item.display_desc == LOOPS_NAME)
        {
            loops_item = item;
            loops = true;
            continue;
        }
        if (loops)
        {
            if (m_nurse_items.size() < MAX_WAVE - 2)
                if (!IsRepeat(item))
                    m_nurse_items.push_back(item);
        }
        else if (m_nurse_items.size() < MAX_WAVE)
            if (!IsRepeat(item))
                m_nurse_items.push_back(item);
        if (loops && m_nurse_items.size() == MAX_WAVE - 2)
            break;
        if (!loops && m_nurse_items.size() == MAX_WAVE)
            break;
    }
    if (loops && m_nurse_items.size() <= MAX_WAVE - 2)
        m_nurse_items.push_back(loops_item);

    if (m_nurse_items.size() > 1)
        std::sort(m_nurse_items.begin(), m_nurse_items.end(), dbCompare);
    for(int i = 0; i < (int)m_nurse_items.size();i++)
        qDebug()<<"====CheckNurseDB i="<<i<<" "<<m_nurse_items[i].display_desc.c_str();
    return m_nurse_items;
}
void mc_wavepanel::controls_clicked()
{
    controls_on = !controls_on;
    render_controls_btn();
}

void mc_wavepanel::render_controls_btn()
{
    if(controls_on)
        controls_btn->setPixmap(QPixmap(":/icons/controls_btn1.png"));
    else
        controls_btn->setPixmap(QPixmap(":/icons/controls_btn0.png"));
}
void mc_wavepanel::UpdateWave()
{
    dbDisplayItems item;
    m_DeviceName = "Savina";
    QueryRtItems(m_DeviceName);
    QueryObItems(m_DeviceName);
    if (!m_DisplayItems.size())
        QueryDisplayItems();
    Common* common = Common::instance();
    if (!m_bDrawlayout && common->patient_id.size())
    {
        push_add_item();
        m_bDrawlayout = true;
    }
    if (m_bDrawlayout)
    {
        for (int i = 0; i < (int) m_nurse_items.size() && i < MAX_WAVE;i++)
        {
            if(i % 2==1)
            {
                common->observation_main_page->ui->loop_frame->setStyleSheet("background-color: rgb(7, 22, 40);");
                common->observation_main_page->ui->option_loop->setStyleSheet("background-color: rgb(3, 41, 86);");
            }
            else
            {
                common->observation_main_page->ui->loop_frame->setStyleSheet("background-color: rgb(11, 42, 78);");
                common->observation_main_page->ui->option_loop->setStyleSheet("background-color: rgb(9, 58, 115);");
            }
            if (m_nurse_items[i].display_desc == LOOPS_NAME)
                continue;
            add_wave_to_chart_RTO(0,
                        m_DeviceName, m_nurse_items[i].mdc_code,
                        common->rtobservation_wave_reader,
                        m_RTO_chart_list[i],
                        m_rtchart_wave_list[i],
                        m_rtchart_time_list[i]);
            m_RTO_chart_list[i]->trim_left();
        }
    }
}

std::vector<std::string> mc_wavepanel::QueryRtItems(std::string DeviceName)
{
    try
    {
        std::string CapturedIssues_model;
        std::vector<std::string> items;
        Common* common = Common::instance();
        if(common->patient_id.size() == 0)
            return items;
        CapturedIssues_model = DeviceName;
        auto fit = common->md->dm.devices.find(DeviceName);
        if(fit == common->md->dm.devices.end())
        {
            if (DeviceName == "Savina")
            {
                fit = common->md->dm.devices.find("Savina 300");
                if(fit == common->md->dm.devices.end())
                    return items;
                else
                    CapturedIssues_model="Savina 300";
            }
            else
                return items;
        }
        std::string querystr = "vmd_id MATCH '";
        querystr.append(common->vmd_id);
        querystr.append("' AND patient_id MATCH '");
        querystr.append(common->patient_id);
        querystr.append("' AND model MATCH '");
        querystr.append(fit->first);
        querystr.append("'");
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples2;
        dds::sub::cond::QueryCondition qcond2(
                dds::sub::Query(common->rtobservation_wave_reader, querystr),
                dds::sub::status::DataState(
                dds::sub::status::SampleState::any(),
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
        samples2 = common->rtobservation_wave_reader.select().condition(qcond2).read();
        if(samples2.length() == 0)
            return items;
        for(auto& sample : samples2)
        {
            if(sample.info().valid())
            {   //need "DisplayDesc"
                dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
                std::string mdc_code=data.value<std::string>("mdc_code").c_str();
                items.push_back(mdc_code);
            }
        }
        if (items.size() > m_WaveRtItems.size())
        {
            m_WaveRtItems = items;
            m_RtLowerCount = 0;
        }
        else
        {
            m_RtLowerCount++;
            if (m_RtLowerCount > LOWER_MAX)
            {
                m_WaveRtItems = items;
                m_RtLowerCount = 0;
                printf("*** m_WaveRtItems Reduce\n");
            }
        }
        return items;
    }
    catch(...)
    {
        printf("mc_wavepanel::QueryRtItems exception\n");
    }
}
std::vector<std::string> mc_wavepanel::QueryObItems(std::string DeviceName)
{
    try
    {
        std::string CapturedIssues_model;
        std::vector<std::string> items;
        Common* common = Common::instance();
        if(common->patient_id.size() == 0)
            return items;
        CapturedIssues_model = DeviceName;
        auto fit = common->md->dm.devices.find(DeviceName);
        if(fit == common->md->dm.devices.end())
        {
            if (DeviceName == "Savina")
            {
                fit = common->md->dm.devices.find("Savina 300");
                if(fit == common->md->dm.devices.end())
                    return items;
                else
                    CapturedIssues_model="Savina 300";
            }
            else
                return items;
        }
        std::string querystr = "vmd_id MATCH '";
        querystr.append(common->vmd_id);
        querystr.append("' AND patient_id MATCH '");
        querystr.append(common->patient_id);
        querystr.append("' AND model MATCH '");
        querystr.append(fit->first);
        querystr.append("'");
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples2;
        dds::sub::cond::QueryCondition qcond2(
                dds::sub::Query(common->observation_wave_reader, querystr),
                dds::sub::status::DataState(
                dds::sub::status::SampleState::any(),
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
        samples2 = common->observation_wave_reader.select().condition(qcond2).read();
        if(samples2.length() == 0)
            return items;
        for(auto& sample : samples2)
        {
            if(sample.info().valid())
            {   //need "DisplayDesc"
                dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
                std::string mdc_code=data.value<std::string>("mdc_code").c_str();
                //printf("****agooda m_WaveObItems mdc_code=%s\n", mdc_code.c_str());
                items.push_back(mdc_code);
            }
        }
        if (items.size() > m_WaveObItems.size())
            m_WaveObItems = items;
        if (items.size() > m_WaveObItems.size())
        {
            m_WaveObItems = items;
            m_ObLowerCount = 0;
        }
        else
        {
            m_ObLowerCount++;
            if (m_ObLowerCount > LOWER_MAX)
            {
                m_WaveObItems = items;
                m_ObLowerCount = 0;
                printf("***m_WaveObItems Reduce\n");
            }
        }
        return items;
    }
    catch(...)
    {
        printf("mc_wavepanel::QueryObItems exception\n");
    }
}

bool mc_wavepanel::QueryDisplayItems(void)
{
    stDisplayItems item;
    try
    {
        std::string CapturedIssues_model;
        std::vector<stDisplayItems> *items = new std::vector<stDisplayItems>;//Agooda20230224:stack is not enough, will cause exception, need heap
        Common* common = Common::instance();
        dds::sub::cond::ReadCondition *item_cond;
        if (common->domain_id == -1)
            return false;
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples2;
        item_cond = new dds::sub::cond::ReadCondition(
                    common->m_DisplayItem_reader,
                    dds::sub::status::DataState(
                    dds::sub::status::SampleState::any(),
                    dds::sub::status::ViewState::any(),
                    dds::sub::status::InstanceState::alive()));
        samples2 = common->m_DisplayItem_reader.select().condition(*item_cond).read();
        printf("*** QueryDisplayItems length()=%d\n", samples2.length());

        if(samples2.length() == 0)
            return false;
        for(auto& sample : samples2)
        {
            if(sample.info().valid())
            {
                dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
                item.display_desc = data.value<std::string>("display_desc").c_str();
                item.mdc_code = data.value<std::string>("mdc_code").c_str();
                item.model = data.value<std::string>("model").c_str();
                item.wave_type = data.value<std::string>("wave_type").c_str();
                item.y_max = data.value<std::string>("y_max").c_str();      //Agooda 20230223:Qt's Bug:string is more stable than int
                item.y_min = data.value<std::string>("y_min").c_str();
                item.y_step = data.value<std::string>("y_step").c_str();
                items->push_back(item);
            }
        }
        if (items->size() > m_DisplayItems.size())
            m_DisplayItems = *items;
        else
            delete items;
        delete item_cond;
        return true;
    }
    catch(...)
    {
    }
}

std::vector<stDisplayItems> mc_wavepanel::GetDisplayIntersec(std::string model, std::string type)
{
    std::vector<stDisplayItems> items;
    std::vector<std::string> *source;
    if (type == "RTO")
        source = &m_WaveRtItems;
    else
        source = &m_WaveObItems;
    try
    {
        printf("*** GetDisplayIntersec model=%s, type=%s, source size=%d all size=%d\n", model.c_str(), type.c_str(), source->size(), m_DisplayItems.size());
        for (int i = 0; i < (int)m_DisplayItems.size();i++)
        {
            for(auto& sample : *source)
            {
                if (sample == m_DisplayItems[i].mdc_code &&
                    model == m_DisplayItems[i].model &&
                    type == m_DisplayItems[i].wave_type)
                {
                    items.push_back(m_DisplayItems[i]);
                    printf("*** GetDisplayIntersec %s\n", m_DisplayItems[i].display_desc.c_str());
                }
            }
        }
    }
    catch(...)
    {
        printf("mc_wavepanel::GetRTOIntersec exception\n");
    }
    return items;
}
void mc_wavepanel::add_wave_to_chart_RTO(int series_index, std::string model, std::string mdc_code,
                                                             dds::sub::DataReader<dds::core::xtypes::DynamicData> reader,
                                                             mc_chart* chart,
                                                             QList<std::vector<float>> &wave_list,
                                                             QList<uint64_t> &time_list)
{
    Common* common = Common::instance();
    if(common->patient_id.size() == 0)
        return;
    int line_break_delta;
    line_break_delta=common->RTO_line_break_delta;
    chart->set_line_break_delta(common->RTO_line_break_delta);
    std::string querystr = "vmd_id MATCH '";
    querystr.append(common->vmd_id);
    querystr.append("' AND patient_id MATCH '");
    querystr.append(common->patient_id);
    querystr.append("' AND model MATCH '");
    querystr.append(model);
    querystr.append("' AND mdc_code MATCH '");
    querystr.append(mdc_code);
    querystr.append("'");

    dds::sub::cond::QueryCondition qcond(
                dds::sub::Query(reader, querystr),
                dds::sub::status::DataState(
                dds::sub::status::SampleState::any(),
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
    dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples2 = reader.select().condition(qcond).take();

    if(samples2.length() == 0)
        return;
    for(auto& sample : samples2)
    {
        if(sample.info().valid())
        {
            dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
            rti::core::xtypes::LoanedDynamicData loaned_member = data.loan_value("source_timestamp");
            int32_t sec = loaned_member.get().value<int32_t>("sec");
            uint32_t nsec = loaned_member.get().value<uint32_t>("nanosec");
            loaned_member.return_loan();
            time_t now = time(NULL);
            now -= 3;
            if(sec < now)
                continue;
            uint64_t t = ((uint64_t)sec)*1000 + ((uint64_t)nsec)/1000000;
            std::vector<float> vals;
            auto left_over_rtchart_vals =wave_list[series_index];
            auto last_rtchart_time =time_list[series_index];
            data.get_values("values", vals);
            if (last_rtchart_time && t < last_rtchart_time)
                continue;
            if(left_over_rtchart_vals.size() > 0)
            {
                if(t - last_rtchart_time < line_break_delta)
                {
                    double delta = (t-last_rtchart_time)/((double)left_over_rtchart_vals.size()+1);
                    for(int i=0;i<(int)left_over_rtchart_vals.size();i++)
                        chart->add_point(series_index, last_rtchart_time+delta*(i+1), left_over_rtchart_vals[i]);
                }
                left_over_rtchart_vals.clear();
            }
            if(t > chart->get_view_range_max_x())
            {
                chart->set_view_range_max_x(t);
                chart->set_view_range_min_x(t-m_wave_interval);
            }
            if(vals.size() > 0)
            {
                chart->add_point(series_index, t, vals[0]);
                if(vals.size() > 1)
                {
                    vals.erase(vals.begin());
                    left_over_rtchart_vals = vals;
                    last_rtchart_time = t;
                }
                wave_list[series_index]=left_over_rtchart_vals;
                time_list[series_index]=last_rtchart_time ;
            }
        }
    }
}
