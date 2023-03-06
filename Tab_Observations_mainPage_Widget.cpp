#include <QScrollBar>
#include <QResizeEvent>
#include <QPointF>
#include <QDateTime>
#include "MainDialog.h"
#include "Tab_Observations_historyPage_Widget.h"
#include "Tab_Observations_mainPage_Widget.h"
#include "ui_Tab_Observations_historyPage_Widget.h"
#include "ui_Tab_Observations_mainPage_Widget.h"
#include "Common.h"

#define Device_TableWidget_WIDTH 320


Tab_Observations_mainPage_Widget::Tab_Observations_mainPage_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab_Observations_mainPage_Widget),
    lsd(this),
    vsd(this),
    update_timer(this),
    chart_update_timer(this)
{
    ui->setupUi(this);
    Common* common = Common::instance();
    common->observation_main_page = this;
    active = 0;
    connect(&update_timer, SIGNAL(timeout()), this, SLOT(update_triggered()));
    update_timer.start(1000);
    connect(&chart_update_timer, SIGNAL(timeout()), this, SLOT(chart_update_triggered()));
    chart_update_timer.start(50);
    set_next_loop_as_ref = 0;
    set_next_loop_as_cap = 0;
    captured = 0;
    delay_reference = 0;

    ui->rt_chart1->set_axis_visible(1);
    ui->rt_chart1->set_selection_width(40);
    ui->rt_chart1->set_selection_type(MC_SELECT_SERIES);
    ui->rt_chart1->set_series_width(0,2);
    ui->rt_chart1->set_scrollable(0);
    ui->rt_chart1->set_zoomable(0);
    ui->rt_chart1->set_view_range_max_y(200);
    ui->rt_chart1->set_view_range_min_y(0);
    ui->rt_chart1->set_num_labels_x(4);
    ui->rt_chart1->set_num_labels_y(5);

    ui->rt_chart2->set_axis_visible(1);
    ui->rt_chart2->set_selection_width(40);
    ui->rt_chart2->set_selection_type(MC_SELECT_SERIES);
    ui->rt_chart2->set_series_width(0,2);
    ui->rt_chart2->set_scrollable(0);
    ui->rt_chart2->set_zoomable(0);
    ui->rt_chart2->set_view_range_max_y(1000);
    ui->rt_chart2->set_view_range_min_y(0);
    ui->rt_chart2->set_num_labels_x(4);
    ui->rt_chart2->set_num_labels_y(5);

/*
    ui->pChart->set_axis_visible(1);
    ui->pChart->set_selection_width(40);
    ui->pChart->set_selection_type(MC_SELECT_SERIES);
    ui->pChart->set_series_width(0,2);
    ui->pChart->set_scrollable(0);
    ui->pChart->set_zoomable(0);
    ui->pChart->set_view_range_max_y(200);
    ui->pChart->set_view_range_min_y(0);
    ui->pChart->set_num_labels_x(5);
    ui->pChart->set_num_labels_y(5);
    ui->pChart->set_series_color(0, QColor(255,255,255));
*/
//    ui->legend->set_text_color(QColor(255,255,255,255));
//    ui->legend->set_text_color(QColor(0,0,0,255));
//    ui->legend->set_vertical_spacing(80);
//    connect(ui->legend, SIGNAL(on_series_select(std::string,std::string)), this, SLOT(on_series_pressed(std::string,std::string)));
    connect(ui->rt_chart1, SIGNAL(clicked()), this, SLOT(on_Obs_clicked()));
    connect(ui->rt_chart2, SIGNAL(clicked()), this, SLOT(on_Obs_clicked()));
    connect(ui->loop1, SIGNAL(on_press()), this, SLOT(loop_clicked()));
    connect(ui->loop2, SIGNAL(on_press()), this, SLOT(loop_clicked()));
    connect(ui->visualizetion, SIGNAL(on_press()), this, SLOT(visualizetion_clicked()));
    ui->visualization_show_btn->hide();
    ui->visualization_label->hide();
    //fordebug
    ui->visualizetion->hide();
    ui->visualization_setting->hide();
    ui->visualization_config_btn->hide();
    ui->visualization_hide_btn->hide();
    ui->visualization_new->hide();
    //set_checked(ui->cap_btn, 0);
    //set_checked(ui->ref_btn, 0);

 /*   for(int i=0;i<20;++i)
    {
        std::vector<float> left_over_rtchart1_paw_vals;
        left_over_rtchart_1_vals<<left_over_rtchart1_paw_vals;
        left_over_rtchart_2_vals<<left_over_rtchart1_paw_vals;
        last_rtchart_1_time<<0;
        last_rtchart_2_time<<0;

    }*/


    set_loop1_type(LOOP_PRESSURE_VOLUME);
    set_loop2_type(LOOP_VOLUME_FLOW);
    last_flow_val = 0.0f;
    last_loop1_x = 0.0f;
    last_loop1_y = 0.0f;
    last_loop2_x = 0.0f;
    last_loop2_y = 0.0f;

    max_loop1_x = 0.0f;
    max_loop1_y = 0.0f;
    min_loop1_x = 0.0f;
    min_loop1_y = 0.0f;
    max_loop2_x = 0.0f;
    max_loop2_y = 0.0f;
    min_loop2_x = 0.0f;
    min_loop2_y = 0.0f;
    for(int t =0;t<21;++t)
        {
        std::vector<float> vals;
        uint64_t time;
        rtchart1_wave_list<<vals;
        rtchart2_wave_list<<vals;
        rtchart1_time_list<<time;
        rtchart2_time_list<<time;
    }
 /*   ui->loop1->hide();
    ui->loop2->hide();
    ui->ref_btn->hide();
    ui->cap_btn->hide();*/
    line_color_list<<QColor(100,200,30)
                   <<QColor(140,240,70)
                   <<QColor(160,255,110)
                   <<QColor(40,150,150)
                   <<QColor(60,170,170)

                   <<QColor(80,190,210)
                   <<QColor(150,50,150)
                   <<QColor(170,70,170)
                   <<QColor(190,90,190)
                   <<QColor(150,150,50)

                   <<QColor(170,170,70)
                   <<QColor(190,190,90)
                   <<QColor(100,150,135)
                   <<QColor(120,170,155)
                   <<QColor(140,190,175)

                   <<QColor(100,45,120)
                   <<QColor(120,65,140)
                   <<QColor(140,85,160)
                   <<QColor(150,50,70)
                   <<QColor(170,70,90)

                   <<QColor(190,90,110);


    SetWavePanelSlots();
}
void Tab_Observations_mainPage_Widget::SetWavePanelSlots()
{
    RTO_wave_list<<ui->wave_1
                 <<ui->wave_2
                 <<ui->wave_3
                 <<ui->wave_4
                 <<ui->wave_5
                 <<ui->wave_6;
    ui->wavePanel->set_wave_ui(RTO_wave_list);
    RTO_chart_list<<ui->chart_1
                 <<ui->chart_2
                 <<ui->chart_3
                 <<ui->chart_4
                 <<ui->chart_5
                 <<ui->chart_6;
    ui->wavePanel->set_chart_ui(RTO_chart_list);
    RTO_option_list<<ui->option_1
                 <<ui->option_2
                 <<ui->option_3
                 <<ui->option_4
                 <<ui->option_5
                 <<ui->option_6;
    ui->wavePanel->set_option_ui(RTO_option_list);
    RTO_minus_list<<ui->minus_1
                 <<ui->minus_2
                 <<ui->minus_3
                 <<ui->minus_4
                 <<ui->minus_5
                 <<ui->minus_6;
    ui->wavePanel->set_minus_ui(RTO_minus_list);
    RTO_name_list<<ui->wave_name_1
                <<ui->wave_name_2
                <<ui->wave_name_3
                <<ui->wave_name_4
                <<ui->wave_name_5
                <<ui->wave_name_6;
    ui->wavePanel->set_name_ui(RTO_name_list);
    RTO_chart_list<<ui->chart_1
                 <<ui->chart_2
                 <<ui->chart_3
                 <<ui->chart_4
                 <<ui->chart_5
                 <<ui->chart_6;
    ui->wavePanel->set_chart_ui(RTO_chart_list);
    RTO_option_list<<ui->option_1
                 <<ui->option_2
                 <<ui->option_3
                 <<ui->option_4
                 <<ui->option_5
                 <<ui->option_6;
    ui->wavePanel->set_option_ui(RTO_option_list);
    RTO_minus_list<<ui->minus_1
                 <<ui->minus_2
                 <<ui->minus_3
                 <<ui->minus_4
                 <<ui->minus_5
                 <<ui->minus_6;
    ui->wavePanel->set_minus_ui(RTO_minus_list);
    RTO_name_list<<ui->wave_name_1
                <<ui->wave_name_2
                <<ui->wave_name_3
                <<ui->wave_name_4
                <<ui->wave_name_5
                <<ui->wave_name_6;
    ui->wavePanel->set_name_ui(RTO_name_list);
    ui->wavePanel->set_frame_ui(ui->item, ui->loop_frame, ui->add_frame);
    connect(ui->add_btn, SIGNAL(clicked()), this, SLOT(on_add_btn_clicked));

    ui->minus_1->setProperty("index", 0);
    connect(ui->minus_1, SIGNAL(clicked()), this, SLOT(on_del_btn_clicked()));
    ui->minus_2->setProperty("index", 1);
    connect(ui->minus_2, SIGNAL(clicked()), this, SLOT(on_del_btn_clicked()));
    ui->minus_3->setProperty("index", 2);
    connect(ui->minus_3, SIGNAL(clicked()), this, SLOT(on_del_btn_clicked()));
    ui->minus_4->setProperty("index", 3);
    connect(ui->minus_4, SIGNAL(clicked()), this, SLOT(on_del_btn_clicked()));
    ui->minus_5->setProperty("index", 4);
    connect(ui->minus_5, SIGNAL(clicked()), this, SLOT(on_del_btn_clicked()));
    ui->minus_6->setProperty("index", 5);
    connect(ui->minus_6, SIGNAL(clicked()), this, SLOT(on_del_btn_clicked()));
    ui->loop_minus_2->setProperty("index", 6);
    connect(ui->loop_minus_2, SIGNAL(clicked()), this, SLOT(on_del_btn_clicked()));
    ui->wavePanel->set_loop_minus(ui->loop_minus_2, ui->add_btn);

    ui->chart_1->setProperty("index", 0);
    connect(ui->chart_1, SIGNAL(clicked()), this, SLOT(on_enlarge_btn_clicked()));
    ui->chart_2->setProperty("index", 1);
    connect(ui->chart_2, SIGNAL(clicked()), this, SLOT(on_enlarge_btn_clicked()));
    ui->chart_3->setProperty("index", 2);
    connect(ui->chart_3, SIGNAL(clicked()), this, SLOT(on_enlarge_btn_clicked()));
    ui->chart_4->setProperty("index", 3);
    connect(ui->chart_4, SIGNAL(clicked()), this, SLOT(on_enlarge_btn_clicked()));
    ui->chart_5->setProperty("index", 4);
    connect(ui->chart_5, SIGNAL(clicked()), this, SLOT(on_enlarge_btn_clicked()));
    ui->chart_6->setProperty("index", 5);
    connect(ui->chart_6, SIGNAL(clicked()), this, SLOT(on_enlarge_btn_clicked()));
  //  connect(ui->setup_btn, SIGNAL(clicked()), this, SLOT(on_setup_btn_clicked()));
}
void Tab_Observations_mainPage_Widget::on_setup_btn_clicked()
{
    std::vector<dbDisplayItems> item = ui->wavePanel->m_nurse_items;
    int j = 0;
    ui->wavePanel->add_frame_control();
    for (int i = 0; i < (int)item.size();i++)
    {
        if (ui->wavePanel->m_setup)
        {
            ui->add_btn->hide();
            if (item[i].display_desc == LOOPS_NAME)
            {
                ui->loop_minus_2->hide();
                continue;
            }
            RTO_minus_list[j]->hide();
        }
        else
        {
            ui->add_btn->show();
            if (item[i].display_desc == LOOPS_NAME)
            {
                ui->loop_minus_2->show();
                continue;
            }
            RTO_minus_list[j]->show();
        }
        j++;
    }
    ui->wavePanel->m_setup = ui->wavePanel->m_setup ? false:true;
}
void Tab_Observations_mainPage_Widget::on_add_btn_clicked()
{
    ui->wavePanel->mc_add_clicked(ui->wavePanel);

}
void Tab_Observations_mainPage_Widget::on_del_btn_clicked()
{
    qDebug()<<"***** del";
    mc_btn_Clickable* b = (mc_btn_Clickable*)sender();
    int index = b->property("index").value<int>();
    ui->wavePanel->mc_del_clicked(index);
}
void Tab_Observations_mainPage_Widget::on_enlarge_btn_clicked()
{

    mc_btn_Clickable* b = (mc_btn_Clickable*)sender();
    int index = b->property("index").value<int>();
    if (index > MAX_WAVE)
        return;
    dbDisplayItems item = ui->wavePanel->m_nurse_items[index];
    std::string min;
    std::string max;
    min = QString::number(item.y_min).toStdString();
    max = QString::number(item.y_max).toStdString();
    on_series_pressed(item.display_desc,
                      item.model,
                      item.mdc_code,
                      min,
                      max,
                      "",
                      "RTObservation");
}
void Tab_Observations_mainPage_Widget::visualizetion_clicked()
{
    Common* common = Common::instance();

}

void Tab_Observations_mainPage_Widget::loop_clicked()
{
    Common* common = Common::instance();
    lsd.set_current_loops(loop1_type, loop2_type);
    auto hostRect = this->geometry();
    lsd.move(hostRect.center() - lsd.rect().center());
/*    if(is_ref)
        ui->ref_btn->setStyleSheet(common->css.Enabled_Checked_ButtonStyle);
    else
        ui->ref_btn->setStyleSheet(common->css.Enabled_unChecked_ButtonStyle);
    if(is_cap)
        ui->cap_btn->setStyleSheet(common->css.Enabled_Checked_ButtonStyle);
    else
        ui->cap_btn->setStyleSheet(common->css.Enabled_unChecked_ButtonStyle);
    lsd.exec();
    if (is_cap)
    {
        ui->cap_btn->setStyleSheet(common->css.Checked_ButtonStyle);
        if(is_ref)
            ui->ref_btn->setStyleSheet(common->css.Enabled_Checked_ButtonStyle);
        else
            ui->ref_btn->setStyleSheet(common->css.Enabled_unChecked_ButtonStyle);
    }
    else
    {
        ui->cap_btn->setStyleSheet(common->css.unChecked_ButtonStyle);
        if(is_ref)
            ui->ref_btn->setStyleSheet(common->css.Checked_ButtonStyle);
        else
            ui->ref_btn->setStyleSheet(common->css.unChecked_ButtonStyle);
    }*/
    int l1;
    int l2;
    lsd.get_current_loops(&l1, &l2);
    set_loop1_type(l1);
    set_loop2_type(l2);
}

void Tab_Observations_mainPage_Widget::load_loops(mc_loop* loop, int type)
{
    if(is_ref && ref_loop_snapshot.size() > 0)
    {
        if(type == LOOP_PRESSURE_VOLUME)
        {
            for(int i=0;i<(int)ref_loop_snapshot.size();i++)
                loop->add_point(ref_loop_snapshot[i].press, ref_loop_snapshot[i].vol);
        }
        else if(type == LOOP_VOLUME_FLOW)
        {
            for(int i=0;i<(int)ref_loop_snapshot.size();i++)
                loop->add_point(ref_loop_snapshot[i].vol, ref_loop_snapshot[i].flow);
        }
        else if(type == LOOP_FLOW_PRESSURE)
        {
            for(int i=0;i<(int)ref_loop_snapshot.size();i++)
                loop->add_point(ref_loop_snapshot[i].flow, ref_loop_snapshot[i].press);
        }
        else
            return;
        loop->save_points_as_ref();
        loop->clear_points();
    }
    if(is_cap && cap_loop_snapshot.size() > 0)
    {
        if(type == LOOP_PRESSURE_VOLUME)
        {
            for(int i=0;i<(int)cap_loop_snapshot.size();i++)
                loop->add_point(cap_loop_snapshot[i].press, cap_loop_snapshot[i].vol);
        }
        else if(type == LOOP_VOLUME_FLOW)
        {
            for(int i=0;i<(int)cap_loop_snapshot.size();i++)
                loop->add_point(cap_loop_snapshot[i].vol, cap_loop_snapshot[i].flow);
        }
        else if(type == LOOP_FLOW_PRESSURE)
        {
            for(int i=0;i<(int)cap_loop_snapshot.size();i++)
                loop->add_point(cap_loop_snapshot[i].flow, cap_loop_snapshot[i].press);
        }
        else
            return;
    }
}

void Tab_Observations_mainPage_Widget::set_loop1_type(int type)
{
    loop1_type = type;
    if(type == LOOP_PRESSURE_VOLUME)
    {
        ui->loop1->set_view_range_max_x(30);
        ui->loop1->set_view_range_min_x(-5);
        ui->loop1->set_view_range_max_y(1000);
        ui->loop1->set_view_range_min_y(0);
        ui->loop1->set_x_label("Paw cmH2O");
        ui->loop1->set_y_label("Volume mL");
        loop1_x_label=("MDC_PRESS_AWAY");
        loop1_y_label=("FOYA_MEASURED_VolumeInspirationBegan");
    }
    else if(type == LOOP_VOLUME_FLOW)
    {
        ui->loop1->set_view_range_max_x(1000);
        ui->loop1->set_view_range_min_x(0);
        ui->loop1->set_view_range_max_y(100);
        ui->loop1->set_view_range_min_y(-100);
        ui->loop1->set_x_label("Volume mL");
        ui->loop1->set_y_label("Flow L/min");
        loop1_x_label=("FOYA_MEASURED_VolumeInspirationBegan");
        loop1_y_label=("MDC_Flow_AWAY");
    }
    else if(type == LOOP_FLOW_PRESSURE)
    {
        ui->loop1->set_view_range_max_x(100);
        ui->loop1->set_view_range_min_x(-100);
        ui->loop1->set_view_range_max_y(30.0);
        ui->loop1->set_view_range_min_y(-5);
        ui->loop1->set_x_label("Flow L/min");
        ui->loop1->set_y_label("Paw cmH2O");
        loop1_x_label=("MDC_Flow_AWAY");
        loop1_y_label=("MDC_PRESS_AWAY");
    }
    else if(type == LOOP_PTRACH_VOLUME)
    {
        ui->loop1->set_view_range_max_x(30);
        ui->loop1->set_view_range_min_x(-5);
        ui->loop1->set_view_range_max_y(1000);
        ui->loop1->set_view_range_min_y(0);
        ui->loop1->set_x_label("Ptrach cmH2O");
        ui->loop1->set_y_label("Volume mL");
        loop1_x_label=("Ptrach");
        loop1_y_label=("FOYA_MEASURED_VolumeInspirationBegan");
    }
    else
    {
        ui->loop1->set_view_range_max_x(100);
        ui->loop1->set_view_range_min_x(-100);
        ui->loop1->set_view_range_max_y(30);
        ui->loop1->set_view_range_min_y(-5);
        ui->loop1->set_x_label("Flow L/min");
        ui->loop1->set_y_label("Ptrach cmH2O");
        loop1_x_label=("MDC_Flow_AWAY ");
        loop1_y_label=("Ptrach");
    }
    ui->loop1->clear_points();
    ui->loop1->clear_ref_points();
    load_loops(ui->loop1, type);
}

void Tab_Observations_mainPage_Widget::set_loop2_type(int type)
{
    loop2_type = type;
    if(type == LOOP_PRESSURE_VOLUME)
    {
        ui->loop2->set_view_range_max_x(30);
        ui->loop2->set_view_range_min_x(-5);
        ui->loop2->set_view_range_max_y(1000);
        ui->loop2->set_view_range_min_y(0);
        ui->loop2->set_x_label("Paw cmH2O");
        ui->loop2->set_y_label("Volume mL");
        loop2_x_label=("MDC_PRESS_AWAY ");
        loop2_y_label=("FOYA_MEASURED_VolumeInspirationBegan");
    }
    else if(type == LOOP_VOLUME_FLOW)
    {
        ui->loop2->set_view_range_max_x(1000);
        ui->loop2->set_view_range_min_x(0);
        ui->loop2->set_view_range_max_y(100);
        ui->loop2->set_view_range_min_y(-100);
        ui->loop2->set_x_label("Volume mL");
        ui->loop2->set_y_label("Flow L/min");
        loop2_x_label=("MDC_Flow_AWAY ");
        loop2_y_label=("FOYA_MEASURED_VolumeInspirationBegan");
    }
    else if(type == LOOP_FLOW_PRESSURE)
    {
        ui->loop2->set_view_range_max_x(100);
        ui->loop2->set_view_range_min_x(-100);
        ui->loop2->set_view_range_max_y(30.0);
        ui->loop2->set_view_range_min_y(-5);
        ui->loop2->set_x_label("Flow L/min");
        ui->loop2->set_y_label("Paw cmH2O");
        loop2_x_label=("MDC_Flow_AWAY ");
        loop2_y_label=("MDC_PRESS_AWAY");
    }
    else if(type == LOOP_PTRACH_VOLUME)
    {
        ui->loop2->set_view_range_max_x(30);
        ui->loop2->set_view_range_min_x(-5);
        ui->loop2->set_view_range_max_y(1000);
        ui->loop2->set_view_range_min_y(0);
        ui->loop2->set_x_label("Ptrach cmH2O");
        ui->loop2->set_y_label("Volume mL");
        loop2_x_label=("Ptrach ");
        loop2_y_label=("FOYA_MEASURED_VolumeInspirationBegan");
    }
    else
    {
        ui->loop2->set_view_range_max_x(100);
        ui->loop2->set_view_range_min_x(-100);
        ui->loop2->set_view_range_max_y(30);
        ui->loop2->set_view_range_min_y(-5);
        ui->loop2->set_x_label("Flow L/min");
        ui->loop2->set_y_label("Ptrach cmH2O");
        loop2_x_label=("MDC_Flow_AWAY ");
        loop2_y_label=("Ptrach");
    }
    ui->loop2->clear_points();
    ui->loop2->clear_ref_points();
    load_loops(ui->loop2, type);
}
void Tab_Observations_mainPage_Widget::clear_points()
{
    for(int i =0;i<mc_chart1_line;++i)
        ui->rt_chart1->clear_points(i);
    for(int i =0;i<mc_chart2_line;++i)
        ui->rt_chart2->clear_points(i);

    for(int t =0;t<21;++t)
        {
        std::vector<float> vals;
        uint64_t time;
        rtchart1_wave_list[t].clear();
        rtchart2_wave_list[t].clear();
    }
    ui->loop1->clear_points();
    ui->loop1->clear_ref_points();
    ui->loop2->clear_points();
    ui->loop2->clear_ref_points();
/*    on_ref_btn_toggled(false);
    ui->ref_btn->setChecked(false);
    on_cap_btn_toggled(false);
    ui->cap_btn->setChecked(false); */

}

void Tab_Observations_mainPage_Widget::on_series_pressed(std::string name,std::string model, std::string mdccode,
                                                         std::string y_min,std::string y_max,std::string unit,
                                                         std::string datasource)
{
    Common* common = Common::instance();
    auto fit = common->md->dm.devices.find(model);
    if(fit == common->md->dm.devices.end())return;
    if(mdccode=="")return;

    common->history_page->set_title_text(mdccode,
                                         model,
                                         name,
                                         y_min,
                                         y_max,
                                         unit,
                                         datasource);
    emit changeToHistoryPage();
    //    for(int i=0;i<(int)legends.size();i++)
    //        if( legends[i] == sender() )
    //        {
    //            common->history_datasource = "Observation";
    //        }
/*    if(sender() == ui->legend)
    {
        if(model == 0)
        {
            common->history_mdccode = "MDC_PRESS_AWAY";
            emit changeToHistoryPage();
        }
        else if(model == 1)
        {
            common->history_mdccode = "MDC_FLOW_AWAY";
            emit changeToHistoryPage();
        }
        else if(model == 2)
        {
            common->history_mdccode = "FOYA_MEASURED_VolumeInspirationBegan";
            emit changeToHistoryPage();
        }
    }
    else if(sender() == ui->rt_chart1)
    {
        if(model == 0)
        {
            common->history_mdccode = "MDC_PRESS_AWAY";
            emit changeToHistoryPage();
        }
        else if(model == 1)
        {
            common->history_mdccode = "MDC_FLOW_AWAY";
            emit changeToHistoryPage();
        }
    }
    else if(sender() == ui->rt_chart2)
    {
        if(model == 0)
        {
            common->history_mdccode = "FOYA_MEASURED_VolumeInspirationBegan";
            emit changeToHistoryPage();
        }
    }*/
}

Tab_Observations_mainPage_Widget::~Tab_Observations_mainPage_Widget()
{
    delete ui;
}

void Tab_Observations_mainPage_Widget::chart_update_triggered()
{
    Common* common = Common::instance();
    if(common->patient_id.size() == 0)
        return;
    std::string model;
    auto fit = common->md->dm.devices.find("Savina");
    if(fit == common->md->dm.devices.end())
    {
        fit = common->md->dm.devices.find("Savina 300");
        if(fit == common->md->dm.devices.end())
            return;
    }
    CapturedIssues_channel_id=fit->second.channel_id;
    model = fit->first;
    int loop_start = -1;
    std::vector<float> loop1_x;
    std::vector<float> loop1_y;
    std::vector<float> loop2_x;
    std::vector<float> loop2_y;
    std::vector<mc_loop_entry> next_loop_snapshot;
    int snap_start = 0;

    std::string querystr = "vmd_id MATCH '";
    querystr.append(common->vmd_id);
    querystr.append("' AND patient_id MATCH '");
    querystr.append(common->patient_id);
    querystr.append("' AND model MATCH '");
    querystr.append(model);
    querystr.append("' AND mdc_code MATCH 'MDC_FLOW_AWAY'");
    dds::sub::cond::QueryCondition qcond2(
                dds::sub::Query(common->rtobservation_reader, querystr),
                dds::sub::status::DataState(
                dds::sub::status::SampleState::any(),
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
    dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples2 = common->rtobservation_reader.select().condition(qcond2).take();
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
            std::vector<float> vals;
            data.get_values("values", vals);
            for(int i=0;i<(int)vals.size();i++)
            {
                if(last_flow_val <= 0.0f && vals[i] > 0.0f)
                    loop_start = i;
                last_flow_val = vals[i];
            }
            if(loop_start != -1)
            {
                snap_start = loop_snapshot.size();
                for(int i=0;i<loop_start;i++)
                {
                    mc_loop_entry e;
                    e.flow = vals[i];
                    e.press = 0.0f;
                    e.vol = 0.0f;
                    loop_snapshot.push_back(e);
                }
                for(int i=loop_start;i<(int)vals.size();i++)
                {
                    mc_loop_entry e;
                    e.flow = vals[i];
                    e.press = 0.0f;
                    e.vol = 0.0f;
                    next_loop_snapshot.push_back(e);
                }
            }
            else
            {
                snap_start = loop_snapshot.size();
                for(int i=0;i<(int)vals.size();i++)
                {
                    mc_loop_entry e;
                    e.flow = vals[i];
                    e.press = 0.0f;
                    e.vol = 0.0f;
                    loop_snapshot.push_back(e);
                }
            }
            if(loop1_type == LOOP_VOLUME_FLOW)
                loop1_y = vals;
            else if(loop1_type == LOOP_FLOW_PRESSURE)
                loop1_x = vals;
            if(loop2_type == LOOP_VOLUME_FLOW)
                loop2_y = vals;
            else if(loop2_type == LOOP_FLOW_PRESSURE)
                loop2_x = vals;
        }
    }

    querystr = "vmd_id MATCH '";
    querystr.append(common->vmd_id);
    querystr.append("' AND patient_id MATCH '");
    querystr.append(common->patient_id);
    querystr.append("' AND model MATCH '");
    querystr.append(model);
    querystr.append("' AND mdc_code MATCH 'MDC_PRESS_AWAY'");
    dds::sub::cond::QueryCondition qcond(
                dds::sub::Query(common->rtobservation_reader, querystr),
                dds::sub::status::DataState(
                dds::sub::status::SampleState::any(),
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
    dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples = common->rtobservation_reader.select().condition(qcond).take();
    for(auto& sample : samples)
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
            std::vector<float> vals;
            data.get_values("values", vals);
            if(loop_start != -1)
            {
                if(loop_start < (int)vals.size())
                {
                    if((int)loop_snapshot.size() - snap_start >= loop_start)
                    {
                        for(int i=0;i<loop_start;i++)
                        {
                            loop_snapshot[i+snap_start].press = vals[i];
                        }
                    }
                    if((int)next_loop_snapshot.size() >= (int)vals.size() - loop_start)
                    {
                        for(int i=loop_start;i<(int)vals.size();i++)
                        {
                            next_loop_snapshot[i-loop_start].press = vals[i];
                        }
                    }
                }
            }
            else
            {
                if((int)loop_snapshot.size() - snap_start >= (int)vals.size())
                {
                    for(int i=0;i<(int)vals.size();i++)
                    {
                        loop_snapshot[i+snap_start].press = vals[i];
                    }
                }
            }
            if(loop1_type == LOOP_PRESSURE_VOLUME)
                loop1_x = vals;
            else if(loop1_type == LOOP_FLOW_PRESSURE)
                loop1_y = vals;
            if(loop2_type == LOOP_PRESSURE_VOLUME)
                loop2_x = vals;
            else if(loop2_type == LOOP_FLOW_PRESSURE)
                loop2_y = vals;

        }
    }

    querystr = "vmd_id MATCH '";
    querystr.append(common->vmd_id);
    querystr.append("' AND patient_id MATCH '");
    querystr.append(common->patient_id);
    querystr.append("' AND model MATCH '");
    querystr.append(model);
    querystr.append("' AND mdc_code MATCH 'FOYA_MEASURED_VolumeInspirationBegan'");
    dds::sub::cond::QueryCondition qcond3(
                dds::sub::Query(common->rtobservation_reader, querystr),
                dds::sub::status::DataState(
                dds::sub::status::SampleState::any(),
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
    dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples3 = common->rtobservation_reader.select().condition(qcond3).take();
    for(auto& sample : samples3)
    {
        if(sample.info().valid())
        {
            dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
            rti::core::xtypes::LoanedDynamicData loaned_member = data.loan_value("source_timestamp");
            int32_t sec = loaned_member.get().value<int32_t>("sec");
            loaned_member.return_loan();
            time_t now = time(NULL);
            now -= 3;
            if(sec < now)
                continue;
            std::vector<float> vals;
            data.get_values("values", vals);
            if(loop_start != -1)
            {
                if(loop_start < (int)vals.size())
                {
                    if((int)loop_snapshot.size() - snap_start >= loop_start)
                    {
                        for(int i=0;i<loop_start;i++)
                        {
                            loop_snapshot[i+snap_start].vol = vals[i];
                        }
                    }
                    if((int)next_loop_snapshot.size() >= (int)vals.size() - loop_start)
                    {
                        for(int i=loop_start;i<(int)vals.size();i++)
                        {
                            next_loop_snapshot[i-loop_start].vol = vals[i];
                        }
                    }
                }
            }
            else
            {
                if((int)loop_snapshot.size() - snap_start >= (int)vals.size())
                {
                    for(int i=0;i<(int)vals.size();i++)
                    {
                        loop_snapshot[i+snap_start].vol = vals[i];
                    }
                }
            }
            if(loop1_type == LOOP_PRESSURE_VOLUME)
                loop1_y = vals;
            else if(loop1_type == LOOP_VOLUME_FLOW)
                loop1_x = vals;
            if(loop2_type == LOOP_PRESSURE_VOLUME)
                loop2_y = vals;
            else if(loop2_type == LOOP_VOLUME_FLOW)
                loop2_x = vals;

        }
    }

    if(captured)
        return;
    if(loop1_x.size() > 0 && loop1_y.size() > 0 && loop1_type != LOOP_PTRACH_VOLUME && loop1_type != LOOP_FLOW_PTRACH)
    {
        if(loop_start == -1)
        {
            for(int i=0;i<(int)loop1_x.size();i++)
            {
                ui->loop1->add_point(loop1_x[i], loop1_y[i]);
                loop_check_and_expand(1, loop1_x[i], loop1_y[i]);
                if(loop1_x[i] > max_loop1_x)
                    max_loop1_x = loop1_x[i];
                if(loop1_y[i] > max_loop1_y)
                    max_loop1_y = loop1_y[i];
                if(loop1_x[i] < min_loop1_x)
                    min_loop1_x = loop1_x[i];
                if(loop1_y[i] < min_loop1_y)
                    min_loop1_y = loop1_y[i];
            }
        }
        else
        {
            if(set_next_loop_as_ref || set_next_loop_as_cap)
            {
                for(int k=0;k<loop_start;k++)
                {
                    ui->loop1->add_point(loop1_x[k], loop1_y[k]);
                    loop_check_and_expand(1, loop1_x[k], loop1_y[k]);
                    if(loop1_x[k] > max_loop1_x)
                        max_loop1_x = loop1_x[k];
                    if(loop1_y[k] > max_loop1_y)
                        max_loop1_y = loop1_y[k];
                    if(loop1_x[k] < min_loop1_x)
                        min_loop1_x = loop1_x[k];
                    if(loop1_y[k] < min_loop1_y)
                        min_loop1_y = loop1_y[k];
                }
                uint32_t now = Common::get_time_ms();
                uint32_t elapsed = Common::get_elapsed_time(now, show_time);
                if(set_next_loop_as_ref && elapsed > 2000 && !delay_reference)
                {
                    ui->loop1->save_points_as_ref();
                }
                if(set_next_loop_as_cap)
                {
                    goto prepare_loop2;
                }
            }
            loop_check_and_shrink(1, max_loop1_x, max_loop1_y);

            max_loop1_x = 0.0f;
            max_loop1_y = 0.0f;
            min_loop1_x = 0.0f;
            min_loop1_y = 0.0f;
            ui->loop1->clear_points();
            int i=loop_start;
            ui->loop1->add_point(last_loop1_x, last_loop1_y);
            loop_check_and_expand(1, last_loop1_x, last_loop1_y);
            for(;i<(int)loop1_x.size();i++)
            {
                ui->loop1->add_point(loop1_x[i], loop1_y[i]);
                loop_check_and_expand(1, loop1_x[i], loop1_y[i]);
                if(loop1_x[i] > max_loop1_x)
                    max_loop1_x = loop1_x[i];
                if(loop1_y[i] > max_loop1_y)
                    max_loop1_y = loop1_y[i];
                if(loop1_x[i] < min_loop1_x)
                    min_loop1_x = loop1_x[i];
                if(loop1_y[i] < min_loop1_y)
                    min_loop1_y = loop1_y[i];
            }
        }
        last_loop1_x = loop1_x[loop1_x.size()-1];
        last_loop1_y = loop1_y[loop1_y.size()-1];
    }

    prepare_loop2:
    if(loop2_x.size() > 0 && loop2_y.size() > 0 && loop2_type != LOOP_PTRACH_VOLUME && loop2_type != LOOP_FLOW_PTRACH)
    {
        if(loop_start == -1)
        {
            for(int i=0;i<(int)loop2_x.size();i++)
            {
                ui->loop2->add_point(loop2_x[i], loop2_y[i]);
                loop_check_and_expand(2, loop2_x[i], loop2_y[i]);
                if(loop2_x[i] > max_loop2_x)
                    max_loop2_x = loop2_x[i];
                if(loop2_y[i] > max_loop2_y)
                    max_loop2_y = loop2_y[i];
                if(loop2_x[i] < min_loop2_x)
                    min_loop2_x = loop2_x[i];
                if(loop2_y[i] < min_loop2_y)
                    min_loop2_y = loop2_y[i];
            }
        }
        else
        {
            if(set_next_loop_as_ref || set_next_loop_as_cap)
            {
                for(int k=0;k<loop_start;k++)
                {
                    ui->loop2->add_point(loop2_x[k], loop2_y[k]);
                    loop_check_and_expand(2, loop2_x[k], loop2_y[k]);
                    if(loop2_x[k] > max_loop2_x)
                        max_loop2_x = loop2_x[k];
                    if(loop2_y[k] > max_loop2_y)
                        max_loop2_y = loop2_y[k];
                    if(loop2_x[k] < min_loop2_x)
                        min_loop2_x = loop2_x[k];
                    if(loop2_y[k] < min_loop2_y)
                        min_loop2_y = loop2_y[k];
                }
                uint32_t now = Common::get_time_ms();
                uint32_t elapsed = Common::get_elapsed_time(now, show_time);
                if(set_next_loop_as_ref && elapsed > 2000 && !delay_reference)
                    ui->loop2->save_points_as_ref();
                if(set_next_loop_as_cap)
                    goto finish;
            }
            loop_check_and_shrink(2, max_loop2_x, max_loop2_y);

            max_loop2_x = 0.0f;
            max_loop2_y = 0.0f;
            min_loop2_x = 0.0f;
            min_loop2_y = 0.0f;
            ui->loop2->clear_points();
            int i=loop_start;
            ui->loop2->add_point(last_loop2_x, last_loop2_y);
            loop_check_and_expand(2, last_loop2_x, last_loop2_y);
            for(;i<(int)loop2_x.size();i++)
            {
                ui->loop2->add_point(loop2_x[i], loop2_y[i]);
                loop_check_and_expand(2, loop2_x[i], loop2_y[i]);
                if(loop2_x[i] > max_loop2_x)
                    max_loop2_x = loop2_x[i];
                if(loop2_y[i] > max_loop2_y)
                    max_loop2_y = loop2_y[i];
                if(loop2_x[i] < min_loop2_x)
                    min_loop2_x = loop2_x[i];
                if(loop2_y[i] < min_loop2_y)
                    min_loop2_y = loop2_y[i];
            }
        }
        last_loop2_x = loop2_x[loop2_x.size()-1];
        last_loop2_y = loop2_y[loop2_y.size()-1];
    }
finish:
    if(loop_start != -1)
    {
        if(set_next_loop_as_cap)
        {
            captured = 1;
            cap_loop_snapshot = loop_snapshot;
        }
        if(set_next_loop_as_ref)
            ref_loop_snapshot = loop_snapshot;
        uint32_t now = Common::get_time_ms();
        uint32_t elapsed = Common::get_elapsed_time(now, show_time);
        if(elapsed > 2000)
        {
            if(!delay_reference)
                set_next_loop_as_ref = 0;
            else
                delay_reference = 0;
        }
        set_next_loop_as_cap = 0;
        loop_snapshot = next_loop_snapshot;
    }
}

void Tab_Observations_mainPage_Widget::loop_check_and_expand(int loopnum, float x, float y)
{
    if(loopnum == 1)
    {
        if(loop1_type == LOOP_PRESSURE_VOLUME)
        {
            if(ui->loop1->get_view_range_max_x() == 30.0 && x >= ui->loop1->get_view_range_max_x())
                ui->loop1->set_view_range_max_x(60.0);
            if(ui->loop1->get_view_range_max_y() == 1000.0 && y >= ui->loop1->get_view_range_max_y())
                ui->loop1->set_view_range_max_y(2000.0);
            else if(ui->loop1->get_view_range_max_y() == 500.0 && y >= ui->loop1->get_view_range_max_y())
                ui->loop1->set_view_range_max_y(1000.0);
            else if(ui->loop1->get_view_range_max_y() == 250.0 && y >= ui->loop1->get_view_range_max_y())
                ui->loop1->set_view_range_max_y(500.0);
        }
        else if(loop1_type == LOOP_VOLUME_FLOW)
        {
            if(ui->loop1->get_view_range_max_x() == 1000.0 && x >= 1000.0)
                ui->loop1->set_view_range_max_x(2000.0);
            else if(ui->loop1->get_view_range_max_x() == 500.0 && x >= 500.0)
                ui->loop1->set_view_range_max_x(1000.0);
            else if(ui->loop1->get_view_range_max_x() == 250.0 && x >= 250.0)
                ui->loop1->set_view_range_max_x(500.0);
            if(ui->loop1->get_view_range_max_y() == 100.0 && y >= 100.0)
            {
                ui->loop1->set_view_range_max_y(200.0);
                ui->loop1->set_view_range_min_y(-200.0);
            }
            if(ui->loop1->get_view_range_min_y() == -100.0 && y <= -100.0)
            {
                ui->loop1->set_view_range_max_y(200.0);
                ui->loop1->set_view_range_min_y(-200.0);
            }
        }
        else if(loop1_type == LOOP_FLOW_PRESSURE)
        {
            if(ui->loop1->get_view_range_max_x() == 100.0 && x >= 100.0)
            {
                ui->loop1->set_view_range_max_x(200.0);
                ui->loop1->set_view_range_min_x(-200.0);
            }
            if(ui->loop1->get_view_range_min_x() == -100.0 && x <= -100.0)
            {
                ui->loop1->set_view_range_max_x(200.0);
                ui->loop1->set_view_range_min_x(-200.0);
            }
            if(ui->loop1->get_view_range_max_y() == 30.0 && y >= 30.0)
                ui->loop1->set_view_range_max_y(60.0);
        }
    }
    else
    {
        if(loop2_type == LOOP_PRESSURE_VOLUME)
        {
            if(ui->loop2->get_view_range_max_x() == 30.0 && x >= ui->loop2->get_view_range_max_x())
                ui->loop2->set_view_range_max_x(60.0);
            if(ui->loop2->get_view_range_max_y() == 1000.0 && y >= ui->loop2->get_view_range_max_y())
                ui->loop2->set_view_range_max_y(2000.0);
            else if(ui->loop2->get_view_range_max_y() == 500.0 && y >= ui->loop2->get_view_range_max_y())
                ui->loop2->set_view_range_max_y(1000.0);
            else if(ui->loop2->get_view_range_max_y() == 250.0 && y >= ui->loop2->get_view_range_max_y())
                ui->loop2->set_view_range_max_y(500.0);
        }
        else if(loop2_type == LOOP_VOLUME_FLOW)
        {
            if(ui->loop2->get_view_range_max_x() == 1000.0 && x >= 1000.0)
                ui->loop2->set_view_range_max_x(2000.0);
            else if(ui->loop2->get_view_range_max_x() == 500.0 && x >= 500.0)
                ui->loop2->set_view_range_max_x(1000.0);
            else if(ui->loop2->get_view_range_max_x() == 250.0 && x >= 250.0)
                ui->loop2->set_view_range_max_x(500.0);
            if(ui->loop2->get_view_range_max_y() == 100.0 && y >= 100.0)
            {
                ui->loop2->set_view_range_max_y(200.0);
                ui->loop2->set_view_range_min_y(-200.0);
            }
            if(ui->loop2->get_view_range_min_y() == -100.0 && y <= -100.0)
            {
                ui->loop2->set_view_range_max_y(200.0);
                ui->loop2->set_view_range_min_y(-200.0);
            }
        }
        else if(loop2_type == LOOP_FLOW_PRESSURE)
        {
            if(ui->loop2->get_view_range_max_x() == 100.0 && x >= 100.0)
            {
                ui->loop2->set_view_range_max_x(200.0);
                ui->loop2->set_view_range_min_x(-200.0);
            }
            if(ui->loop2->get_view_range_min_x() == -100.0 && x <= -100.0)
            {
                ui->loop2->set_view_range_max_x(200.0);
                ui->loop2->set_view_range_min_x(-200.0);
            }
            if(ui->loop2->get_view_range_max_y() == 30.0 && y >= 30.0)
                ui->loop2->set_view_range_max_y(60.0);
        }
    }
}

void Tab_Observations_mainPage_Widget::loop_check_and_shrink(int loopnum, float x, float y)
{
    if(loopnum == 1)
    {
        if(loop1_type == LOOP_PRESSURE_VOLUME)
        {
            if(ui->loop1->get_view_range_max_x() == 60.0 && x < 30.0)
                ui->loop1->set_view_range_max_x(30.0);
            if(ui->loop1->get_view_range_max_y() == 2000.0 && y < 1000.0)
                ui->loop1->set_view_range_max_y(1000.0);
            //else if(ui->loop1->get_view_range_max_y() == 1000.0 && y < 500.0)
                //ui->loop1->set_view_range_max_y(500.0);
            //else if(ui->loop1->get_view_range_max_y() == 500.0 && y < 250.0)
                //ui->loop1->set_view_range_max_y(250.0);
        }
        else if(loop1_type == LOOP_VOLUME_FLOW)
        {
            if(ui->loop1->get_view_range_max_x() == 2000.0 && x < 1000.0)
                ui->loop1->set_view_range_max_x(1000.0);
            //else if(ui->loop1->get_view_range_max_x() == 1000.0 && x < 500.0)
                //ui->loop1->set_view_range_max_x(500.0);
            //else if(ui->loop1->get_view_range_max_x() == 500.0 && x < 250.0)
                //ui->loop1->set_view_range_max_x(250.0);
            if(ui->loop1->get_view_range_max_y() == 200.0 && y < 100.0)
            {
                ui->loop1->set_view_range_max_y(100.0);
                ui->loop1->set_view_range_min_y(-100.0);
            }
            if(ui->loop1->get_view_range_min_y() == -200.0 && y > -100.0)
            {
                ui->loop1->set_view_range_max_y(100.0);
                ui->loop1->set_view_range_min_y(-100.0);
            }
        }
        else if(loop1_type == LOOP_FLOW_PRESSURE)
        {
            if(ui->loop1->get_view_range_max_x() == 200.0 && x < 100.0)
            {
                ui->loop1->set_view_range_max_x(100.0);
                ui->loop1->set_view_range_min_x(-100.0);
            }
            if(ui->loop1->get_view_range_min_x() == -200.0 && x > -100.0)
            {
                ui->loop1->set_view_range_max_x(100.0);
                ui->loop1->set_view_range_min_x(-100.0);
            }
            if(ui->loop1->get_view_range_max_y() == 60.0 && y < 30.0)
                ui->loop1->set_view_range_max_y(30.0);
        }
    }
    else
    {
        if(loop2_type == LOOP_PRESSURE_VOLUME)
        {
            if(ui->loop2->get_view_range_max_x() == 60.0 && x < 30.0)
                ui->loop2->set_view_range_max_x(30.0);
            if(ui->loop2->get_view_range_max_y() == 2000.0 && y < 1000.0)
                ui->loop2->set_view_range_max_y(1000.0);
            //else if(ui->loop2->get_view_range_max_y() == 1000.0 && y < 500.0)
                //ui->loop2->set_view_range_max_y(500.0);
            //else if(ui->loop2->get_view_range_max_y() == 500.0 && y < 250.0)
                //ui->loop2->set_view_range_max_y(250.0);
        }
        else if(loop2_type == LOOP_VOLUME_FLOW)
        {
            if(ui->loop2->get_view_range_max_x() == 2000.0 && x < 1000.0)
                ui->loop2->set_view_range_max_x(1000.0);
            //else if(ui->loop2->get_view_range_max_x() == 1000.0 && x < 500.0)
                //ui->loop2->set_view_range_max_x(500.0);
            //else if(ui->loop2->get_view_range_max_x() == 500.0 && x < 250.0)
                //ui->loop2->set_view_range_max_x(250.0);
            if(ui->loop2->get_view_range_max_y() == 200.0 && y < 100.0)
            {
                ui->loop2->set_view_range_max_y(100.0);
                ui->loop2->set_view_range_min_y(-100.0);
            }
            if(ui->loop2->get_view_range_min_y() == -200.0 && y > -100.0)
            {
                ui->loop2->set_view_range_max_y(100.0);
                ui->loop2->set_view_range_min_y(-100.0);
            }
        }
        else if(loop2_type == LOOP_FLOW_PRESSURE)
        {
            if(ui->loop2->get_view_range_max_x() == 200.0 && x < 100.0)
            {
                ui->loop2->set_view_range_max_x(100.0);
                ui->loop2->set_view_range_min_x(-100.0);
            }
            if(ui->loop2->get_view_range_min_x() == -200.0 && x > -100.0)
            {
                ui->loop2->set_view_range_max_x(100.0);
                ui->loop2->set_view_range_min_x(-100.0);
            }
            if(ui->loop2->get_view_range_max_y() == 60.0 && y < 30.0)
                ui->loop2->set_view_range_max_y(30.0);
        }
    }
}

void Tab_Observations_mainPage_Widget::set_checked(QWidget* w, uint8_t checked)
{
    QPalette p = w->palette();
    if(checked)
        p.setColor(QPalette::Active, QPalette::Button, QColor(0x0e,0x39,0x6a));
    else
        p.setColor(QPalette::Active, QPalette::Button, QColor(0x34,0x65,0xA4));
    w->setPalette(p);
}

void Tab_Observations_mainPage_Widget::update_triggered()
{
    Common* common = Common::instance();
    if(common->patient_id.size() == 0)
        return;

    //if(common->device_checkstate.size() == 0)
        common->populate_device_checkstate();
    //if(common->item_checkstate.size() == 0)
        common->populate_item_checkstate();
        //對每個已偵測到的Device作處理
    for(auto it=common->md->dm.devices.begin();it!=common->md->dm.devices.end();it++)
    {
        //確認Device已選取
        auto dit = common->device_checkstate.find(it->second.model);
        if(dit != common->device_checkstate.end() && dit->second == 0)
        {
            continue;
        }
        std::string querystr = "vmd_id MATCH '";
        querystr.append(common->vmd_id);
        querystr.append("' AND patient_id MATCH '");
        querystr.append(common->patient_id);
        querystr.append("' AND model MATCH '");
        querystr.append(it->second.model);
        querystr.append("'");
        dds::sub::cond::QueryCondition qcond(
                    dds::sub::Query(common->history_observation_reader, querystr),
                    dds::sub::status::DataState(
                    dds::sub::status::SampleState::any(),
                    dds::sub::status::ViewState::any(),
                    dds::sub::status::InstanceState::alive()));
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples = common->history_observation_reader.select().condition(qcond).read();
        std::multimap<int, mc_entry> entries;
        std::multimap<std::string, mc_entry> left_over;
        for(auto& sample : samples)
        {
            if(sample.info().valid())
            {
                dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
                mc_entry e;
                e.code = data.value<std::string>("code");
                e.mdccode = data.value<std::string>("mdc_code");
                e.model = data.value<std::string>("model");
                e.desc = data.value<std::string>("description");
                e.unit = data.value<std::string>("unit");
                e.val = data.value<float>("value");
                e.abbv = data.value<std::string>("abbreviation");
                std::string vt_desc;

                if(e.model.compare("Savina") == 0)
                    vt_desc = "Tidal volume in mL";
                else
                    vt_desc = "Tidal volume";

                std::string querystr = "mdc_code MATCH '";
                querystr.append(e.mdccode);
                querystr.append("' AND model MATCH '");
                querystr.append(e.model);
                querystr.append("' AND wave_type MATCH 'Obs'");
                dds::sub::cond::QueryCondition qcond(
                            dds::sub::Query(common->m_DisplayItem_reader, querystr),
                            dds::sub::status::DataState(
                            dds::sub::status::SampleState::any(),
                            dds::sub::status::ViewState::any(),
                            dds::sub::status::InstanceState::alive()));
                dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples = common->m_DisplayItem_reader.select().condition(qcond).read();
                for(auto& sample : samples)
                {
                    dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
                    e.y_max = data.value<std::string>("y_min");
                    e.y_max = data.value<std::string>("y_max");
                }
                rti::core::xtypes::LoanedDynamicData loaned_member = data.loan_value("source_timestamp");
                e.ts.tv_sec = loaned_member.get().value<int32_t>("sec");
                e.ts.tv_nsec = loaned_member.get().value<uint32_t>("nanosec");
                loaned_member.return_loan();
                if(common->item_checkstate.size() == 0)
                {
                    auto it2 = common->special_items.find(it->second.model);
                    if(it2 != common->special_items.end())
                    {
                        int i=0;
                        for(;i<(int)it2->second.size();i++)
                        {
                            if(it2->second[i].compare(e.desc) == 0)
                                break;
                        }
                        if(i != (int)it2->second.size())
                        {
                            entries.emplace(i, e);
                        }
                        else
                            left_over.emplace(e.desc, e);
                    }
                }
                else
                {
                    std::string str = it->second.model;
                    str.append(",");
                    str.append(e.desc);
                    bool is_continue =0;
                    for(auto it3=entries.begin();it3!=entries.end();it3++)
                        if(it3->second.desc==e.desc)
                            {
                            is_continue=1;
                            break;
                            }
                    if(is_continue)
                        continue;
                    auto it2 = common->item_checkstate.find(str);
                    if(it2==common->item_checkstate.end())
                    {
                        e.color = it2->second.color;
                        entries.emplace(9999, e);
                    }
                    else if(it2->second.checked)
                    {
                        e.color = it2->second.color;
                        entries.emplace(it2->second.order, e);
                    }
                    else
                        left_over.emplace(e.desc, e);
                }
            }
        }
        if(it->second.model.compare("Savina") == 0 ||
           it->second.model.compare("Savina 300") == 0)
        {
            common->remove_savina_items(&entries);
            common->add_savina_items(it->second.model, &entries, &left_over);
        }
        int row=0;
        mc_chart1_line =0;
        mc_chart2_line =0;
        for(int i=0;i<(int)legends.size();i++)
            {

            delete legends[i];
            }
        legends.clear();
        bool has_matecare_time =0;
        struct timespec matecare_time;
        for(auto it2=entries.begin();it2!=entries.end();)
        {
            char tempbuf[128];
            std::string desc;
            if(it2->second.abbv.size() == 0)
                desc = it2->second.desc;
            else
                desc = it2->second.abbv;
            if(it2->second.desc.compare("I:E Ratio") == 0)
                strcpy(tempbuf, it2->second.code.c_str());
            else if(it2->second.desc.compare("Flow peak") == 0)
            {
                it2->second.unit = "L/min";
                QString str = QString::number((it2->second.val*60.0)/1000.0, 'f', 2);
                it2->second.val = str.toFloat();
            }
            if(!has_matecare_time)
            {
                has_matecare_time=1;
                matecare_time.tv_sec=it2->second.ts.tv_sec;
                matecare_time.tv_nsec=it2->second.ts.tv_nsec;
            }
            if(it2->second.is_matecares &&has_matecare_time)
            {
                it2->second.ts.tv_sec=matecare_time.tv_sec;
                it2->second.ts.tv_nsec=matecare_time.tv_nsec;
            }
            else if (it2->second.is_matecares &&!has_matecare_time)
                continue;
            mc_legend *legend1 = new mc_legend(ui->WidgetContents);
            legend1->setGeometry(0,
                                 10+(8+60)*row,
                                 220, 60);
            legend1->set_square_width(210);
            legend1->set_square_height(60);
            legend1->set_vertical_spacing(8);
            legend1->set_text_color(QColor(0,0,0,255));
            legend1->set_series_color(it2->second.color);
            legend1->set_series_text(desc,
                                     it2->second.model,
                                     it2->second.unit,
                                     it2->second.mdccode,
                                     it2->second.val,
                                     it2->second.y_min,
                                     it2->second.y_max,
                                     "Observation");
            legend1->show();
            connect(legend1, SIGNAL(on_series_select(std::string,std::string,std::string,
                                                     std::string,std::string,std::string,std::string)),
                    this, SLOT(on_series_pressed(std::string,std::string,std::string,
                                                 std::string,std::string,std::string,std::string)));
            legends.push_back(legend1);
            uint64_t t = ((uint64_t)it2->second.ts.tv_sec)*1000 + ((uint64_t)it2->second.ts.tv_nsec)/1000000;
            if(it2->second.y_max=="200")
            {
//                qDebug()<<QString::fromStdString(desc);
//                qDebug()<<it2->second.color;


                ui->rt_chart1->set_series_color(mc_chart1_line, it2->second.color);
                add_wave_to_chart_Obs(mc_chart1_line,
                                  t,it2->second.val,
                                  ui->rt_chart1,
                                  rtchart1_wave_list,
                                  rtchart1_time_list);
                ++mc_chart1_line;
            }
            else if (it2->second.y_max=="1000")
            {
                ui->rt_chart2->set_series_color(mc_chart2_line, it2->second.color);
                add_wave_to_chart_Obs(mc_chart2_line,
                                  t,it2->second.val,
                                  ui->rt_chart2,
                                  rtchart2_wave_list,
                                  rtchart2_time_list);
                ++mc_chart2_line;
            }
            else
                {
                qDebug()<<QString::fromStdString(it2->second.mdccode);
                qDebug()<<QString::fromStdString(it2->second.desc);
                qDebug()<<QString::fromStdString(it2->second.abbv);
                qDebug()<<QString::fromStdString(it2->second.y_max);
                qDebug()<<"y_max error ============================================================";
                }
            row++;
            it2++;
        }
       /* for (int i =0;i<row;++i) //目前顏色尚未使用固定
        {
            if(mc_chart1_line>=i)
            {
                ui->rt_chart1->set_series_color(i, line_color_list[i]);
            }
            else
            {
                ui->rt_chart2->set_series_color(i-mc_chart1_line-1, line_color_list[i]);
            }
        }*/
        ui->WidgetContents->update();
        ui->WidgetContents->setMinimumHeight(10+(60+8)*row);
    }
    ui->rt_chart1->trim_left();
    ui->rt_chart2->trim_left();
}

void Tab_Observations_mainPage_Widget::showEvent(QShowEvent *event)
{
    active = 1;
    delay_reference = 1;
    show_time = Common::get_time_ms();
}

void Tab_Observations_mainPage_Widget::hideEvent(QHideEvent *event)
{
    active = 0;
}

void Tab_Observations_mainPage_Widget::mouseMoveEvent(QMouseEvent *event)
{
    event->setAccepted(false);
}

void Tab_Observations_mainPage_Widget::mousePressEvent(QMouseEvent *event)
{
    /*
    if(childAt(event->pos()) == ui->scrollAreaWidgetContents)
    {
        emit changeToMetricItemsDisplayConfigPage();
    }*/
    event->setAccepted(false);
}

void Tab_Observations_mainPage_Widget::mouseReleaseEvent(QMouseEvent *event)
{
    event->setAccepted(false);
}

void Tab_Observations_mainPage_Widget::on_Obs_clicked()
{
    //color
    emit changeToMetricItemsDisplayConfigPage();
}
void Tab_Observations_mainPage_Widget::on_ref_btn_toggled(bool checked)
{
    Common* common = Common::instance();
    if(checked)
    {
        set_next_loop_as_ref = 1;
        set_checked(ui->ref_btn, 1);
        ui->ref_btn->setStyleSheet(common->css.Checked_ButtonStyle);
        is_ref =1;

    }
    else
    {
        set_checked(ui->ref_btn, 0);
        set_next_loop_as_ref = 0;
        ui->loop1->clear_ref_points();
        ui->loop2->clear_ref_points();
        ui->ref_btn->setStyleSheet(common->css.unChecked_ButtonStyle);
        is_ref =0;
        ref_loop_snapshot.clear();
    }
}


void Tab_Observations_mainPage_Widget::on_cap_btn_toggled(bool checked)
{
    Common* common = Common::instance();
    if(checked)
    {

        set_next_loop_as_cap = 1;
        set_checked(ui->cap_btn, 1);
        ui->ref_btn->setEnabled(false);
        if(is_ref)
            ui->ref_btn->setStyleSheet(common->css.Enabled_Checked_ButtonStyle);
        else
            ui->ref_btn->setStyleSheet(common->css.Enabled_unChecked_ButtonStyle);
        is_cap =1;


        dds::core::xtypes::DynamicData sample(common->CapturedIssues_type);
        sample.value<std::string>("patient_id", common->patient_id);
        sample.value<std::string>("channel_id", CapturedIssues_channel_id);
        rti::core::xtypes::LoanedDynamicData point_member = sample.loan_value("captured_point");
        point_member.get().value("x_mdc_code", loop1_x_label);
        point_member.get().value("y_mdc_code", loop1_y_label);
        point_member.return_loan();
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        rti::core::xtypes::LoanedDynamicData loaned_member = sample.loan_value("captured_timestamp");
        loaned_member.get().value("sec", (int32_t)ts.tv_sec);
        loaned_member.get().value("nanosec", (uint32_t)ts.tv_nsec);
        loaned_member.return_loan();
        common->CapturedIssues_writer.write(sample);

        dds::core::xtypes::DynamicData sample1(common->CapturedIssues_type);
        sample1.value<std::string>("patient_id", common->patient_id);
        sample1.value<std::string>("channel_id", CapturedIssues_channel_id);
        rti::core::xtypes::LoanedDynamicData point_member1 = sample1.loan_value("captured_point");
        point_member1.get().value("x_mdc_code", loop2_x_label);
        point_member1.get().value("y_mdc_code", loop2_y_label);
        point_member1.return_loan();
        rti::core::xtypes::LoanedDynamicData loaned_member1 = sample1.loan_value("captured_timestamp");
        loaned_member1.get().value("sec", (int32_t)ts.tv_sec);
        loaned_member1.get().value("nanosec", (uint32_t)ts.tv_nsec);
        loaned_member1.return_loan();
        common->CapturedIssues_writer1.write(sample1);
    }
    else
    {
        ui->ref_btn->setEnabled(true);
        set_checked(ui->cap_btn, 0);
        set_next_loop_as_cap = 0;
        captured = 0;
        ui->loop1->clear_points();
        ui->loop2->clear_points();
        if(is_ref)
            ui->ref_btn->setStyleSheet(common->css.Checked_ButtonStyle);
        else
            ui->ref_btn->setStyleSheet(common->css.unChecked_ButtonStyle);
        is_cap =0;
        cap_loop_snapshot.clear();
    }
}



void Tab_Observations_mainPage_Widget::on_visualization_hide_btn_clicked()
{
    ui->visualizetion->hide();
    ui->visualization_setting->hide();
    ui->visualization_config_btn->hide();
    ui->visualization_hide_btn->hide();
    ui->visualization_show_btn->show();
    ui->visualization_label->show();
    ui->visualization_new->hide();

}


void Tab_Observations_mainPage_Widget::on_visualization_show_btn_clicked()
{
    ui->visualizetion->show();
    ui->visualization_setting->show();
    ui->visualization_config_btn->show();
    ui->visualization_hide_btn->show();
    ui->visualization_show_btn->hide();
    ui->visualization_label->hide();
    ui->visualization_new->show();
}


void Tab_Observations_mainPage_Widget::on_visualization_new_clicked()
{
    Visualizetion V;
    V.highest=100;
    V.higher=70;
    V.lower=50;
    V.lowest=20;
    V.dataname="SpO2";
    V.device="test";
    ui->visualizetion->visualizetionlist<<V;
    qDebug()<<ui->visualizetion->visualizetionlist.size();
    ui->visualizetion->update();

}
//0215
void Tab_Observations_mainPage_Widget::add_wave_to_chart_Obs(int series_index,
                                                             uint64_t t, float val,
                                                             mc_chart* chart,
                                                             QList<std::vector<float>> &wave_list,
                                                             QList<uint64_t> &time_list)
{
    Common* common = Common::instance();
    if(common->patient_id.size() == 0)
        return;
    int line_break_delta;
    line_break_delta=6000;
    chart->set_line_break_delta(6000);
    time_t now = time(NULL)-6;
    time_t future = time(NULL)+6;
//    if((t < now)||t>future)
//        return;
//    uint64_t t = ((uint64_t)sec)*1000 + ((uint64_t)nsec)/1000000;
    std::vector<float> vals;
    vals.push_back(val);
    auto left_over_rtchart_vals =wave_list[series_index];
    auto last_rtchart_time =time_list[series_index];
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
        chart->set_view_range_min_x(t-15*60*1000);
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
void Tab_Observations_mainPage_Widget::add_wave_to_chart_RTO(int series_index, std::string model, std::string code,
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
void Tab_Observations_mainPage_Widget::mapping_UI_reset()
{
    clear_points();
}
