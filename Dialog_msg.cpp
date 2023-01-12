#include "Dialog_msg.h"
#include "ui_Dialog_msg.h"

Dialog_msg::Dialog_msg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_msg)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);
    ui->label->setWordWrap(true);
    ui->label->adjustSize();
}

Dialog_msg::~Dialog_msg()
{
    delete ui;
}



void Dialog_msg::setText(QString qstr)
{
    ui->label->setText(qstr);
    ui->label->setWordWrap(true);
    ui->label->adjustSize();
    int nNewHeight = ui->label->height();
    ui->label->setGeometry(0, 50, 530, nNewHeight);
    ui->ConfirmButton->setGeometry(220, 70+nNewHeight, 90, 40);
    resize(530,135+nNewHeight);
}

void Dialog_msg::on_ConfirmButton_clicked()
{
    close();
}

