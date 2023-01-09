#include "ObservationsTab_realtime_Widget.h"
#include "ui_ObservationsTab_realtime_Widget.h"

ObservationsTab_realtime_Widget::ObservationsTab_realtime_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ObservationsTab_realtime_Widget)
{
    ui->setupUi(this);
    //
    trendViewList = { ui->trendView1_scrollArea, ui->trendView2_scrollArea, ui->trendView3_scrollArea, ui->trendView4_scrollArea, ui->trendView5_scrollArea };
    trendViewBtnList = { ui->trendView1_pushButton, ui->trendView2_pushButton, ui->trendView3_pushButton, ui->trendView4_pushButton, ui->trendView5_pushButton };
    //連接trend button顯示事件
    connect(ui->trendView_buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(on_trendView_pushButton_clicked(QAbstractButton*)));
    //隱藏trendView scrollArea
    for(QScrollArea* widget : trendViewList)
    {
        widget->hide();
        ui->left_verticalLayout->removeWidget(widget);
    }
    connect(ui->realtimeDataDisplay_textEdit, SIGNAL(openMetricItemDisplayConfigForm()), this, SLOT(changeToMetricItemsDisplayConfig()));
    QString labelText;
    for(int i=0; i<999; i++)
    {
        labelText += QString::asprintf("%03d,", i);
    }
    QList<QLabel*> scrollAreaLabelList = {ui->scrollarea1_label, ui->scrollarea2_label, ui->scrollarea3_label, ui->scrollarea4_label, ui->scrollarea5_label};
    for(QLabel* label : scrollAreaLabelList)
    {
        label->setText(labelText);
    }
}

ObservationsTab_realtime_Widget::~ObservationsTab_realtime_Widget()
{
    delete ui;
}

void ObservationsTab_realtime_Widget::on_trendView_pushButton_clicked(QAbstractButton* button)
{
    qsizetype idx = trendViewBtnList.indexOf(button);
    QScrollArea* trendView = trendViewList[idx];
    ui->left_verticalLayout->addWidget(trendView);
    trendView->show();
//    qDebug() << ui->realtimeTrendView_widget->width() << "," << ui->realtimeTrendView_widget->height();
}
