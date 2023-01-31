#ifndef mc_visualization_H
#define mc_visualization_H

#include <QWidget>

class Visualizetion
{
public:
    float highest;
    float higher;
    float lower;
    float lowest;
    QString device;
    QString dataname;
//    uint64_t source_timestamp;
};
class mc_visualization : public QWidget
{
    Q_OBJECT

public:
    explicit mc_visualization(QWidget *parent = nullptr);
    void set_modle_count(int count);
    QList <Visualizetion> visualizetionlist;

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

#endif // mc_visualization_H
