#include <QCheckBox>
#include "MetricItemsDisplayConfig.h"
#include "ui_MetricItemsDisplayConfig.h"

MetricItemsDisplayConfigForm::MetricItemsDisplayConfigForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MetricItemsDisplayConfigForm)
{
    ui->setupUi(this);
    //
    QTableWidgetItem *chkboxItem = new QTableWidgetItem();
    chkboxItem->setCheckState(Qt::Unchecked);
    chkboxItem->setFlags(Qt::NoItemFlags);
    QTableWidgetItem *textItem = new QTableWidgetItem();
    chkboxItem->setFlags(Qt::NoItemFlags);
    //初始化device list tableWidget的Cell屬性及值
    {
    deviceListStruct devices[] = {
        {true, "Savina", "呼吸機"},
        {false, "MP40", "多功能生理監測儀"},
    };
    int rowCount = sizeof(devices)/sizeof(deviceListStruct);
    QTableWidget *tableWidget = ui->devices_tableWidget;
    tableWidget->setRowCount(rowCount);
    for(int row=0; row<rowCount; row++)
    {
        QTableWidgetItem* _chkboxItem = new QTableWidgetItem(*chkboxItem);
        QTableWidgetItem* _textItem1 = new QTableWidgetItem(*textItem);
        QTableWidgetItem* _textItem2 = new QTableWidgetItem(*textItem);
        _chkboxItem->setCheckState(devices[row].visible ? Qt::Checked : Qt::Unchecked);
        _textItem1->setText(devices[row].name);
        _textItem2->setText(devices[row].description);
        tableWidget->setItem(row, 0, _chkboxItem);
        tableWidget->setItem(row, 1, _textItem1);
        tableWidget->setItem(row, 2, _textItem2);
    }
    }
    //初始化savina metric tableWidget的Cell屬性及值
    {
    deviceMetricStruct savinaMetrics[] = {
        {true,  "Type1", "Test Metric"},
        {true,  "Type1", "Test Metric"},
        {false, "Type3", "Test Metric"},
    };
    int rowCount = sizeof(savinaMetrics)/sizeof(deviceMetricStruct);
    QTableWidget *tableWidget = ui->savina_tableWidget;
    tableWidget->setRowCount(rowCount);
    for(int row=0; row<rowCount; row++)
    {
        QTableWidgetItem* _chkboxItem = new QTableWidgetItem(*chkboxItem);
        QTableWidgetItem* _textItem1 = new QTableWidgetItem(*textItem);
        QTableWidgetItem* _textItem2 = new QTableWidgetItem(*textItem);
        _chkboxItem->setCheckState(savinaMetrics[row].visible ? Qt::Checked : Qt::Unchecked);
        _textItem1->setText(savinaMetrics[row].type);
        _textItem2->setText(savinaMetrics[row].description);
        tableWidget->setItem(row, 0, _chkboxItem);
        tableWidget->setItem(row, 1, _textItem1);
        tableWidget->setItem(row, 2, _textItem2);
    }
    }
    //初始化MP40 metric tableWidget的Cell屬性及值
    {
    deviceMetricStruct mp40Metrics[] = {
        {false,  "Type2", "Test Metric"},
    };
    int rowCount = sizeof(mp40Metrics)/sizeof(deviceMetricStruct);
    QTableWidget *tableWidget = ui->mp40_tableWidget;
    tableWidget->setRowCount(rowCount);
    for(int row=0; row<rowCount; row++)
    {
        QTableWidgetItem* _chkboxItem = new QTableWidgetItem(*chkboxItem);
        QTableWidgetItem* _textItem1 = new QTableWidgetItem(*textItem);
        QTableWidgetItem* _textItem2 = new QTableWidgetItem(*textItem);
        _chkboxItem->setCheckState(mp40Metrics[row].visible ? Qt::Checked : Qt::Unchecked);
        _textItem1->setText(mp40Metrics[row].type);
        _textItem2->setText(mp40Metrics[row].description);
        tableWidget->setItem(row, 0, _chkboxItem);
        tableWidget->setItem(row, 1, _textItem1);
        tableWidget->setItem(row, 2, _textItem2);
    }
    }
    delete chkboxItem;
    delete textItem;
    //同步生理監測項目顯示設定區的各頁簽初始可見狀態
    QTableWidget *devicesTable = ui->devices_tableWidget;
    for(int row=0; row < devicesTable->rowCount(); row++)
    {
        _toggleDeviceTabVisible(row);
    }
    //
    ui->devices_tableWidget->setColumnWidth(0, 50);
    ui->devices_tableWidget->setColumnWidth(1, 100);
    //ui->deviceTable_tableWidget->setColumnWidth(2, 150);
    //
    connect(ui->devices_tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(toggleEnableItemStatus(int,int)));
    connect(ui->savina_tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(toggleEnableItemStatus(int,int)));
    connect(ui->mp40_tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(toggleEnableItemStatus(int,int)));
    //
    connect(ui->devices_tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(toggleDeviceTabVisible(int,int)));
}

MetricItemsDisplayConfigForm::~MetricItemsDisplayConfigForm()
{
    delete ui;
}

void MetricItemsDisplayConfigForm::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    //backup checkbox status
    QTableWidget *tableWidget = ui->devices_tableWidget;
    //device list tablewidget
    for(int row=0; row<tableWidget->rowCount(); row++)
    {
        QTableWidgetItem* item = tableWidget->item(row, 0);
        item->setStatusTip(item->checkState()?"1":"0");
    }
    //savina tablewidget
    tableWidget = ui->savina_tableWidget;
    //backup checkbox status
    for(int row=0; row<tableWidget->rowCount(); row++)
    {
        QTableWidgetItem* item = tableWidget->item(row, 0);
        item->setStatusTip(item->checkState()?"1":"0");
    }
    //MP40 tablewidget
    tableWidget = ui->mp40_tableWidget;
    //backup checkbox status
    for(int row=0; row<tableWidget->rowCount(); row++)
    {
        QTableWidgetItem* item = tableWidget->item(row, 0);
        item->setStatusTip(item->checkState()?"1":"0");
    }
}

void MetricItemsDisplayConfigForm::toggleEnableItemStatus(int row,int column)
{
    if(column != 0) return; //只處理column 0的觸發事件
    QTableWidgetItem *enableItem = static_cast<QTableWidget*>(this->sender())->item(row, 0);
    if(enableItem->checkState() == Qt::Unchecked)
        enableItem->setCheckState(Qt::Checked);
    else
        enableItem->setCheckState(Qt::Unchecked);
}

void MetricItemsDisplayConfigForm::toggleDeviceTabVisible(int row,int column)
{
    if(column != 0) return; //只處理column 0的觸發事件
    if(this->sender() != ui->devices_tableWidget) return; //只處理devices_tableWidget的觸發事件
    _toggleDeviceTabVisible(row);
}

void MetricItemsDisplayConfigForm::_toggleDeviceTabVisible(int row)
{
    QTableWidget *devicesTable = ui->devices_tableWidget;
    QTableWidgetItem *visibleItem = devicesTable->item(row, 0);
    QTableWidgetItem *nameItem = devicesTable->item(row, 1);
    QTabWidget *tabWidget = ui->deviceMetric_tabWidget;
    bool visible = visibleItem->checkState() == Qt::Checked ? true : false;
    int index = -1;
    if     (nameItem->text() == "Savina") index = tabWidget->indexOf(ui->savina_tab);
    else if(nameItem->text() == "MP40")   index = tabWidget->indexOf(ui->mp40_tab);
    tabWidget->setTabVisible(index, visible);
    if(visible) tabWidget->setCurrentIndex(index);
}

void MetricItemsDisplayConfigForm::on_close_pushButton_clicked()
{
    emit backToMainWidget();
}


void MetricItemsDisplayConfigForm::on_restore_pushButton_clicked()
{
    //restore checkbox status
    QTableWidget *tableWidget = ui->devices_tableWidget;
    //device list tablewidget
    for(int row=0; row<tableWidget->rowCount(); row++)
    {
        QTableWidgetItem* item = tableWidget->item(row, 0);
        item->setCheckState((item->statusTip() == "1") ? Qt::Checked : Qt::Unchecked);
    }
    //savina tablewidget
    tableWidget = ui->savina_tableWidget;
    //backup checkbox status
    for(int row=0; row<tableWidget->rowCount(); row++)
    {
        QTableWidgetItem* item = tableWidget->item(row, 0);
        item->setCheckState((item->statusTip() == "1") ? Qt::Checked : Qt::Unchecked);
    }
    //MP40 tablewidget
    tableWidget = ui->mp40_tableWidget;
    //backup checkbox status
    for(int row=0; row<tableWidget->rowCount(); row++)
    {
        QTableWidgetItem* item = tableWidget->item(row, 0);
        item->setCheckState((item->statusTip() == "1") ? Qt::Checked : Qt::Unchecked);
    }
    //同步生理監測項目顯示設定區的各頁簽初始可見狀態
    QTableWidget *devicesTable = ui->devices_tableWidget;
    for(int row=0; row < devicesTable->rowCount(); row++)
    {
        _toggleDeviceTabVisible(row);
    }
}

