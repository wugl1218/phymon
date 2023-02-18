#ifndef MC_WAVEPANEL_H
#define MC_WAVEPANEL_H

#include "Dialog_wave_selection.h"
#include "mc_btn_Clickable.h"
#include "qboxlayout.h"
#include "qlabel.h"
#include <QWidget>
#include <stdint.h>
#include <QTimer>

struct stDisplayItems
{
    std::string model;
    std::string display_desc;
    std::string wave_type;
    std::string mdc_code;

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
    std::vector<std::string> GetDisplayIntersec(std::string model, std::string type);
    std::string m_DeviceName;
    //int m_LowerCount;
signals:

private:
    QLabel* corner;
    QVBoxLayout* main_layout;
    QVBoxLayout* item_layout;
    QHBoxLayout* header_layout;
    QWidget* header_bar;
    mc_btn_Clickable* controls_btn;
    uint8_t controls_on;
    Dialog_wave_selection menu;

    void render_controls_btn();
    void push_add_item();
private slots:
    void controls_clicked();
    void UpdateWave();
};

#endif // MC_WAVEPANEL_H
