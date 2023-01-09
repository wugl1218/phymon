#ifndef MC_VISUALIZETION_H
#define MC_VISUALIZETION_H

#include <QWidget>

class mc_visualizetion : public QWidget
{
    Q_OBJECT

public:
    explicit mc_visualizetion(QWidget *parent = nullptr);
    void set_modle_count(int count);
private:
protected:
    void paintEvent(QPaintEvent *event) override;
    int modle_count=0;

};

#endif // MC_VISUALIZETION_H
