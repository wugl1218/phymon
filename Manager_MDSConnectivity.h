#ifndef MANAGER_MDSCONNECTIVITY_H
#define MANAGER_MDSCONNECTIVITY_H

#include "Common.h"

class MDSConnectivity
{
public:
    std::string vmd_id;
    std::string patient_id;
    std::string room_id;
    std::string bed_id;
//    uint64_t source_timestamp;
};
class Alarm
{
public:
    std::string alarm_description;
    std::string patient_id;
    std::string alarm_priority;
    std::string alarm_code;
    std::string alarm_no;
    std::string channel_id;
    std::string vmd_id;
    std::string model;
    std::string alarm_state;
    uint64_t sec;
    uint64_t nanosec;
};

class Manager_MDSConnectivity
{
public:
    dds::sub::cond::ReadCondition* cond;
    uint32_t last_query_time;
    uint32_t last_query_time1;
    std::map<std::string, MDSConnectivity> mdsconnectivity;
    QList<Alarm> patient_alarm;
    QList<Alarm> technical_alarm;
    void init();
    void step();
    int time_running =0;
};


#endif // MANAGER_MDSCONNECTIVITY_H

