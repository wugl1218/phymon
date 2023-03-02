#ifndef MC_WAVEPANEL_ADD_H
#define MC_WAVEPANEL_ADD_H
#include "mc_chart.h"

#include <QWidget>

typedef void (*mc_wavepanel_add_clicked_cb)(void* param);

class mc_wavepanel_add : public QWidget
{
public:
    explicit mc_wavepanel_add(QWidget *parent = nullptr, mc_wavepanel_add_clicked_cb cb = NULL);

protected:
    void mousePressEvent(QMouseEvent* event);
private:
    mc_wavepanel_add_clicked_cb clicked_cb;
};

#endif // MC_WAVEPANEL_ADD_H
