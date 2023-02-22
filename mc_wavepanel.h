#ifndef MC_WAVEPANEL_H
#define MC_WAVEPANEL_H

#include "Dialog_wave_selection.h"
#include "mc_btn_Clickable.h"
#include "qboxlayout.h"
#include "qlabel.h"
#include <QWidget>
#include <stdint.h>
#include <QTimer>
#include "mc_wavepanel_add.h"
#include "dds/core/corefwd.hpp"
#include "dds/sub/TDataReader.hpp"

#define MAX_WAVE 6

struct stDisplayItems
{
    std::string model;
    std::string display_desc;
    std::string wave_type;
    std::string mdc_code;
    int y_max;
    int y_min;
    int y_step;
};

class mc_wavepanel : public QWidget
{
    Q_OBJECT
public:
    explicit mc_wavepanel(QWidget *parent = nullptr);
    void add_clicked();
    std::vector<std::string> QueryRtItems(std::string DeviceName);
    std::vector<std::string> QueryObItems(std::string DeviceName);
    bool QueryDisplayItems(void);
    QTimer m_Timer;
    std::vector<std::string> m_WaveRtItems;
    std::vector<std::string> m_WaveObItems;
    std::vector<stDisplayItems> m_DisplayItems;
    std::vector<stDisplayItems> GetDisplayIntersec(std::string model, std::string type);
    std::string m_DeviceName;
    int m_RtLowerCount;
    int m_ObLowerCount;
    mc_wavepanel_add *m_WaveItem[MAX_WAVE];
    QList<QHBoxLayout*> m_RTO_wave_list;
    QList<mc_chart*> m_RTO_chart_list;
    QList<QWidget*> m_RTO_option_list;

    QList<std::vector<float>> m_rtchart1_wave_list;
    QList<uint64_t> m_rtchart1_time_list;

    void add_wave_to_chart_RTO(int series_index, std::string model, std::string code,
                               dds::sub::DataReader<dds::core::xtypes::DynamicData> reader,
                               mc_chart *chart,
                               QList<std::vector<float>> &wave_list,
                               QList<uint64_t> &time_list);
    void set_wave_ui(QList<QHBoxLayout*> wave_list) {m_RTO_wave_list = wave_list;};
    void set_chart_ui(QList<mc_chart*> chart_list) {m_RTO_chart_list = chart_list;};
    void set_option_ui(QList<QWidget*> option_list) {m_RTO_option_list = option_list;};

signals:

private:
    QLabel* corner;
    //QVBoxLayout* verticalLayout;
    //QVBoxLayout* main_layout;
    //QVBoxLayout* item_layout;
    //QHBoxLayout* header_layout;
    //QWidget* header_bar;
    mc_btn_Clickable* controls_btn;
    uint8_t controls_on;
    Dialog_wave_selection menu;
    bool m_bDrawlayout;
    void render_controls_btn();
    void push_add_item();

private slots:
    void controls_clicked();
    void UpdateWave();
};

#endif // MC_WAVEPANEL_H
