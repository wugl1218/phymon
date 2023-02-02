#include "Dialog_topalarm_mute.h"
#include "ui_Dialog_topalarm_mute.h"

Dialog_topalarm_mute::Dialog_topalarm_mute(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_topalarm_mute)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);
}

Dialog_topalarm_mute::~Dialog_topalarm_mute()
{
    delete ui;
}
int Dialog_topalarm_mute::get_mutetime()
{
    return mutetime;
}

void Dialog_topalarm_mute::on_mutebtn1_clicked()
{
    mutetime = 0;
    close();
}


void Dialog_topalarm_mute::on_mutebtn2_clicked()
{
    mutetime = 2;
    close();
}


void Dialog_topalarm_mute::on_mutebtn3_clicked()
{
    mutetime = 5;
    close();
}


void Dialog_topalarm_mute::on_mutebtn4_clicked()
{
    mutetime = 10;
    close();
}

