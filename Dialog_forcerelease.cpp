#include "Dialog_forcerelease.h"
#include "ui_Dialog_forcerelease.h"

Dialog_forcerelease::Dialog_forcerelease(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_forcerelease)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::WindowStaysOnTopHint|Qt::WindowTitleHint| Qt::CustomizeWindowHint);
    setWindowFlags(Qt::Popup);
}

Dialog_forcerelease::~Dialog_forcerelease()
{
    delete ui;
}

std::string Dialog_forcerelease::getText()
{
    return ui->lineEdit->text().toStdString();
}

void Dialog_forcerelease::clearText()
{
    ui->lineEdit->clear();
    ui->lineEdit->setFocus();
}

void Dialog_forcerelease::on_pushButton_clicked()
{
    if(ui->lineEdit->text().size() == 0)
        return;
    close();
}

void Dialog_forcerelease::mousePressEvent(QMouseEvent *event)
{

}
