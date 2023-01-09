#ifndef DIALOG_RELEASE_H
#define DIALOG_RELEASE_H

#include <QDialog>

namespace Ui {
class ReleaseDialog;
}

class ReleaseDialog : public QDialog
{
    Q_OBJECT

public:
    uint8_t ok;
    explicit ReleaseDialog(QWidget *parent = nullptr);
    ~ReleaseDialog();
    std::string getText();
    void clearText();

private slots:
    void on_cancel_btn_clicked();
    void on_ok_btn_clicked();

private:
    Ui::ReleaseDialog *ui;

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // DIALOG_RELEASE_H
