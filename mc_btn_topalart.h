#ifndef MC_BTN_TOPALART_H
#define MC_BTN_TOPALART_H

#include <QLabel>
#include <QWidget>
#include "Dialog_topalarm_mute.h"
#include "Common.h"

class mc_btn_topalart : public QLabel
{
    Q_OBJECT
public:
    explicit mc_btn_topalart(QWidget *parent = nullptr);
    bool is_patient_alarm;
    std::string alarm_description;
    std::string alarm_priority;
    std::string alarm_code;
    std::string alarm_no;
    std::string channel_id;
    std::string model;
    std::string alarm_state;
    uint64_t sec;
    uint64_t nanosec;
    Dialog_topalarm_mute mute;
    int mutetime;

signals:
    void clicked();
    void pressed();
    void released();

protected:
    void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);


};


#endif // MC_BTN_TOPALART_H
