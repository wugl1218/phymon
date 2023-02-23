#ifndef DIALOG_SELECT_MENU_H
#define DIALOG_SELECT_MENU_H

#include <QDialog>

namespace Ui {
class Dialog_select_menu;
}
class btn{
public:
    std::string name;
    int index;
    bool is_select;

};
class Dialog_select_menu : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_select_menu(QWidget *parent = nullptr);
    ~Dialog_select_menu();
    //std::string為name bool填入是否選中
    std::vector<QPushButton*> btns;
    int Btn_spacing;
    int Btn_width;
    int Btn_height;
    void make_btn(std::vector<btn> btn);
    int select_btn;
    int get_btn(){return select_btn;};

private slots:
    void btn_clicked();

private:
    Ui::Dialog_select_menu *ui;
signals:
    void clicked();
};

#endif // DIALOG_SELECT_MENU_H
