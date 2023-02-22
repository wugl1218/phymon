#ifndef DIALOG_RELEASE_H
#define DIALOG_RELEASE_H

#include <QDialog>

namespace Ui {
class Dialog_release;
}

class Dialog_release : public QDialog
{
    Q_OBJECT

public:
    uint8_t ok;
    explicit Dialog_release(QWidget *parent = nullptr);
    ~Dialog_release();
    std::string getText();
    void clearText();

private slots:
    void on_cancel_btn_clicked();
    void on_ok_btn_clicked();

private:
    Ui::Dialog_release *ui;

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // DIALOG_RELEASE_H
