#ifndef TAB_NS_MONITOR_H
#define TAB_NS_MONITOR_H

#include <QWidget>
#include <QDate>
#include <QFile>
#include "Card_Patient.h"
#include "qtimer.h"
#include "Dialog_release.h"
#include <vector>
#include "Card_MDSConnectivity.h"
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

private slots:
    void patient_clicked();


signals:
    void bindingChanged();
private:
    Ui::Tab_NS_Monitor *ui;
    std::vector<Card_MDSConnectivity*> cards;
    void on_itemSelectionChanged();


};

#endif // TAB_NS_MONITOR_H
