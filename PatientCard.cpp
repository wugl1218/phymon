#include "PatientCard.h"
#include "Common.h"
#include "qpainter.h"

PatientCard::PatientCard(QWidget *parent)
    : QWidget{parent}
{
    setAutoFillBackground(true);
    male = 1;
    selected = 0;
}

void PatientCard::paintEvent(QPaintEvent *event)
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

void PatientCard::mouseMoveEvent(QMouseEvent *event)
{

}

void PatientCard::mousePressEvent(QMouseEvent *event)
{
    emit clicked();
}

void PatientCard::mouseReleaseEvent(QMouseEvent *event)
{

}

void PatientCard::set_is_male(uint8_t is_male)
{
    male = is_male;
    update();
}

uint8_t PatientCard::is_male()
{
    return male;
}

void PatientCard::set_selected(uint8_t selected)
{
    this->selected = selected;
    update();
}

uint8_t PatientCard::is_selected()
{
    return selected;
}

void PatientCard::set_bed(std::string bed)
{
    this->bed = bed;
    update();
}

std::string PatientCard::get_bed()
{
    return bed;
}

void PatientCard::set_mrn(std::string mrn)
{
    this->mrn = mrn;
    update();
}

std::string PatientCard::get_mrn()
{
    return mrn;
}

void PatientCard::set_name(std::string name)
{
    this->name = name;
    update();
}

std::string PatientCard::get_name()
{
    return name;
}
