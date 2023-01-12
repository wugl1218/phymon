#ifndef DIALOG_NETWORK_H
#define DIALOG_NETWORK_H

#include <QDialog>

namespace Ui {
class Dialog_network;
}

class Dialog_network : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_network(QWidget *parent = nullptr);
    ~Dialog_network();
    Ui::Dialog_network *ui;
    bool is_server=0 ;


private:
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_ConfirmButton_clicked();
};

#endif // DIALOG_NETWORK_H
