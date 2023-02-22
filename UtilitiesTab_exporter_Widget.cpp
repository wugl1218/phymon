#include "UtilitiesTab_exporter_Widget.h"
#include "ui_UtilitiesTab_exporter_Widget.h"

UtilitiesTab_exporter_Widget::UtilitiesTab_exporter_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UtilitiesTab_exporter_Widget)
{
    ui->setupUi(this);
}

UtilitiesTab_exporter_Widget::~UtilitiesTab_exporter_Widget()
{
    delete ui;
}
