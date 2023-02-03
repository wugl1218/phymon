#ifndef MC_BTN_CLICKABLESTRETCH_H
#define MC_BTN_CLICKABLESTRETCH_H

#include <QLabel>
#include <QWidget>

typedef void (*mc_btn_ClickableStretch_clicked_cb)(void* param);

class mc_btn_ClickableStretch : public QLabel
{
public:
    mc_btn_ClickableStretch(QWidget *parent = nullptr, mc_btn_ClickableStretch_clicked_cb cb = NULL);

protected:
    void mousePressEvent(QMouseEvent* event);

private:
    mc_btn_ClickableStretch_clicked_cb clicked_cb;
};

#endif // MC_BTN_CLICKABLESTRETCH_H
