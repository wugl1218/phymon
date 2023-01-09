#include "Tab_Utilities_exporterPage_Widget.h"
#include "ui_Tab_Utilities_exporterPage_Widget.h"
#include "Common.h"
#include "MainDialog.h"
#include "Tab_Utilities_Widget.h"

Tab_Utilities_exporterPage_Widget::Tab_Utilities_exporterPage_Widget(QWidget *parent) :
    QWidget(parent),
    jt(this),
    ui(new Ui::Tab_Utilities_exporterPage_Widget)
{
    Common* common = Common::instance();
    ui->setupUi(this);
    ui->stop_pushButton->setEnabled(false);
    ui->stop_pushButton->setStyleSheet(common->css.Enabled_ButtonStyle);
    ui->send_pushButton->setEnabled(false);
    ui->send_pushButton->setStyleSheet(common->css.Enabled_ButtonStyle);
    common->exporter_page = this;
}

Tab_Utilities_exporterPage_Widget::~Tab_Utilities_exporterPage_Widget()
{
    delete ui;
}

void Tab_Utilities_exporterPage_Widget::showEvent(QShowEvent *event)
{

}

void Tab_Utilities_exporterPage_Widget::hideEvent(QHideEvent *event)
{

}

void Tab_Utilities_exporterPage_Widget::log(const char* line)
{
    //ui->log_textBrowser->append(line);

    QString str = line;
    str.append("\n");
    str.append(ui->log_textBrowser->toPlainText());
    ui->log_textBrowser->setText(str);
}

void Tab_Utilities_exporterPage_Widget::clear_log()
{
    ui->log_textBrowser->clear();
}

void Tab_Utilities_exporterPage_Widget::on_execute_pushButton_clicked()
{
    Common* common = Common::instance();
    if(common->patient_id.size() == 0)
    {
        common->msg.setText("Unable to execute. No patient bound.");
        common->msg.exec();
        return;
    }
    common->rd.ok = 0;
    common->rd.clearText();
    common->rd.exec();
    if(common->rd.ok == 0)
        return;
    std::string employeeID = common->rd.getText();
    common->commit_user_action(employeeID.c_str(), "Exporter", "Execute");
    ui->stop_pushButton->setEnabled(true);
    ui->stop_pushButton->setStyleSheet(common->css.unEnabled_ButtonStyle);
    ui->send_pushButton->setEnabled(false);
    ui->send_pushButton->setStyleSheet(common->css.Enabled_ButtonStyle);
    ui->execute_pushButton->setEnabled(false);
    ui->execute_pushButton->setStyleSheet(common->css.Enabled_ButtonStyle);
    struct tm t;
    time_t now = time(NULL);
    localtime_r(&now, &t);
    char buf[256];
    sprintf(buf, "%s-%04d%02d%02d%02d%02d%02d.json", common->patient_id.c_str(),
            t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour,
            t.tm_min, t.tm_sec);
    ui->fileName_label->setText(buf);
    std::string path = common->export_path;
    path.append(buf);
    ui->log_textBrowser->clear();
    common->md->exporter.start_exporting(path, get_interval());
}
void Tab_Utilities_exporterPage_Widget::on_Frequency_comboBox_currentIndexChanged(int index)
{
    index_u8 = ui->Frequency_comboBox->currentIndex();
    ui->log_textBrowser->clear();
}

uint32_t Tab_Utilities_exporterPage_Widget::get_interval()
{
    if(index_u8 == 0)
        return 5000;
    else if(index_u8 == 1)
        return 10000;
    else if(index_u8 == 2)
        return 30000;
    else if(index_u8 == 3)
        return 60000;
    else if(index_u8 == 4)
        return 300000;
    else if(index_u8 == 5)
        return 900000;
    else if(index_u8 == 6)
        return 1200000;
}

void Tab_Utilities_exporterPage_Widget::handle_stop()
{
    Common* common = Common::instance();

    ui->execute_pushButton->setEnabled(true);
    ui->execute_pushButton->setStyleSheet(common->css.unEnabled_ButtonStyle);
    ui->stop_pushButton->setEnabled(false);
    ui->stop_pushButton->setStyleSheet(common->css.Enabled_ButtonStyle);
    ui->send_pushButton->setEnabled(true);
    ui->send_pushButton->setStyleSheet(common->css.unEnabled_ButtonStyle);
    common->md->exporter.stop_exporting();
}

void Tab_Utilities_exporterPage_Widget::on_stop_pushButton_clicked()
{
    handle_stop();
}


void Tab_Utilities_exporterPage_Widget::on_send_pushButton_clicked()
{
    Common* common = Common::instance();

    if (ui->fileName_label->text()=="")return;
    ui->execute_pushButton->setEnabled(false);
    ui->execute_pushButton->setStyleSheet(common->css.Enabled_ButtonStyle);
    ui->execute_pushButton->update();
    jt.clearText();
    jt.getText(ui->fileName_label->text());
    jt.exec();
    ui->execute_pushButton->setEnabled(true);
    ui->execute_pushButton->setStyleSheet(common->css.unEnabled_ButtonStyle);

}
void Tab_Utilities_exporterPage_Widget::mousePressEvent(QMouseEvent *event)
{

}

