#include "mc_selection_entry.h"
#include "Common.h"
#include "qevent.h"
#include "qpainter.h"
#include <vector>

mc_selection_entry::mc_selection_entry(QWidget *parent)
    : QWidget{parent}
{
    margin_x = 30;
    margin_y = 6;
    tab_index = 0;
    table_index = 0;
}

void mc_selection_entry::set_type(int type)
{
    this->type = type;
    update();
}

int mc_selection_entry::get_type()
{
    return type;
}

void mc_selection_entry::set_text(const char* text)
{
    this->text = text;
    update();
}

std::string mc_selection_entry::get_text()
{
    return text;
}

void mc_selection_entry::set_margin_x(int margin)
{
    margin_x = margin;
    update();
}

void mc_selection_entry::set_margin_y(int margin)
{
    margin_y = margin;
    update();
}

void mc_selection_entry::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPen frame_pen(QColor(0x2f,0x52,0x73));
    painter.setPen(frame_pen);
    std::vector<QPoint> framepoints;
    framepoints.push_back(QPoint(margin_x, margin_y));
    framepoints.push_back(QPoint(margin_x, height()-margin_y));
    framepoints.push_back(QPoint(width()-margin_x, height()-margin_y));
    framepoints.push_back(QPoint(width()-margin_x, margin_y));
    framepoints.push_back(QPoint(margin_x, margin_y));
    painter.drawPolyline(&framepoints[0], framepoints.size());
    QPen text_pen(Qt::white);
    painter.setPen(text_pen);
    QFont font = painter.font();
    font.setPixelSize(24);
    font.setFamily("Arial");
    painter.setFont(font);
    Common::draw_text(painter, margin_x+10, height()/2, Qt::AlignVCenter | Qt::AlignLeft, text.c_str());
    if(type == MC_ENTRY_RIGHT)
    {
        painter.drawPixmap(width()-margin_x-50, 15, QPixmap(":/icons/minus.png"));
        //painter.drawPixmap(width()-margin_x-100, 15, QPixmap(":/icons/up_arrow.png"));
        //painter.drawPixmap(width()-margin_x-150, 15, QPixmap(":/icons/down_arrow.png"));
    }
    else if(type == MC_ENTRY_LEFT)
        painter.drawPixmap(width()-margin_x-50, 15, QPixmap(":/icons/plus.png"));
}

void mc_selection_entry::mousePressEvent(QMouseEvent *event)
{
    if(type == MC_ENTRY_STATIC)
        return;
    if(event->pos().x() >= width()-margin_x-50)
        emit button_press(type);
    /*
    else if(type == MC_ENTRY_RIGHT && event->pos().x() >= width()-margin_x-100)
        emit button_press(1);
    else if(type == MC_ENTRY_RIGHT && event->pos().x() >= width()-margin_x-150)
        emit button_press(0);*/
}
