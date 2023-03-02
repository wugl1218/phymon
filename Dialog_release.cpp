#include "Dialog_release.h"
#include "qevent.h"
#include "ui_Dialog_release.h"

Dialog_release::Dialog_release(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_release)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);
}

Dialog_release::~Dialog_release()
{
    delete ui;
}

void Dialog_release::mousePressEvent(QMouseEvent *event)
{

}

std::string Dialog_release::getText()
{
    return ui->lineEdit->text().toStdString();
}

void Dialog_release::clearText()
{
    ui->lineEdit->clear();
    ui->lineEdit->setFocus();
}

void Dialog_release::on_cancel_btn_clicked()
{
    ok = 0;
    close();
}


void Dialog_release::on_ok_btn_clicked()
{
    if(ui->lineEdit->text().size() == 0)
        return;
    ok = 1;
    close();
}
