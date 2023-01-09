#ifndef MANAGER_TOPALARM_H
#define MANAGER_TOPALARM_H

#include <map>
#include "Common.h"

class TOPAlarm
{
public:
    std::string alarm_description;
    std::string patient_id;
    std::string alarm_priority;
    std::string alarm_code;
    std::string alarm_no;
    std::string channel_id;
    std::string vmd_id;
    std::string alarm_state;
    uint64_t sec;
    uint64_t nanosec;
};
class TopAlarmManager
{
public:
    uint32_t last_query_time;
    QList<TOPAlarm> top_patient_alarm;
    QList<TOPAlarm> top_technical_alarm;
    void init();
    void step();

};

#endif // MANAGER_TOPALARM_H
