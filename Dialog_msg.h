#ifndef DIALOG_MSG_H
#define DIALOG_MSG_H

#include <QDialog>

namespace Ui {
class Dialog_msg;
}

class Dialog_msg : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_msg(QWidget *parent = nullptr);
    ~Dialog_msg();
    void setText(QString qstr);

private slots:
    void on_ConfirmButton_clicked();

private:
    Ui::Dialog_msg *ui;
};

#endif // DIALOG_MSG_H
