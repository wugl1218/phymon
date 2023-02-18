#ifndef MC_WAVEPANEL_H
#define MC_WAVEPANEL_H

#include "Dialog_wave_selection.h"
#include "mc_btn_Clickable.h"
#include "qboxlayout.h"
#include "qlabel.h"
#include <QWidget>
#include <stdint.h>

class mc_wavepanel : public QWidget
{
    Q_OBJECT
public:
    explicit mc_wavepanel(QWidget *parent = nullptr);
    void add_clicked();

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
};

#endif // MC_WAVEPANEL_H
