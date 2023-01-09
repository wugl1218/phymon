#include "Thread_network.h"
#include <QDebug>
#include <QThread>
#include "MainDialog.h"
#include <QMessageBox>
#include "ui_Dialog_network.h"

network_thread::network_thread(QWidget *parent)
{
    client = new QTcpSocket(this);
    client->connectToHost(url, 80);
}
network_thread::~network_thread()
{
}
void network_thread::init()
{
    return;
    Common* common = Common::instance();
    if (QFile::exists("../config/config.json"))
    {
        jsonReplicator = common->RetrieveJson(("../config/config.json"));
        url = jsonReplicator["PingIP"].toString();
        int posStart = url.indexOf("/");
        int posEnd = url.lastIndexOf(":");
        url = url.sliced(posStart + 2, posEnd - posStart - 2);
        msg = "";
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Debug by Philo\nPlese check config.json.");
        msgBox.exec();
    }
    if(common->is_server)
        common->md->nd.is_server=1;
    else
        common->md->nd.is_server=0;
}
void network_thread::run()
{
    return;
    Common* common = Common::instance();
    while(1)
    {
       qDebug()<<"Network_thread";
        fflog_out(common->log,"Network_thread");
        foyaCouchbaseReplicator();
        if(common->is_server)
            QThread::currentThread()->msleep(common->Network_Interval*1000);
        else
            QThread::currentThread()->msleep(common->Network_Interval*3000);

    }
}
void network_thread::foyaCouchbaseReplicator()
{
    Common* common = Common::instance();
    // we need to wait...
    client = new QTcpSocket;
    client->connectToHost(url, 80);
    if(!client->waitForConnected(1200))
    {
        if(common->is_server)
            common->md->ui->mapping_tab->force_return();
       common->md->ui->network_label->setStyleSheet("border-image: url(:/icons/Network_not.png);");
        if(frist)
            {
            common->md->nd.show();
            frist =0;
            }
        is_network_error = 1;
        is_network_checking =1;
    }
    else
    {
        if(!common->is_server)
            common->md->ui->network_label->setText("");
        if(is_network_error)
        {
            is_network_error = 0;
        }
        frist =1;
        common->md->ui->network_label->setStyleSheet("border-image: url(:/icons/Network.png);");
        if(!common->md->nd.isHidden())
            common->md->nd.hide();
    }
    if(client->isOpen())
        client->close();
}
