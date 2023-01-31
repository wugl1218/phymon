#include "Manager_Patient_Card.h"
#include "Common.h"
#include "qpainter.h"

Manager_Patient_Card::Manager_Patient_Card(QWidget *parent)
    : QWidget{parent}
{
    setAutoFillBackground(true);
    male = 1;
    selected = 0;
}

void Manager_Patient_Card::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(11, 42, 78));
    painter.drawRect(0,0,width(),height());
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QPen nopen(Qt::NoPen);
    QPen whitepen(Qt::white);
    QBrush lightbluebrush(QColor(0x07,0x62,0xca));
    if(selected)
    {
        painter.setPen(nopen);
        painter.setBrush(lightbluebrush);
        painter.drawRect(0,0,width(),height());
        painter.setPen(whitepen);
    }
    painter.setPen(whitepen);
    QTransform id = painter.worldTransform();
    painter.scale(0.7, 0.7);
    if(male)
        painter.drawPixmap(36, 55, QPixmap(":/icons/male.png"));
    else
        painter.drawPixmap(36, 55, QPixmap(":/icons/female.png"));
    painter.setWorldTransform(id);
    QFont font = painter.font();
    if(bed.size() > 0)
    {
        font.setPixelSize(20);
        painter.setFont(font);
        Common::draw_text(painter, 6, 16, Qt::AlignLeft | Qt::AlignVCenter, bed.c_str());
    }
    font.setPixelSize(24);
    painter.setFont(font);
    if(mrn.size() > 0)
        Common::draw_text(painter, 158, 44, Qt::AlignHCenter | Qt::AlignVCenter, mrn.c_str());
    if(name.size() > 0)
        Common::draw_text(painter, 148, 95, Qt::AlignLeft | Qt::AlignVCenter, name.c_str());
}

void Manager_Patient_Card::mouseMoveEvent(QMouseEvent *event)
{

}

void Manager_Patient_Card::mousePressEvent(QMouseEvent *event)
{
    emit clicked();
}

void Manager_Patient_Card::mouseReleaseEvent(QMouseEvent *event)
{

}

void Manager_Patient_Card::set_is_male(uint8_t is_male)
{
    male = is_male;
    update();
}

uint8_t Manager_Patient_Card::is_male()
{
    return male;
}

void Manager_Patient_Card::set_selected(uint8_t selected)
{
    this->selected = selected;
    update();
}

uint8_t Manager_Patient_Card::is_selected()
{
    return selected;
}

void Manager_Patient_Card::set_bed(std::string bed)
{
    this->bed = bed;
    update();
}

std::string Manager_Patient_Card::get_bed()
{
    return bed;
}

void Manager_Patient_Card::set_mrn(std::string mrn)
{
    this->mrn = mrn;
    update();
}

std::string Manager_Patient_Card::get_mrn()
{
    return mrn;
}

void Manager_Patient_Card::set_name(std::string name)
{
    this->name = name;
    update();
}

std::string Manager_Patient_Card::get_name()
{
    return name;
}
