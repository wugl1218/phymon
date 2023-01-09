#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QWidget>

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    ClickableLabel(QWidget *parent = nullptr);

signals:
    void clicked();
    void pressed();
    void released();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
};

#endif // CLICKABLELABEL_H
