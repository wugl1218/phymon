#ifndef THREAD_NETWORK_H
#define THREAD_NETWORK_H

#include <QAbstractItemModel>
#include <QObject>
#include <QThread>
#include <QAbstractSocket>
#include <QMessageBox>
#include "Common.h"
#include <QTcpSocket>

class network_thread : public QThread
{
    Q_OBJECT
signals:
    void message(const QString& info);
    void progress(int present);
    void force_return();
    void nd_hide();
    void client_close();
public:
    network_thread(QWidget *parent = nullptr);
    ~network_thread();
    void init();
    void run();
    QString url ;
    QString msg ;
    bool is_network_error = 0;
    bool is_network_checking = 0;
    bool frist =1;
    QJsonObject jsonReplicator;
    void foyaCouchbaseReplicator();
    QTcpSocket *client = nullptr;

private:
    int m_runCount;

};
#endif // THREAD_NETWORK_H
