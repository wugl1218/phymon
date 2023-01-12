#include "mc_btn_topalart.h"
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>


Topalartbtn::Topalartbtn(QWidget *parent): QLabel(parent)
{
}

void Topalartbtn::mousePressEvent(QMouseEvent *event)
{
}

void Topalartbtn::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if(event->button() == Qt::LeftButton)
    {
        emit on_click();
    }
}

