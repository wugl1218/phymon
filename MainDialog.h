#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QStackedLayout>
#include <QTimer>
#include <QAbstractButton>
#include "BarcodeDaemon.h"
#include "Dialog_topalarm_mute.h"
#include "Manager_Patient.h"
#include "Manager_Device.h"
#include "Common.h"
#include "ui_MainDialog.h"
#include "Tab_Domaininput.h"
#include "exporter.h"
#include "Manager_Topalarm.h"
#include "Manager_MDSConnectivity.h"
#include "NetworkManager.h"
#include <QSoundEffect>
#include "Dialog_network.h"
#include "Thread_network.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainDialog; }
QT_END_NAMESPACE

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();

    Ui::MainDialog *ui;
    Manager_Patient pm;
    Manager_Device dm;
    Tab_Domaininput di;
    Exporter exporter;
    Manager_Topalarm ta;
    Manager_MDSConnectivity mdsm;
    Tab_NS_Monitor monitor;
    //NetworkManager nm;
        Dialog_topalarm_mute mute;
    Dialog_network nd;
    Thread_network* network;
    //topalarm
    QSoundEffect *efx;

    void outPutInfo(QString info);
    void startThread();
private slots:
    void updateCurrentTimeDisplay();
    void mainWorkerUpdate();
    void on_tab_switched(int i);
    void db_clean();
    void on_Xbtn_clicked();

    
protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    QStackedLayout *mainStackedLayout;
    QString dialogTitle = "Monitoring Station -- %1";
    QTimer *currentTimer;
    QTimer *mainWorker;
    QTimer *db_cleaner;
    BarcodeDaemon bd;
    Common common;
};
#endif // MAINDIALOG_H
