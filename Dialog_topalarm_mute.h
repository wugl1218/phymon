#ifndef DIALOG_TOPALARM_MUTE_H
#define DIALOG_TOPALARM_MUTE_H

#include <QDialog>

namespace Ui {
class Dialog_topalarm_mute;
}

class Dialog_topalarm_mute : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_topalarm_mute(QWidget *parent = nullptr);
    ~Dialog_topalarm_mute();
    int mutetime;
    int get_mutetime();

private slots:
    void on_mutebtn1_clicked();

    void on_mutebtn2_clicked();

    void on_mutebtn3_clicked();

    void on_mutebtn4_clicked();

private:
    Ui::Dialog_topalarm_mute *ui;
};

#endif // DIALOG_TOPALARM_MUTE_H
