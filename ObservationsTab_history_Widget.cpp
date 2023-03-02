#include "ObservationsTab_history_Widget.h"
#include "ui_ObservationsTab_history_Widget.h"

ObservationsTab_history_Widget::ObservationsTab_history_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ObservationsTab_history_Widget)
{
    ui->setupUi(this);
}

ObservationsTab_history_Widget::~ObservationsTab_history_Widget()
{
    delete ui;
}

void ObservationsTab_history_Widget::on_return_pushButton_clicked()
{
    emit changeToRealtimePage();
}

