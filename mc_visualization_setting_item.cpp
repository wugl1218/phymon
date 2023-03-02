#include "mc_visualization_setting_item.h"
#include "qpainter.h"
#include "Common.h"
mc_visualization_setting_item::mc_visualization_setting_item(QWidget *parent)
    : QWidget{parent}
{
    margin_x = 30;
    margin_y = 6;
    tab_index = 0;
    table_index = 0;
}

void mc_visualization_setting_item::paintEvent(QPaintEvent *event)
{
    Common* common = Common::instance();

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    if(1==1)
    {
        painter.setBrush(QColor(7, 22, 40));
        painter.drawRect(0,0,width(),height());
        painter.setBrush(QColor(3, 41, 86));
        painter.drawRect(0,0,120,70);
    }
    else
    {
        painter.setBrush(QColor(11, 42, 78));
        painter.drawRect(0,0,width(),height());
        painter.setBrush(QColor(9, 58, 115));
        painter.drawRect(0,0,130,60);

    }
    QPen text_pen(Qt::white);
    painter.setPen(text_pen);
    QFont font = painter.font();
    font.setPixelSize(16);
    font.setFamily("Arial");
    painter.setFont(font);
    std::string dataname ="dataname";
    std::string Device ="(Device)";
    int length1= dataname.length()*5;
    int length2= Device.length()*5;
    Common::draw_text(painter, 60-length1,22, Qt::AlignLeft | Qt::AlignVCenter, "dataname");
    Common::draw_text(painter, 60-length2,47, Qt::AlignLeft | Qt::AlignVCenter, "(Device)");
    QPen yellow_pen(common->css.alarm_yellow);
    QPen red_pen(common->css.alarm_red);
    font.setPixelSize(20);
    painter.setFont(font);
    painter.setPen(yellow_pen);
    Common::draw_text(painter, 190,35, Qt::AlignLeft | Qt::AlignVCenter, "100");
    Common::draw_text(painter, 240,35, Qt::AlignLeft | Qt::AlignVCenter, "200");
    painter.setPen(red_pen);
    Common::draw_text(painter, 140,35, Qt::AlignLeft | Qt::AlignVCenter, "000");
    Common::draw_text(painter, 290,35, Qt::AlignLeft | Qt::AlignVCenter, "300");

}
