#include "Dialog_network.h"
#include "ui_Dialog_network.h"
#include <QCloseEvent>

Dialog_network::Dialog_network(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_network)
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

Dialog_network::~Dialog_network()
{
    delete ui;
}
void Dialog_network::mousePressEvent(QMouseEvent *event)
{

}

void Dialog_network::on_ConfirmButton_clicked()
{

    if(is_server)
        {
        exit(1);
        }
    else
        close();
}

void Dialog_network::keyPressEvent(QKeyEvent *event)
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
