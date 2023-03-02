#ifndef MC_SELECTION_ENTRY_H
#define MC_SELECTION_ENTRY_H

#include <QWidget>

#define MC_ENTRY_LEFT 0
#define MC_ENTRY_RIGHT 1
#define MC_ENTRY_STATIC 2

class mc_selection_entry : public QWidget
{
    Q_OBJECT
public:
    int table_index;
    int tab_index;

    explicit mc_selection_entry(QWidget *parent = nullptr);
    void set_type(int type);
    int get_type();
    void set_text(const char* text);
    std::string get_text();
    void set_margin_x(int margin);
    void set_margin_y(int margin);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void button_press(int type);

private:
    int type;
    std::string text;
    int margin_x;
    int margin_y;
};

#endif // MC_SELECTION_ENTRY_H
