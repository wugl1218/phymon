#include "mc_btn_ClickableStretch.h"

mc_btn_ClickableStretch::mc_btn_ClickableStretch(QWidget *parent, mc_btn_ClickableStretch_clicked_cb cb)
    : QLabel(parent)
{
    clicked_cb = cb;
}

void mc_btn_ClickableStretch::mousePressEvent(QMouseEvent* event)
{
    if(clicked_cb)
        clicked_cb(this->parent());
}
