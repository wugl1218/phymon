#ifndef TAB_OBSERVATIONS_MAINPAGE_WIDGET_H
#define TAB_OBSERVATIONS_MAINPAGE_WIDGET_H

#include "dds/core/corefwd.hpp"
#include "dds/sub/TDataReader.hpp"
#include "mc_chart.h"
#include "mc_loop.h"
#include "qgroupbox.h"
#include "qtablewidget.h"
#include "qtimer.h"
#include <QWidget>
#include <QScrollArea>
#include <QAbstractButton>
#include "Dialog_loopselection.h"
#include "Dialog_visualization_config.h"
#include "mc_legend.h"

class mc_loop_entry
{
public:
    float flow;
    float press;
    float vol;
};

namespace Ui {
class Tab_Observations_mainPage_Widget;
}

class Tab_Observations_mainPage_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Tab_Observations_mainPage_Widget(QWidget *parent = nullptr);
    ~Tab_Observations_mainPage_Widget();
    void clear_points();
    Ui::Tab_Observations_mainPage_Widget *ui;
    std::string CapturedIssues_channel_id;
    Dialog_loopselection lsd;
    Dialog_visualization_config vsd;
    std::vector<mc_legend*> legends;
    void mapping_UI_reset();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QTimer update_timer;
    QTimer chart_update_timer;
    uint8_t active;
    QList<std::vector<float>> rtchart1_wave_list;
    QList<std::vector<float>> rtchart2_wave_list;
    QList<uint64_t> rtchart1_time_list;
    QList<uint64_t> rtchart2_time_list;
    QList<QColor> line_color_list;
    int mc_chart1_line =0;
    int mc_chart2_line =0;
    uint64_t last_rtchart1_paw_time;
    std::vector<float> left_over_rtchart1_flow_vals;
    uint64_t last_rtchart1_flow_time;
    std::vector<float> left_over_rtchart2_vals;
    uint64_t last_rtchart2_time;
//    std::vector<float> left_over_rtchart1_paw_vals;
//    uint64_t last_rtchart1_paw_time;
    int loop1_type;
    int loop2_type;
    float last_flow_val;
    float last_loop1_x;
    float last_loop1_y;
    float last_loop2_x;
    float last_loop2_y;
    float max_loop1_x;
    float max_loop1_y;
    float min_loop1_x;
    float min_loop1_y;
    float max_loop2_x;
    float max_loop2_y;
    float min_loop2_x;
    float min_loop2_y;
    std::string loop1_x_label;
    std::string loop1_y_label;
    std::string loop2_x_label;
    std::string loop2_y_label;
    uint8_t set_next_loop_as_ref;
    uint8_t set_next_loop_as_cap;
    uint8_t captured;
    bool is_ref =0;
    bool is_cap =0;
    uint8_t delay_reference;
    uint32_t show_time;
    std::vector<mc_loop_entry> loop_snapshot;
    std::vector<mc_loop_entry> ref_loop_snapshot;
    std::vector<mc_loop_entry> cap_loop_snapshot;
    void set_loop1_type(int type);
    void set_loop2_type(int type);
    void load_loops(mc_loop* loop, int type);
    void set_checked(QWidget* w, uint8_t checked);
    void loop_check_and_expand(int loopnum, float x, float y);
    void loop_check_and_shrink(int loopnum, float x, float y);
    void add_wave_to_chart_Obs(int series_index,
                               uint64_t t,float val,
                               mc_chart *chart,
                               QList<std::vector<float>> &wave_list,
                               QList<uint64_t> &time_list);
    void add_wave_to_chart_RTO(int series_index, std::string model, std::string code,
                               dds::sub::DataReader<dds::core::xtypes::DynamicData> reader,
                               mc_chart *chart,
                               QList<std::vector<float>> &wave_list,
                               QList<uint64_t> &time_list);
private slots:
    void update_triggered();
    void chart_update_triggered();
    void on_series_pressed(std::string name, std::string model, std::string mdccode,
                           std::string y_min, std::string y_max, std::string unit, std::string datasource);
/*
    void on_device1_tableWidget_cellClicked(int row, int column);

    void on_device2_tableWidget_cellClicked(int row, int column);

    void on_device3_tableWidget_cellClicked(int row, int column);

    void on_device4_tableWidget_cellClicked(int row, int column);*/

    void visualizetion_clicked();

    void loop_clicked();

    void on_ref_btn_toggled(bool checked);

    void on_cap_btn_toggled(bool checked);

    void on_visualization_hide_btn_clicked();

    void on_visualization_show_btn_clicked();

    void on_visualization_new_clicked();

    void on_Obs_clicked();
signals:
    void changeToMetricItemsDisplayConfigPage();
    void changeToHistoryPage();
};

#endif // TAB_OBSERVATIONS_MAINPAGE_WIDGET_H
