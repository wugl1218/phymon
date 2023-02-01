#include "NetworkManager.h"
#include "MainDialog.h"
#include <QMessageBox>
#include "ui_Dialog_network.h"

//已用Thread_network代替 未來刪除

void NetworkManager::init()
{
    Common* common = Common::instance();

    last_query_time=0;

    if (QFile::exists("../config/config.json"))
    {
        jsonReplicator = common->RetrieveJson(("../config/config.json"));
        url = jsonReplicator["VMDUrl"].toString();
        int posStart = url.indexOf("/");
        int posEnd = url.lastIndexOf(":");
        url = url.sliced(posStart + 2, posEnd - posStart - 2);
        qDebug()<<"ping IP ="<<url;
        msg = "";
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Debug by Philo\nPlease check config.json.");
        msgBox.exec();
        exit(1);
    }
    if(common->is_server)
        common->md->nd.is_server=1;
    else
        common->md->nd.is_server=0;
}
void NetworkManager::step()
{
return;
    Common* common = Common::instance();
    uint32_t current_time = Common::get_time_ms();
    if(Common::get_elapsed_time(current_time, last_query_time) > (uint32_t)common->Network_Interval*1000)
    {
   qDebug()<<"NetworkManager";
        fflog_out(common->log,"NetworkManager");
        last_query_time = current_time;
        foyaCouchbaseReplicator();
    }

}
void NetworkManager::foyaCouchbaseReplicator()
{
 /*   Common* common = Common::instance();
    common->md->client->connectToHost(url, 80);
    // we need to wait...
    if(!common->md->client->waitForConnected(800))
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
    if(common->md->client->isOpen())
        common->md->client->close();*/
}
