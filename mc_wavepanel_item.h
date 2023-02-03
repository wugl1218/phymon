#ifndef MC_WAVEPANEL_ITEM_H
#define MC_WAVEPANEL_ITEM_H

#include "qboxlayout.h"
#include <QWidget>
#include <stdint.h>

class mc_wavepanel_item : public QWidget
{
    Q_OBJECT
public:
    explicit mc_wavepanel_item(QWidget *parent = nullptr);
    void set_controls_enabled(uint8_t enabled);

signals:

private:
    uint8_t controls_on;
    QHBoxLayout* main_layout;

    void enable_controls();
    void disable_controls();
};

#endif // MC_WAVEPANEL_ITEM_H
