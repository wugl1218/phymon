#include "mc_legend.h"
#include "qevent.h"
#include "qpainter.h"
#include "Common.h"

mc_legend::mc_legend(QWidget *parent)
    :QWidget(parent)
{
    square_width = 220;
    square_height =80;
    vertical_spacing = 10;
    font_size = 16;
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

void mc_legend::set_series_color(QColor color)
{

    entries.color = color;
    update();
}

QColor mc_legend::get_series_color()
{
    return entries.color;
}

void mc_legend::set_series_text(std::string text,
                                std::string model,
                                std::string unit,
                                std::string mdccode,
                                float val,
                                std::string y_min,
                                std::string y_max,
                                std::string datasource
                               )
{

    entries.name = text;
    entries.model = model;
    entries.unit = unit;
    entries.val = val;
    entries.mdccode = mdccode;
    entries.y_min = y_min;
    entries.y_max = y_max;
    entries.datasource = datasource;

    update();
}

std::string mc_legend::get_series_text()
{
    return entries.name;
}

void mc_legend::remove_series()
{
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
    Common* common = Common::instance();
    QPainter painter(this);
    if(entries.name.size() == 0)return;
    QPen nopen(Qt::NoPen);
    QColor blackColor =QColor(04,05,07);
    QPen black(blackColor);
    painter.setPen(nopen);
    painter.drawRect(0,0,width(),height());
    QFont font ;
    font.setPixelSize(22);
    painter.setFont(font);

    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(text_color);
//    QColor blackColor =QColor(04,05,07);
//    QPen black(blackColor);
//    painter.setPen(nopen);
//    painter.drawRect(0,0,width(),height());
        QBrush brush(entries.color);
        painter.setBrush(brush);
        painter.setPen(nopen);

        painter.drawRoundedRect(0,0,square_width,square_height,10,10);
        if(entries.name.size() > 0)
        {

            std::string name =entries.name;
            name.append(" (");
            name.append(entries.model);
            name.append(")");
            painter.setPen(pen);
            Common::draw_text(painter, 10,
                              0 + 20,
                              Qt::AlignLeft | Qt::AlignVCenter, name.c_str());
            Common::draw_text(painter, 10,
                              0 + 50,
                              Qt::AlignLeft | Qt::AlignVCenter, QString::number(entries.val));
            Common::draw_text(painter, 135,
                              0 + 50,
                              Qt::AlignLeft | Qt::AlignVCenter, QString::fromStdString(entries.unit));

        }
    update();

}

void mc_legend::mouseMoveEvent(QMouseEvent *event)
{

}

void mc_legend::mousePressEvent(QMouseEvent *event)
{

    std::string name =entries.name;
    std::string mdccode =entries.mdccode;
    std::string model =entries.model;
    std::string y_min=entries.y_min;
    std::string y_max=entries.y_max;
    std::string unit=entries.unit;
    std::string datasource=entries.datasource;
    emit on_series_select(name,model,mdccode,y_min,y_max,unit,datasource);
}


void mc_legend::mouseReleaseEvent(QMouseEvent *event)
{

}
