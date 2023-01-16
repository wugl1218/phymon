#ifndef MC_VISUALIZETION_H
#define MC_VISUALIZETION_H

#include <QWidget>

class Visualizetion
{
public:
    float count;
    float highest;
    float higher;
    float lower;
    float lowest;
    QString datename;
//    uint64_t source_timestamp;
};
class mc_visualizetion : public QWidget
{
    Q_OBJECT

public:
    explicit mc_visualizetion(QWidget *parent = nullptr);
    void set_modle_count(int count);
    QList <Visualizetion> visualizetion;

private:
    int modle_count=0;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent* event)override;

signals:
    void clicked();
    void pressed();
    void released();
};

#endif // MC_VISUALIZETION_H
