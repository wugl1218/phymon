#include "mc_legend.h"
#include "qevent.h"
#include "qpainter.h"
#include "Common.h"

mc_legend::mc_legend(QWidget *parent)
    :QWidget(parent)
{
    left_margin = 0;
    top_margin = 0;
    square_width = 20;
    vertical_spacing = 40;
    font_size = 16;
}

void mc_legend::set_left_margin(int margin)
{
    left_margin = margin;
    update();
}

int mc_legend::get_left_margin()
{
    return left_margin;
}

void mc_legend::set_top_margin(int margin)
{
    top_margin = margin;
    update();
}

int mc_legend::get_top_margin()
{
    return top_margin;
}

void mc_legend::set_vertical_spacing(int spacing)
{
    vertical_spacing = spacing;
    update();
}

int mc_legend::get_vertical_spacing()
{
    return vertical_spacing;
}

void mc_legend::set_square_width(int width)
{
    square_width = width;
    update();
}

int mc_legend::get_square_width()
{
    return square_width;
}

void mc_legend::set_font_size(int size)
{
    font_size = size;
    update();
}

int mc_legend::get_font_size()
{
    return font_size;
}

static void mc_legend_init(mc_legend_entry* e)
{
    e->color.setRgb(255,255,255,255);
}

void mc_legend::set_series_color(int series_index, QColor color)
{
    if(series_index >= (int)entries.size())
    {
        mc_legend_entry e;
        mc_legend_init(&e);
        entries.resize(series_index+1, e);
    }
    entries[series_index].color = color;
    update();
}

QColor mc_legend::get_series_color(int series_index)
{
    if(series_index >= (int)entries.size())
    {
        mc_legend_entry e;
        mc_legend_init(&e);
        return e.color;
    }
    return entries[series_index].color;
}

void mc_legend::set_series_text(int series_index, std::string text)
{
    if(series_index >= (int)entries.size())
    {
        mc_legend_entry e;
        mc_legend_init(&e);
        entries.resize(series_index+1, e);
    }
    entries[series_index].name = text;
    update();
}

std::string mc_legend::get_series_text(int series_index)
{
    if(series_index >= (int)entries.size())
        return "";
    return entries[series_index].name;
}

void mc_legend::remove_series(int series_index)
{
    int i=0;
    for(auto it=entries.begin();it!=entries.end();it++)
    {
        if(i==series_index)
        {
            entries.erase(it);
            break;
        }
        i++;
    }
}

void mc_legend::set_text_color(QColor color)
{
    text_color = color;
    update();
}

QColor mc_legend::get_text_color()
{
    return text_color;
}

void mc_legend::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen nopen(Qt::NoPen);
    QPen pen(text_color);
    painter.setPen(nopen);
    for(int i=0;i<(int)entries.size();i++)
    {
        QBrush brush(entries[i].color);
        painter.setBrush(brush);
        painter.setPen(nopen);
        painter.drawRect(left_margin, top_margin + vertical_spacing*i, square_width, square_width);
        if(entries[i].name.size() > 0)
        {
            painter.setPen(pen);
            Common::draw_text(painter, left_margin + square_width + 10,
                              top_margin + vertical_spacing*i + square_width/2.0,
                              Qt::AlignLeft | Qt::AlignVCenter, entries[i].name.c_str());
        }
    }
}

void mc_legend::mouseMoveEvent(QMouseEvent *event)
{

}

void mc_legend::mousePressEvent(QMouseEvent *event)
{
    int i = (event->pos().y() - top_margin + ((vertical_spacing-square_width)/2.0))/vertical_spacing;
    if(i < (int)entries.size())
        emit on_series_select(i);
}

void mc_legend::mouseReleaseEvent(QMouseEvent *event)
{

}
