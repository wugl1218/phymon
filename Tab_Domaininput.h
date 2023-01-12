#ifndef TAB_DOMAININPUT_H
#define TAB_DOMAININPUT_H

#include <QDialog>

namespace Ui {
class Tab_Domaininput;
}

class Tab_Domaininput : public QDialog
{
    Q_OBJECT

public:
    explicit Tab_Domaininput(QWidget *parent = nullptr);
    ~Tab_Domaininput();
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
    Ui::Tab_Domaininput *ui;
};

#endif // TAB_DOMAININPUT_H
