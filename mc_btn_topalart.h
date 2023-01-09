#ifndef MC_BTN_TOPALART_H
#define MC_BTN_TOPALART_H

#include <QAbstractItemModel>
#include <QLabel>

class mc_btn_topalart : public QLabel
{
    Q_OBJECT

public:
    explicit mc_btn_topalart(QWidget *parent = nullptr);


protected:
    void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
signals:
    void on_click();

};


#endif // MC_BTN_TOPALART_H
