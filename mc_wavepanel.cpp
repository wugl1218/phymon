#include "mc_wavepanel.h"
#include "mc_wavepanel_add.h"
#include "Common.h"
#include "MainDialog.h"

#define DETECT_USEC     50000
#define DETECT_LOOP     100
#define LOWER_MAX       10
#define DOMAIN_DIR      "../../cbldb/VMD.cbl.cblite2"

mc_wavepanel::mc_wavepanel(QWidget *parent)
    : QWidget{parent},
      menu(parent)
{
    m_DeviceName = "Savina";
    controls_on = 1;
    main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(0,0,0,0);
    main_layout->setSpacing(0);
    header_layout = new QHBoxLayout();
    main_layout->addLayout(header_layout, 0);
    header_layout->setContentsMargins(0,0,0,0);
    header_layout->setSpacing(0);
    corner = new QLabel(this);
    header_layout->addWidget(corner,0);
    corner->setPixmap(QPixmap(":/icons/top_left_corner.png"));
    corner->setGeometry(0,0,42,56);
    header_bar = new QWidget(this);
    header_bar->setStyleSheet("background-color: rgb(19, 105, 205)");
    header_layout->addWidget(header_bar, 1);
    controls_btn = new mc_btn_Clickable(this);
    controls_btn->setPixmap(QPixmap(":/icons/controls_btn1.png"));
    controls_btn->setGeometry(0,0,59,56);
    controls_btn->setStyleSheet("background-color: rgb(19, 105, 205)");
    connect(controls_btn, SIGNAL(clicked()), this, SLOT(controls_clicked()));
    header_layout->addWidget(controls_btn, 0);
    item_layout = new QVBoxLayout();
    main_layout->addLayout(item_layout, 1);
    push_add_item();
//    m_Timer.setInterval(1000);
//    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(UpdateWave()));
//    m_Timer.start();
}

void mc_wavepanel::add_clicked()
{
    menu.clear_tabs();
    std::vector<std::string> items = GetDisplayIntersec("Savina", "TPO");

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
}

static void mc_add_clicked(void* param)
{
    mc_wavepanel* wp = (mc_wavepanel*)param;
    wp->add_clicked();
}

void mc_wavepanel::push_add_item()
{
    mc_wavepanel_add* add_item = new mc_wavepanel_add(this, mc_add_clicked);
    add_item->setStyleSheet("background-color: rgb(9, 58, 115)");
    item_layout->addWidget(add_item, 1);
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
    //QueryRtItems(m_DeviceName);
    //QueryObItems(m_DeviceName);
    //QueryDisplayItems();
    //GetDisplayIntersec("Savina", "Obs");
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
        samples2 = common->rtobservation_reader.select().condition(qcond2).take();
        if(samples2.length() == 0)
            return items;
        for(auto& sample : samples2)
        {
            if(sample.info().valid())
            {   //need "DisplayDesc"
                dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
                std::string mdc_code=data.value<std::string>("mdc_code").c_str();
                printf("****agooda m_WaveRtItems mdc_code=%s\n", mdc_code.c_str());
                items.push_back(mdc_code);
            }
        }
        if (items.size() > m_WaveRtItems.size())
            m_WaveRtItems = items;
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
        samples2 = common->observation_reader.select().condition(qcond2).take();
        if(samples2.length() == 0)
            return items;
        for(auto& sample : samples2)
        {
            if(sample.info().valid())
            {   //need "DisplayDesc"
                dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
                std::string mdc_code=data.value<std::string>("mdc_code").c_str();
                printf("****agooda m_WaveObItems mdc_code=%s\n", mdc_code.c_str());
                items.push_back(mdc_code);
            }
        }
        if (items.size() > m_WaveObItems.size())
            m_WaveObItems = items;
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
        printf("***Agooda QueryDisplayItems");
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples2;
        item_cond = new dds::sub::cond::ReadCondition(
                    common->m_DisplayItem_reader,
                    dds::sub::status::DataState(
                    dds::sub::status::SampleState::any(),
                    dds::sub::status::ViewState::any(),
                    dds::sub::status::InstanceState::alive()));
        samples2 = common->m_DisplayItem_reader.select().condition(*item_cond).read();
        printf("***agooda QueryDisplayItems samples2.length()=%d\n", samples2.length());
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
                items.push_back(item);
            }
        }
        if (items.size() > m_DisplayItems.size())
            m_DisplayItems = items;
        return true;
    }
    catch(...)
    {
        printf("mc_wavepanel::QueryDisplayItems exception\n");
    }
}

std::vector<std::string> mc_wavepanel::GetDisplayIntersec(std::string model, std::string type)
{
    std::vector<std::string> items;
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
                    items.push_back(m_DisplayItems[i].display_desc);
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
