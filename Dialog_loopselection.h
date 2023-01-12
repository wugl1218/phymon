#ifndef DIALOG_LOOPSELECTION_H
#define DIALOG_LOOPSELECTION_H

#include <QDialog>

#define LOOP_PRESSURE_VOLUME    0
#define LOOP_VOLUME_FLOW        1
#define LOOP_FLOW_PRESSURE      2
#define LOOP_PTRACH_VOLUME      3
#define LOOP_FLOW_PTRACH        4

namespace Ui {
class Dialog_loopselection;
}

class Dialog_loopselection : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_loopselection(QWidget *parent = nullptr);
    ~Dialog_loopselection();
    void set_current_loops(int loop1_type, int loop2_type);
    void get_current_loops(int* loop1_type, int* loop2_type);

private slots:
    void on_loop1btn1_clicked();

    void on_loop1btn2_clicked();

    void on_loop1btn3_clicked();

    void on_loop2btn1_clicked();

    void on_loop2btn2_clicked();

    void on_loop2btn3_clicked();

    void on_loop1btn4_clicked();

    void on_loop2btn4_clicked();

    void on_loop1btn5_clicked();

    void on_loop2btn5_clicked();

private:
    Ui::Dialog_loopselection *ui;
    int loop1_type;
    int loop2_type;
    void set_checked(QWidget* w, uint8_t checked);

protected:
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // DIALOG_LOOPSELECTION_H
