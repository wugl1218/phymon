#include "Dialog_network.h"
#include "ui_Dialog_network.h"
#include <QCloseEvent>

NetworkDialog::NetworkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkDialog)
{
    ui->setupUi(this);
    if(is_server)
    {
        setWindowFlags(Qt::Dialog|Qt::WindowTitleHint| Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
        ui->ConfirmButton->setText("Close program");
        ui->ConfirmButton->setGeometry(130,120,156,41);
    }
    else
        setWindowFlags(Qt::WindowStaysOnTopHint|Qt::WindowTitleHint| Qt::CustomizeWindowHint|Qt::Popup);
}

NetworkDialog::~NetworkDialog()
{
    delete ui;
}
void NetworkDialog::mousePressEvent(QMouseEvent *event)
{

}

void NetworkDialog::on_ConfirmButton_clicked()
{

    if(is_server)
        {
        exit(1);
        }
    else
        close();
}

void NetworkDialog::keyPressEvent(QKeyEvent *event)
{
    //不處理ESC鍵盤事 件。
    switch (event->key())
    {
    case Qt::Key_Escape:
        return;

    default:
        break;
    }
    QDialog::keyPressEvent(event);
}
