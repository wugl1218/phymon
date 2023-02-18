#include "mc_btn_Clickable.h"
//ms
mc_btn_Clickable::mc_btn_Clickable(QWidget *parent)
    :QLabel(parent)
{

}

void mc_btn_Clickable::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
    emit pressed();
}

void mc_btn_Clickable::mouseReleaseEvent(QMouseEvent* event)
{
    emit released();
}
