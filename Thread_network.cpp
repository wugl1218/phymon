#include "Thread_network.h"
#include <QDebug>
#include <QThread>
#include "MainDialog.h"
#include <QMessageBox>
#include "ui_Dialog_network.h"

Thread_network::Thread_network(QWidget *parent)
{
    client = new QTcpSocket(this);
    client->connectToHost(url, 80);
}
Thread_network::~Thread_network()
{
}
void Thread_network::init()
{
    Common* common = Common::instance();
    if (QFile::exists("../config/config.json"))
    {
        jsonReplicator = common->RetrieveJson(("../config/config.json"));
        url = jsonReplicator["PingIP"].toString();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Debug by Philo\nPlease check config.json.");
        msgBox.exec();
    }

}
void Thread_network::run()
{
    Common* common = Common::instance();
    qDebug()<<url;
    if(url=="0.0.0.0")return;
    while(1)
    {
       qDebug()<<"Thread_network";
        fflog_out(common->log,"Info :: Thread_network");
        foyaCouchbaseReplicator();
        if(common->is_server)
            QThread::currentThread()->msleep(common->Network_Interval*1000);
        else
            QThread::currentThread()->msleep(common->Network_Interval*3000);

    }
}
void Thread_network::foyaCouchbaseReplicator()
{
    Common* common = Common::instance();
    // we need to wait...
    client = new QTcpSocket;
    client->connectToHost(url, 8091);
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
