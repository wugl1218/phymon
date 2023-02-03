#include "mc_wavepanel_add.h"
#include "qgridlayout.h"
#include "qlabel.h"

mc_wavepanel_add::mc_wavepanel_add(QWidget *parent, mc_wavepanel_add_clicked_cb cb)
    : QWidget(parent)
{
    clicked_cb = cb;
    QGridLayout* layout = new QGridLayout(this);
    QLabel* icon = new QLabel(this);
    icon->setPixmap(QPixmap(":/icons/add_icon.png"));
    layout->addWidget(icon, 0, 0, Qt::AlignHCenter);
}

void mc_wavepanel_add::mousePressEvent(QMouseEvent* event)
{
    if(clicked_cb)
        clicked_cb(this->parent());
}
