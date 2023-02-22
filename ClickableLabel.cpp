#include "ClickableLabel.h"
//ms
ClickableLabel::ClickableLabel(QWidget *parent)
    :QLabel(parent)
{

}

void ClickableLabel::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
    emit pressed();
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent* event)
{
    emit released();
}
