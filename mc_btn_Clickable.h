#ifndef MC_BTN_CLICKABLE_H
#define MC_BTN_CLICKABLE_H

#include <QLabel>
#include <QWidget>

class mc_btn_Clickable : public QLabel
{
    Q_OBJECT
public:
    mc_btn_Clickable(QWidget *parent = nullptr);

signals:
    void clicked();
    void pressed();
    void released();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
};

#endif // MC_BTN_CLICKABLE_H
