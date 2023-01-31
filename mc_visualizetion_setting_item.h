#ifndef MC_VISUALIZETION_SETTING_ITEM_H
#define MC_VISUALIZETION_SETTING_ITEM_H

#include <QWidget>

class mc_visualizetion_setting_item : public QWidget
{
public:
    int table_index;
    int tab_index;

    explicit mc_visualizetion_setting_item(QWidget *parent = nullptr);
 /*   void set_type(int type);
    int get_type();
    void set_text(const char* text);
    std::string get_text();
    void set_margin_x(int margin);
    void set_margin_y(int margin);*/

protected:
    void paintEvent(QPaintEvent *event) override;
//    void mousePressEvent(QMouseEvent *event) override;

signals:
    void pressed(int type);
private:
    int type;
    std::string text;
    int margin_x;
    int margin_y;
};


#endif // MC_VISUALIZETION_SETTING_ITEM_H
