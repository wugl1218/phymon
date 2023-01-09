#ifndef TAB_DOMAININPUT_H
#define TAB_DOMAININPUT_H

#include <QDialog>

namespace Ui {
class DomainInput;
}

class DomainInput : public QDialog
{
    Q_OBJECT

public:
    explicit DomainInput(QWidget *parent = nullptr);
    ~DomainInput();
    int domain_id;

private slots:
    void on_b7_clicked();

    void on_b8_clicked();

    void on_b9_clicked();

    void on_b4_clicked();

    void on_b5_clicked();

    void on_b6_clicked();

    void on_b1_clicked();

    void on_b2_clicked();

    void on_b3_clicked();

    void on_bback_clicked();

    void on_b0_clicked();

    void on_bok_clicked();

private:
    Ui::DomainInput *ui;
};

#endif // TAB_DOMAININPUT_H
