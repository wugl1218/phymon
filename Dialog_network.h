#ifndef DIALOG_NETWORK_H
#define DIALOG_NETWORK_H

#include <QDialog>

namespace Ui {
class NetworkDialog;
}

class NetworkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkDialog(QWidget *parent = nullptr);
    ~NetworkDialog();
    Ui::NetworkDialog *ui;
    bool is_server ;


private:
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_ConfirmButton_clicked();
};

#endif // DIALOG_NETWORK_H
