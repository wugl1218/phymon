#ifndef MANAGER_TOPALARM_H
#define MANAGER_TOPALARM_H

#include <map>
#include "Common.h"
class mc_btn_topalart;
class TOPAlarm
{
public:
    std::string alarm_description;
    std::string patient_id;
    std::string alarm_priority;
    std::string alarm_code;
    std::string alarm_no;
    std::string channel_id;
    std::string model;
    std::string vmd_id;
    std::string alarm_state;
    uint64_t sec;
    uint64_t nanosec;
};

class Manager_Topalarm
{
public:
    uint32_t last_query_time;
    std::multimap<int,TOPAlarm,std::greater<int>> top_patient_alarm;
    std::multimap<int,TOPAlarm,std::greater<int>> top_technical_alarm;
    void init();
    void step();
    void topalarm(mc_btn_topalart *label ,Device devices);
    void UI_set(mc_btn_topalart *label ,Device devices);
    QList <mc_btn_topalart *> UI_name;
    bool manager_is_mute=1;

};

#endif // MANAGER_TOPALARM_H
