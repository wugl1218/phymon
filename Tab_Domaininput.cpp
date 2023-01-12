#include "Tab_Domaininput.h"
#include "ui_Tab_Domaininput.h"

Tab_Domaininput::Tab_Domaininput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Tab_Domaininput)
{
    ui->setupUi(this);
    domain_id = -1;
}

Tab_Domaininput::~Tab_Domaininput()
{
    delete ui;
}

void Tab_Domaininput::on_b7_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("7");
}


void Tab_Domaininput::on_b8_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("8");
}


void Tab_Domaininput::on_b9_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("9");
}


void Tab_Domaininput::on_b4_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("4");
}


void Tab_Domaininput::on_b5_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("5");
}


void Tab_Domaininput::on_b6_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("6");
}


void Tab_Domaininput::on_b1_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("1");
}


void Tab_Domaininput::on_b2_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("2");
}


void Tab_Domaininput::on_b3_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("3");
}


void Tab_Domaininput::on_bback_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->backspace();
}


void Tab_Domaininput::on_b0_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("0");
}


void Tab_Domaininput::on_bok_clicked()
{
    if(ui->domainLineEdit->text().size() == 0)
        return;
    domain_id = ui->domainLineEdit->text().toInt();
    close();
}

