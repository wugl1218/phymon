#include "Dialog_forcerelease.h"
#include "ui_Dialog_forcerelease.h"

ForceReleaseDialog::ForceReleaseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ForceReleaseDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::WindowStaysOnTopHint|Qt::WindowTitleHint| Qt::CustomizeWindowHint);
    setWindowFlags(Qt::Popup);
}

ForceReleaseDialog::~ForceReleaseDialog()
{
    delete ui;
}

std::string ForceReleaseDialog::getText()
{
    return ui->lineEdit->text().toStdString();
}

void ForceReleaseDialog::clearText()
{
    ui->lineEdit->clear();
    ui->lineEdit->setFocus();
}

void ForceReleaseDialog::on_pushButton_clicked()
{
    if(ui->lineEdit->text().size() == 0)
        return;
    close();
}

void ForceReleaseDialog::mousePressEvent(QMouseEvent *event)
{

}
