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
#include <QLabel>

#define MAX_WAVE 6

class mc_wavepanel : public QWidget
{
    Q_OBJECT
public:
    explicit mc_wavepanel(QWidget *parent = nullptr);
    void add_clicked();
    std::vector<std::string> QueryRtItems(std::string DeviceName);
    std::vector<std::string> QueryObItems(std::string DeviceName);
    bool QueryDisplayItems(void);
    void WriteNurseDB(stDisplayItems item, bool bDel = false);
    QTimer m_Timer;
    std::vector<std::string> m_WaveRtItems;
    std::vector<std::string> m_WaveObItems;
    std::vector<stDisplayItems> m_DisplayItems;
    QVBoxLayout *m_main_item;
    QFrame *m_loop_frame;
    QFrame *m_add_frame;
    std::vector<stDisplayItems> GetDisplayIntersec(std::string model, std::string type);
    std::string m_DeviceName;
    int m_RtLowerCount;
    int m_ObLowerCount;
    mc_wavepanel_add *m_WaveItem[MAX_WAVE];
    QList<QWidget*> m_RTO_wave_list;
    QList<mc_chart*> m_RTO_chart_list;
    QList<QWidget*> m_RTO_option_list;
    QList<QLabel*> m_RTO_minus_list;
    QList<QLabel*> m_RTO_name_list;
    std::vector<dbDisplayItems> m_nurse_items;
    QList<std::vector<float>> m_rtchart_wave_list[MAX_WAVE];
    QList<uint64_t> m_rtchart_time_list[MAX_WAVE];

    void add_wave_to_chart_RTO(int series_index, std::string model, std::string mdc_code,
                               dds::sub::DataReader<dds::core::xtypes::DynamicData> reader,
                               mc_chart *chart,
                               QList<std::vector<float>> &wave_list,
                               QList<uint64_t> &time_list);
    void set_wave_ui(QList<QWidget*> wave_list) {m_RTO_wave_list = wave_list;};
    void set_chart_ui(QList<mc_chart*> chart_list) {m_RTO_chart_list = chart_list;};
    void set_option_ui(QList<QWidget*> option_list) {m_RTO_option_list = option_list;};
    void set_minus_ui(QList<QLabel*> minus_list) {m_RTO_minus_list = minus_list;};
    void set_name_ui(QList<QLabel*> name_list) {m_RTO_name_list = name_list;};
    void set_frame_ui(QVBoxLayout *item, QFrame *loop_frame, QFrame *add_frame)
    {
        m_main_item = item;
        m_loop_frame = loop_frame;
        m_add_frame = add_frame;
    };
    void mc_add_clicked(mc_wavepanel* wp);
    void mc_del_clicked(int index);
    void mc_enlarge_clicked(int inxex);

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
    std::vector<dbDisplayItems> CheckNurseDB();
private slots:
    void controls_clicked();
    void UpdateWave();
};

#endif // MC_WAVEPANEL_H
