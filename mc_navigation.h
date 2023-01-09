#ifndef MC_NAVIGATION_H
#define MC_NAVIGATION_H

#include <QWidget>

class mc_navigation : public QWidget
{
    Q_OBJECT
public:
    mc_navigation(QWidget *parent = nullptr);
    void set_current_tab(int i);
    int get_current_tab();
    void set_enabled(uint8_t enabled);
    uint8_t is_enabled();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void on_tab_switched(int i);

private:
    int current_tab;
    uint8_t enabled;
};

#endif // MC_NAVIGATION_H
