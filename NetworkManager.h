    #ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QAbstractItemModel>
#include "Common.h"
#include <QAbstractSocket>

class NetworkManager
{
public:
    cbl::Replicator repSync;
    void init();
    void step();
    uint32_t last_query_time;
    QString url ;
    QString msg ;
    bool is_network_error = 0;
    bool is_network_checking = 0;
    bool frist =1;
    QJsonObject jsonReplicator;
    void foyaCouchbaseReplicator();
private:
};

#endif // NETWORKMANAGER_H
