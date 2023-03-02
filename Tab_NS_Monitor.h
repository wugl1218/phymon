#ifndef TAB_NS_MONITOR_H
#define TAB_NS_MONITOR_H

#include <QWidget>
#include <QDate>
#include <QFile>
#include "Manager_Patient_Card.h"
#include "qtimer.h"
#include "Dialog_release.h"
#include <vector>
#include "Manager_MDSConnectivity_Card.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QFile>


namespace Ui {
class Tab_NS_Monitor;
}

class Tab_NS_Monitor : public QWidget
{
    Q_OBJECT

public:
    explicit Tab_NS_Monitor(QWidget *parent = nullptr);
    ~Tab_NS_Monitor();
    void update_MDS();
    int selected_patient;
    void init();
    QString Bed_size;
    QJsonArray Set_bed;
    int Card_spacing;
    int Card_width;
    int Card_height;
    /*width=1920 height=1080*/
private slots:
    void patient_clicked();


signals:
    void bindingChanged();
private:
    Ui::Tab_NS_Monitor *ui;
    std::vector<Manager_MDSConnectivity_Card*> cards;
    void on_itemSelectionChanged();


};

#endif // TAB_NS_MONITOR_H
