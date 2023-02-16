#include "MainDialog.h"
#include "ui_MainDialog.h"
#include <QInputDialog>
#include "Common.h"
#include <QCloseEvent>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollBar>
#include <QTextStream>
#include "Tab_Observations_historyPage_Widget.h"
#include <QThread>
static bool mc_filter(cbl::Document document, CBLDocumentFlags flags)
{
    if (flags == kCBLDocumentFlagsDeleted || flags == kCBLDocumentFlagsAccessRemoved)
        return false;

    if ((document.properties().asDict().get("data_source").asstring() == "Observation" ||
         document.properties().asDict().get("data_source").asstring() == "RTObservation") &&
        (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() -
         document.properties().get("source_timestamp").asDict().get("sec").asUnsigned()) < 48 * 60 * 60)
    {
        return true;
    }
    return false;
}

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainDialog)
    , nd(this)
    , mute(this)
    , common(this)
{
    ui->setupUi(this);
    connect(ui->Xbtn, SIGNAL(clicked()), this, SLOT(on_Xbtn_clicked()));
    efx = new QSoundEffect(this);
    efx->setSource(QUrl::fromLocalFile(":/wav/warning.wav"));
    efx->setVolume(1.0);            // 0.0 ~ 1.0
    efx->setLoopCount(1);
    if(!common.is_server)
    {
        ui->main_stackedWidget->setCurrentIndex(1);
        ui->mapping_tab->ui->returnbtn->hide();
        ui->Xbtn->hide();
    }
    else
        ui->main_stackedWidget->setCurrentIndex(0);
    if(common.domain_id == -1)
    {

        di.setWindowFlags(Qt::FramelessWindowHint);
        di.setWindowState(Qt::WindowFullScreen);
        di.exec();
        common.domain_id = di.domain_id;
        rapidjson::Document d;
        d.SetObject();
        d.AddMember("data_source", "DOMAIN", d.GetAllocator());
        d.AddMember("domain_id", common.domain_id, d.GetAllocator());
        d.AddMember("source_timestamp", time(NULL), d.GetAllocator());
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        d.Accept(writer);
        std::string dummy;
        std::string dummy2;
        common.cbl->saveMutableDocument(common.db, buffer.GetString(), dummy2, dummy);
    }

    std::string dummy;
    if(common.is_server)
    {
        common.observation_puller = common.cbl->getReplicatorBasicPullOnly(common.db, common.vmd_url, common.display_items_uid, common.display_items_pwd, dummy);
        common.observation_puller.start(true);
        common.mapping_tab->ui->patient_list_label->setStyleSheet("color: rgb(61, 56, 70)");
        common.mapping_tab->ui->list_scrollarea->setStyleSheet("background-color: rgb(3, 9, 12)");
        common.mapping_tab->ui->scrollAreaWidgetContents->setStyleSheet("background-color: rgb(3, 9, 12)");
    }

    common.init_dds(common.domain_id);

    currentTimer = new QTimer(this);
    connect(currentTimer, SIGNAL(timeout()), this, SLOT(updateCurrentTimeDisplay()));
    currentTimer->start(1000);
    //資料庫監測
    network = new Thread_network(this);
    network->init();
    network->start();

    bd.init(this);
    pm.init();
    dm.init();
    ta.init();
    mdsm.init();
    ui->monitor_page->init();

    exporter.init();
    mainWorker = new QTimer(this);
    connect(mainWorker, SIGNAL(timeout()), this, SLOT(mainWorkerUpdate()));

    mainWorker->start(16);
    db_cleaner = new QTimer(this);
    connect(db_cleaner, SIGNAL(timeout()), this, SLOT(db_clean()));
    db_cleaner->start(2000);
    connect(ui->nav, SIGNAL(on_tab_switched(int)), this, SLOT(on_tab_switched(int)));          
    ui->nav->set_current_tab(0);
    ui->stackedWidget->setCurrentWidget(ui->mapping_tab);
    this->setWindowState(Qt::WindowFullScreen);

    Common::set_inactive_colors(this);
    Common::set_inactive_colors(ui->nav);
    Common::set_inactive_colors(ui->navbar);
    Common::set_inactive_colors(ui->binding_bar);
    Common::set_inactive_colors(ui->binding_layout);
    Common::set_inactive_colors(ui->header_right);
    Common::set_inactive_colors(ui->header_right_bar);

}
MainDialog::~MainDialog()
{
    delete ui;
}

void MainDialog::on_tab_switched(int i)
{
    if(i==0)
        ui->stackedWidget->setCurrentWidget(ui->mapping_tab);
    else if(i==1)
        ui->stackedWidget->setCurrentWidget(ui->utilitiesTab_Widget);
    else if(i==2)
        ui->stackedWidget->setCurrentWidget(ui->devicesTab_Widget);
    else
        ui->stackedWidget->setCurrentWidget(ui->observationsTab_Widget);
}

void MainDialog::keyPressEvent(QKeyEvent *event)
{
    //不處理ESC鍵盤事件。
    switch (event->key())
    {
    case Qt::Key_Escape:
        return;
    case Qt::Key_Space:
        return;
    default:
        break;
    }
    QDialog::keyPressEvent(event);
}

void MainDialog::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    event->ignore();
}

void MainDialog::updateCurrentTimeDisplay()
{
    ui->currentTime_label->setText(QString("%1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
}

void MainDialog::db_clean()
{
    Common* common = Common::instance();
    std::string dummy;
    std::string sql = "SELECT meta().id FROM _ WHERE (data_source='NumericDeviceSelection' OR data_source='NumericVisibility')";
    sql.append(" AND expired=1");
    sql.append(" AND meta().expiration IS NOT VALUED");
    cbl::ResultSet results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
    int error=0;while (dummy!="IP200"&&error<5)
        {
        results = common->cbl->queryDocuments(common->display_items_db, sql, dummy);
        qDebug()<<QString::fromStdString(dummy);
        fflog_out(common->log,dummy.c_str());error++;
        }
    for(auto& result: results)
    {
        std::string id = result.valueAtIndex(0).asstring();
        int64_t now = time(NULL);
        now+=48*60*60;
        now*=1000;
        common->cbl->setDocExpiration(common->display_items_db, id, now, dummy);
    }
}

void MainDialog::mainWorkerUpdate()
{
//    nm.step();
    if(!common.is_server)
    {
        bd.step();
        pm.step();
        dm.step();
        ta.step();
        exporter.step();
        mdsm.step();
        common.history_page->step();
    }
    else if (common.is_server && !network->is_network_error)
    {
        bd.step();
        pm.step();
        dm.step();
        ta.step();
        exporter.step();
        mdsm.step();
        common.history_page->step();
    }

}

void MainDialog::on_Xbtn_clicked()
{
    exit(1);
}
