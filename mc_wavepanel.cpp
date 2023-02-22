#include "mc_wavepanel.h"
#include "Common.h"
#include "MainDialog.h"
#include "Tab_Observations_mainPage_Widget.h"
#include "ui_Tab_Observations_mainPage_Widget.h"

#define LOWER_MAX       30

mc_wavepanel::mc_wavepanel(QWidget *parent)
    : QWidget{parent},
      menu(parent)
{
    m_DeviceName = "Savina";
    controls_on = 1;
    //connect(controls_btn, SIGNAL(clicked()), this, SLOT(controls_clicked()));

    m_Timer.setInterval(1000);
    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(UpdateWave()));
    m_Timer.start();
    m_RtLowerCount = m_ObLowerCount = 0;
    m_bDrawlayout = false;
}

void mc_wavepanel::add_clicked()
{
    menu.clear_tabs();
/*    std::vector<std::string> items = GetDisplayIntersec("Savina", "TPO");

    for (int i = 0; i < items.size(); i++)
        printf("*** items[%d]=%s\n", i, items[i].c_str());

    for (int i = 0; i < 20; i++)
    {
        char buf[10];
        sprintf(buf, "ECG%02d", i);
        items.push_back(buf);
    }
    menu.add_tab(m_DeviceName.c_str(), &items, 1);
    menu.exec();
    printf("selected item=%s, selected tab=%s\n", menu.selected_item.c_str(), menu.selected_tab_name.c_str());
    */
}

static void mc_add_clicked(void* param)
{
    mc_wavepanel* wp = (mc_wavepanel*)param;
    wp->add_clicked();
}

void mc_wavepanel::push_add_item()
{
    Common* common = Common::instance();

    qDebug()<<"======push_add_item\n";
    //for (auto &item: m_RTO_wave_list)
    //{
        mc_chart *pChart = m_RTO_chart_list[0];
        pChart->set_axis_visible(1);
        pChart->set_selection_width(40);
        pChart->set_selection_type(MC_SELECT_SERIES);
        pChart->set_series_width(0,2);
        pChart->set_scrollable(0);
        pChart->set_zoomable(0);
        pChart->set_view_range_max_y(200);
        pChart->set_view_range_min_y(0);
        pChart->set_num_labels_x(5);
        pChart->set_num_labels_y(5);
        pChart->set_series_color(0, QColor(255,255,255));

        for(int t = 0;t < MAX_WAVE;++t)
        {
            std::vector<float> vals;
            uint64_t time;
            m_rtchart1_wave_list<<vals;
            m_rtchart1_time_list<<time;
        }
    //}
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
    printf("Agooda UpdateWave\n");
    m_DeviceName = "Savina";
<<<<<<< Updated upstream
    QueryRtItems(m_DeviceName);
    QueryObItems(m_DeviceName);
    QueryDisplayItems();
    GetDisplayIntersec("Savina", "Obs");
=======
    //QueryRtItems(m_DeviceName);
    //QueryObItems(m_DeviceName);
    //QueryDisplayItems();
    //GetDisplayIntersec("Savina", "Obs");
    Common* common = Common::instance();

    if (!m_bDrawlayout && common->patient_id.size())
    {
        push_add_item();
        m_bDrawlayout = true;
    }
    if (m_bDrawlayout)
        add_wave_to_chart_RTO(0,
                          "Savina","00",
                          common->rtobservation_reader,
                          m_RTO_chart_list[0],
                          m_rtchart1_wave_list,
                          m_rtchart1_time_list);
>>>>>>> Stashed changes
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
                dds::sub::Query(common->rtobservation_reader, querystr),
                dds::sub::status::DataState(
                dds::sub::status::SampleState::any(),
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
        samples2 = common->rtobservation_reader.select().condition(qcond2).read();
        if(samples2.length() == 0)
            return items;
        for(auto& sample : samples2)
        {
            if(sample.info().valid())
            {   //need "DisplayDesc"
                dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
                std::string mdc_code=data.value<std::string>("mdc_code").c_str();
                //printf("****agooda m_WaveRtItems mdc_code=%s\n", mdc_code.c_str());
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
                printf("***Agooda m_WaveRtItems Reduce\n");
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
                dds::sub::Query(common->observation_reader, querystr),
                dds::sub::status::DataState(
                dds::sub::status::SampleState::any(),
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
        samples2 = common->observation_reader.select().condition(qcond2).read();
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
                printf("***Agooda m_WaveObItems Reduce\n");
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
        std::vector<stDisplayItems> items;
        Common* common = Common::instance();
        dds::sub::cond::ReadCondition *item_cond;

        if (common->domain_id == -1)
            return false;
        //printf("***Agooda QueryDisplayItems");
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples2;
        item_cond = new dds::sub::cond::ReadCondition(
                    common->m_DisplayItem_reader,
                    dds::sub::status::DataState(
                    dds::sub::status::SampleState::any(),
                    dds::sub::status::ViewState::any(),
                    dds::sub::status::InstanceState::alive()));
        samples2 = common->m_DisplayItem_reader.select().condition(*item_cond).read();
        printf("***agooda QueryDisplayItems length()=%d\n", samples2.length());
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
                item.y_max = data.value<int>("y_max");
                item.y_min = data.value<int>("y_max");
                item.y_step = data.value<int>("y_max");
                items.push_back(item);
            }
        }
        if (items.size() > m_DisplayItems.size())
            m_DisplayItems = items;
        delete item_cond;
        return true;
    }
    catch(...)
    {
        printf("mc_wavepanel::QueryDisplayItems exception\n");
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
        printf("***Agooda GetDisplayIntersec model=%s, type=%s, sour size=%d all size=%d\n", model.c_str(), type.c_str(), source->size(), m_DisplayItems.size());
        for (int i = 0; i < (int)m_DisplayItems.size();i++)
        {
            for(auto& sample : *source)
            {
                if (sample == m_DisplayItems[i].mdc_code &&
                    model == m_DisplayItems[i].model &&
                    type == m_DisplayItems[i].wave_type)
                {
                    items.push_back(m_DisplayItems[i]);
                    printf("***Agooda GetDisplayIntersec %s\n", m_DisplayItems[i].display_desc.c_str());
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
void mc_wavepanel::add_wave_to_chart_RTO(int series_index, std::string model, std::string code,
                                                             dds::sub::DataReader<dds::core::xtypes::DynamicData> reader,
                                                             mc_chart* chart,
                                                             QList<std::vector<float>> &wave_list,
                                                             QList<uint64_t> &time_list)
{
    Common* common = Common::instance();
    if(common->patient_id.size() == 0)
        return;
    int line_break_delta;
    line_break_delta=3000;
    chart->set_line_break_delta(3000);
    std::string querystr = "vmd_id MATCH '";
    querystr.append(common->vmd_id);
    querystr.append("' AND patient_id MATCH '");
    querystr.append(common->patient_id);
    querystr.append("' AND model MATCH '");
    querystr.append(model);
    querystr.append("' AND code MATCH '");
    querystr.append(code);
    querystr.append("'");
    dds::sub::cond::QueryCondition qcond(
                dds::sub::Query(reader, querystr),
                dds::sub::status::DataState(
                dds::sub::status::SampleState::any(),
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
    dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples2 = reader.select().condition(qcond).read();

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
            if(left_over_rtchart_vals.size() > 0)
            {
                if(t-last_rtchart_time < line_break_delta)
                {
                    double delta = (t-last_rtchart_time)/((double)left_over_rtchart_vals.size()+1);
                    for(int i=0;i<(int)left_over_rtchart_vals.size();i++)
                    {
                        chart->add_point(series_index, last_rtchart_time+delta*(i+1), left_over_rtchart_vals[i]);
                    }
                }
                left_over_rtchart_vals.clear();
            }
            if(t > chart->get_view_range_max_x())
            {
                chart->set_view_range_max_x(t);
                chart->set_view_range_min_x(t-0.5*60*1000);
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
