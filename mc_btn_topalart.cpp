#include "mc_btn_topalart.h"
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>


mc_btn_topalart::mc_btn_topalart(QWidget *parent): QLabel(parent)
{

}

void mc_btn_topalart::mousePressEvent(QMouseEvent *event)
{

}

void mc_btn_topalart::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if(event->button() == Qt::LeftButton)
    {
        emit on_click();
    }
}

