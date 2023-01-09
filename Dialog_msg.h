#ifndef DIALOG_MSG_H
#define DIALOG_MSG_H

#include <QDialog>

namespace Ui {
class MsgDialog;
}

class MsgDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MsgDialog(QWidget *parent = nullptr);
    ~MsgDialog();
    void setText(QString qstr);

private slots:
    void on_ConfirmButton_clicked();

private:
    Ui::MsgDialog *ui;
};

#endif // DIALOG_MSG_H
