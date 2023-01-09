#include "Dialog_release.h"
#include "qevent.h"
#include "ui_Dialog_release.h"

ReleaseDialog::ReleaseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReleaseDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);
}

ReleaseDialog::~ReleaseDialog()
{
    delete ui;
}

void ReleaseDialog::mousePressEvent(QMouseEvent *event)
{

}

std::string ReleaseDialog::getText()
{
    return ui->lineEdit->text().toStdString();
}

void ReleaseDialog::clearText()
{
    ui->lineEdit->clear();
    ui->lineEdit->setFocus();
}

void ReleaseDialog::on_cancel_btn_clicked()
{
    ok = 0;
    close();
}


void ReleaseDialog::on_ok_btn_clicked()
{
    if(ui->lineEdit->text().size() == 0)
        return;
    ok = 1;
    close();
}
