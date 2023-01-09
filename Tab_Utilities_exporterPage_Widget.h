#ifndef TAB_UTILITIES_EXPORTERPAGE_WIDGET_H
#define TAB_UTILITIES_EXPORTERPAGE_WIDGET_H

#include <QWidget>
#include <Dialog_jsontext.h>


namespace Ui {
class Tab_Utilities_exporterPage_Widget;
}

class Tab_Utilities_exporterPage_Widget : public QWidget
{
    Q_OBJECT

public:
    Ui::Tab_Utilities_exporterPage_Widget *ui;
    explicit Tab_Utilities_exporterPage_Widget(QWidget *parent = nullptr);
    ~Tab_Utilities_exporterPage_Widget();
    uint32_t get_interval();
    uint8_t index_u8;//philo
    void handle_stop();
    void log(const char* line);
    void clear_log();
    jsontext jt;

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void mousePressEvent(QMouseEvent *event);


private slots:
    void on_execute_pushButton_clicked();

    void on_stop_pushButton_clicked();

    void on_send_pushButton_clicked();

    void on_Frequency_comboBox_currentIndexChanged(int index);

private:
};

#endif // TAB_UTILITIES_EXPORTERPAGE_WIDGET_H
