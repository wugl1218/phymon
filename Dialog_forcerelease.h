#ifndef DIALOG_FORCERELEASE_H
#define DIALOG_FORCERELEASE_H

#include <QDialog>

namespace Ui {
class ForceReleaseDialog;
}

class ForceReleaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ForceReleaseDialog(QWidget *parent = nullptr);
    ~ForceReleaseDialog();
    std::string getText();
    void clearText();
    Ui::ForceReleaseDialog *ui;


private slots:
    void on_pushButton_clicked();


protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // DIALOG_FORCERELEASE_H
