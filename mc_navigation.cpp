#include "mc_navigation.h"
#include "qevent.h"
#include "qpainter.h"
#include "Common.h"
#include "MainDialog.h"
#include "Tab_Observations_metricItemsDisplayConfigPage_Widget.h"

mc_navigation::mc_navigation(QWidget* parent)
    :QWidget(parent)
{
    current_tab = 0;
}

void mc_navigation::set_current_tab(int i)
{
    Common* common = Common::instance();
    if(current_tab == i)
        return;
    current_tab = i;
    update();
    emit on_tab_switched(i);
}

int mc_navigation::get_current_tab()
{
    return current_tab;
}

void mc_navigation::set_enabled(uint8_t enabled)
{
    this->enabled = enabled;
}

uint8_t mc_navigation::is_enabled()
{
    return enabled;
}

void mc_navigation::paintEvent(QPaintEvent *event)
{
    Common* common = Common::instance();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.rotate(-90);
    QFont font = painter.font();
    font.setPointSize(21);
    painter.setFont(font);
    QPen white_pen(Qt::white);
    QPen yellow_pen(QColor(0xeb,0xff,0x00));
    QPen grey_pen(QColor(61, 56, 70));
    QBrush teal_brush(QColor(0x29,0xf1,0xfa));
    QPen no_pen(Qt::NoPen);
    if(current_tab == 0)
    {
        painter.setPen(yellow_pen);
        Common::draw_text(painter, -607, 21, Qt::AlignLeft | Qt::AlignVCenter, "Mapping");
        if(common->patient_id.size()==0||common->md->dm.devices.size()==0)
            painter.setPen(grey_pen);
        else
            painter.setPen(white_pen);
        Common::draw_text(painter, -205, 21, Qt::AlignLeft | Qt::AlignVCenter, "Observations");
        if(!common->device_settings_page->has_checked)
            painter.setPen(grey_pen);
        Common::draw_text(painter, -463, 21, Qt::AlignLeft | Qt::AlignVCenter, "Utilities");
        Common::draw_text(painter, -335, 21, Qt::AlignLeft | Qt::AlignVCenter, "Devices");
        painter.setPen(no_pen);
        painter.setBrush(teal_brush);
        std::vector<QPoint> pts;
        pts.push_back(QPoint(-619, 16));
        pts.push_back(QPoint(-614, 21));
        pts.push_back(QPoint(-619, 26));
        painter.drawConvexPolygon(&pts[0], pts.size());
    }
    else if(current_tab == 1)
    {
        painter.setPen(white_pen);
        Common::draw_text(painter, -607, 21, Qt::AlignLeft | Qt::AlignVCenter, "Mapping");
        painter.setPen(yellow_pen);
        Common::draw_text(painter, -463, 21, Qt::AlignLeft | Qt::AlignVCenter, "Utilities");
        painter.setPen(white_pen);
        Common::draw_text(painter, -335, 21, Qt::AlignLeft | Qt::AlignVCenter, "Devices");
        Common::draw_text(painter, -205, 21, Qt::AlignLeft | Qt::AlignVCenter, "Observations");
        painter.setPen(no_pen);
        painter.setBrush(teal_brush);
        std::vector<QPoint> pts;
        pts.push_back(QPoint(-475, 16));
        pts.push_back(QPoint(-470, 21));
        pts.push_back(QPoint(-475, 26));
        painter.drawConvexPolygon(&pts[0], pts.size());
    }
    else if(current_tab == 2)
    {
        painter.setPen(white_pen);
        Common::draw_text(painter, -607, 21, Qt::AlignLeft | Qt::AlignVCenter, "Mapping");
        Common::draw_text(painter, -463, 21, Qt::AlignLeft | Qt::AlignVCenter, "Utilities");
        painter.setPen(yellow_pen);
        Common::draw_text(painter, -335, 21, Qt::AlignLeft | Qt::AlignVCenter, "Devices");
        painter.setPen(white_pen);
        Common::draw_text(painter, -205, 21, Qt::AlignLeft | Qt::AlignVCenter, "Observations");
        painter.setPen(no_pen);
        painter.setBrush(teal_brush);
        std::vector<QPoint> pts;
        pts.push_back(QPoint(-347, 16));
        pts.push_back(QPoint(-342, 21));
        pts.push_back(QPoint(-347, 26));
        painter.drawConvexPolygon(&pts[0], pts.size());
    }
    else
    {
        painter.setPen(white_pen);
        Common::draw_text(painter, -607, 21, Qt::AlignLeft | Qt::AlignVCenter, "Mapping");
        if(!common->device_settings_page->has_checked)
            painter.setPen(grey_pen);
        Common::draw_text(painter, -463, 21, Qt::AlignLeft | Qt::AlignVCenter, "Utilities");
        Common::draw_text(painter, -335, 21, Qt::AlignLeft | Qt::AlignVCenter, "Devices");
        painter.setPen(yellow_pen);
        Common::draw_text(painter, -205, 21, Qt::AlignLeft | Qt::AlignVCenter, "Observations");
        painter.setPen(no_pen);
        painter.setBrush(teal_brush);
        std::vector<QPoint> pts;
        pts.push_back(QPoint(-217, 16));
        pts.push_back(QPoint(-212, 21));
        pts.push_back(QPoint(-217, 26));
        painter.drawConvexPolygon(&pts[0], pts.size());
    }
}

void mc_navigation::mousePressEvent(QMouseEvent *event)
{
    Common* common = Common::instance();
    if(common->patient_id.size()==0 || (common->md->dm.devices.size()==0 && common->is_server ==0)) //philo
        return;
    if(event->pos().y() > 480)
        set_current_tab(0);
    else if(event->pos().y() > 350)
    {
        if(common->device_settings_page->has_checked)
            set_current_tab(1);
    }
    else if(event->pos().y() > 220)
    {
        if(common->device_settings_page->has_checked)
            set_current_tab(2);
    }
    else
    {
        set_current_tab(3);
    }
}
