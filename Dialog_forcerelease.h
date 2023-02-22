#ifndef DIALOG_FORCERELEASE_H
#define DIALOG_FORCERELEASE_H

#include <QDialog>

namespace Ui {
class Dialog_forcerelease;
}

class Dialog_forcerelease : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_forcerelease(QWidget *parent = nullptr);
    ~Dialog_forcerelease();
    std::string getText();
    void clearText();
    Ui::Dialog_forcerelease *ui;


private slots:
    void on_pushButton_clicked();


protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // DIALOG_FORCERELEASE_H
