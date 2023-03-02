#include "mc_wavepanel_add.h"
#include "qgridlayout.h"
#include "qlabel.h"

mc_wavepanel_add::mc_wavepanel_add(QWidget *parent, mc_wavepanel_add_clicked_cb cb)
    : QWidget(parent)
{
    clicked_cb = cb;
    QHBoxLayout* layout = new QHBoxLayout(this);
    QLabel* minus_icon = new QLabel(this);
    minus_icon->setPixmap(QPixmap(":/icons/minus_icon.png"));
    layout->addWidget(minus_icon, 0, Qt::AlignLeft);

    QLabel* add_icon = new QLabel(this);
    add_icon->setPixmap(QPixmap(":/icons/Enlarge.png"));
    layout->addWidget(add_icon, 0, Qt::AlignLeft);
/*
    mc_chart *pRtChart = new mc_chart(this);
    pRtChart->set_axis_visible(1);
    pRtChart->set_selection_width(40);
    pRtChart->set_selection_type(MC_SELECT_SERIES);
    pRtChart->set_series_width(0,2);
    pRtChart->set_scrollable(0);
    pRtChart->set_zoomable(0);
    pRtChart->set_view_range_max_y(200);
    pRtChart->set_view_range_min_y(0);
    pRtChart->set_num_labels_x(5);
    pRtChart->set_num_labels_y(5);

    layout->addWidget(pRtChart, 0, Qt::AlignLeft);*/
}

void mc_wavepanel_add::mousePressEvent(QMouseEvent* event)
{
    if(clicked_cb)
        clicked_cb(this->parent());
}


