#ifndef TAB_NS_MONITOR_H
#define TAB_NS_MONITOR_H

#include <QWidget>
#include <QDate>
#include <QFile>
#include "PatientCard.h"
#include "qtimer.h"
#include "Dialog_release.h"
#include <vector>
#include "Card_MDSConnectivity.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>


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


    void on_spinBox_valueChanged(int arg1);

signals:
    void bindingChanged();
private:
    Ui::Tab_NS_Monitor *ui;
    std::vector<MDSConnectivityCard*> cards;
    void on_itemSelectionChanged();


};

#endif // TAB_NS_MONITOR_H
