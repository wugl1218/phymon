#include "Tab_Domaininput.h"
#include "ui_Tab_Domaininput.h"

DomainInput::DomainInput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DomainInput)
{
    ui->setupUi(this);
    domain_id = -1;
}

DomainInput::~DomainInput()
{
    delete ui;
}

void DomainInput::on_b7_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("7");
}


void DomainInput::on_b8_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("8");
}


void DomainInput::on_b9_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("9");
}


void DomainInput::on_b4_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("4");
}


void DomainInput::on_b5_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("5");
}


void DomainInput::on_b6_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("6");
}


void DomainInput::on_b1_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("1");
}


void DomainInput::on_b2_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("2");
}


void DomainInput::on_b3_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("3");
}


void DomainInput::on_bback_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->backspace();
}


void DomainInput::on_b0_clicked()
{
    ui->domainLineEdit->setCursorPosition(ui->domainLineEdit->text().size());
    ui->domainLineEdit->insert("0");
}


void DomainInput::on_bok_clicked()
{
    if(ui->domainLineEdit->text().size() == 0)
        return;
    domain_id = ui->domainLineEdit->text().toInt();
    close();
}

