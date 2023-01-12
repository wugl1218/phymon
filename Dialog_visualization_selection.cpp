#include "Dialog_visualization_selection.h"
#include "ui_Dialog_visualization_selection.h"
#include <QButtonGroup>

Dialog_visualization_selection::Dialog_visualization_selection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_visualization_selection)
{
    ui->setupUi(this);

}

Dialog_visualization_selection::~Dialog_visualization_selection()
{
    delete ui;
}
