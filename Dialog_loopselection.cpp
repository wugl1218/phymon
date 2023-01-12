#include "Dialog_loopselection.h"
#include "ui_Dialog_loopselection.h"

Dialog_loopselection::Dialog_loopselection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_loopselection)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);
}

Dialog_loopselection::~Dialog_loopselection()
{
    delete ui;
}

void Dialog_loopselection::set_current_loops(int loop1_type, int loop2_type)
{
    this->loop1_type = loop1_type;
    this->loop2_type = loop2_type;
}

void Dialog_loopselection::get_current_loops(int* loop1_type, int* loop2_type)
{
    *loop1_type = this->loop1_type;
    *loop2_type = this->loop2_type;
}

void Dialog_loopselection::set_checked(QWidget* w, uint8_t checked)
{
    QPalette p = w->palette();
    if(checked)
        p.setColor(QPalette::Active, QPalette::Button, QColor(0x0e,0x39,0x6a));
    else
        p.setColor(QPalette::Active, QPalette::Button, QColor(0x34,0x65,0xA4));
    w->setPalette(p);
}

void Dialog_loopselection::showEvent(QShowEvent *event)
{
    ui->loop1btn1->setChecked(false);
    set_checked(ui->loop1btn1, 0);
    ui->loop1btn2->setChecked(false);
    set_checked(ui->loop1btn2, 0);
    ui->loop1btn3->setChecked(false);
    set_checked(ui->loop1btn3, 0);
    ui->loop1btn4->setChecked(false);
    set_checked(ui->loop1btn4, 0);
    ui->loop1btn5->setChecked(false);
    set_checked(ui->loop1btn5, 0);
    ui->loop2btn1->setChecked(false);
    set_checked(ui->loop2btn1, 0);
    ui->loop2btn2->setChecked(false);
    set_checked(ui->loop2btn2, 0);
    ui->loop2btn3->setChecked(false);
    set_checked(ui->loop2btn3, 0);
    ui->loop2btn4->setChecked(false);
    set_checked(ui->loop2btn4, 0);
    ui->loop2btn5->setChecked(false);
    set_checked(ui->loop2btn5, 0);
    if(loop1_type == LOOP_PRESSURE_VOLUME)
    {
        ui->loop1btn1->setChecked(true);
        set_checked(ui->loop1btn1, 1);
    }
    else if(loop1_type == LOOP_VOLUME_FLOW)
    {
        ui->loop1btn2->setChecked(true);
        set_checked(ui->loop1btn2, 1);
    }
    else if(loop1_type == LOOP_FLOW_PRESSURE)
    {
        ui->loop1btn3->setChecked(true);
        set_checked(ui->loop1btn3, 1);
    }
    else if(loop1_type == LOOP_PTRACH_VOLUME)
    {
        ui->loop1btn4->setChecked(true);
        set_checked(ui->loop1btn4, 1);
    }
    else
    {
        ui->loop1btn5->setChecked(true);
        set_checked(ui->loop1btn5, 1);
    }
    if(loop2_type == LOOP_PRESSURE_VOLUME)
    {
        ui->loop2btn1->setChecked(true);
        set_checked(ui->loop2btn1, 1);
    }
    else if(loop2_type == LOOP_VOLUME_FLOW)
    {
        ui->loop2btn2->setChecked(true);
        set_checked(ui->loop2btn2, 1);
    }
    else if(loop2_type == LOOP_FLOW_PRESSURE)
    {
        ui->loop2btn3->setChecked(true);
        set_checked(ui->loop2btn3, 1);
    }
    else if(loop2_type == LOOP_PTRACH_VOLUME)
    {
        ui->loop2btn4->setChecked(true);
        set_checked(ui->loop2btn4, 1);
    }
    else
    {
        ui->loop2btn5->setChecked(true);
        set_checked(ui->loop2btn5, 1);
    }
}

void Dialog_loopselection::on_loop1btn1_clicked()
{
    loop1_type = LOOP_PRESSURE_VOLUME;
    close();
}


void Dialog_loopselection::on_loop1btn2_clicked()
{
    loop1_type = LOOP_VOLUME_FLOW;
    close();
}


void Dialog_loopselection::on_loop1btn3_clicked()
{
    loop1_type = LOOP_FLOW_PRESSURE;
    close();
}


void Dialog_loopselection::on_loop2btn1_clicked()
{
    loop2_type = LOOP_PRESSURE_VOLUME;
    close();
}


void Dialog_loopselection::on_loop2btn2_clicked()
{
    loop2_type = LOOP_VOLUME_FLOW;
    close();
}


void Dialog_loopselection::on_loop2btn3_clicked()
{
    loop2_type = LOOP_FLOW_PRESSURE;
    close();
}


void Dialog_loopselection::on_loop1btn4_clicked()
{
    loop1_type = LOOP_PTRACH_VOLUME;
    close();
}


void Dialog_loopselection::on_loop2btn4_clicked()
{
    loop2_type = LOOP_PTRACH_VOLUME;
    close();
}


void Dialog_loopselection::on_loop1btn5_clicked()
{
    loop1_type = LOOP_FLOW_PTRACH;
    close();
}


void Dialog_loopselection::on_loop2btn5_clicked()
{
    loop2_type = LOOP_FLOW_PTRACH;
    close();
}

void Dialog_loopselection::mousePressEvent(QMouseEvent *event)
{

}
