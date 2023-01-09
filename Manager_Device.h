#ifndef MANAGER_DEVICE_H
#define MANAGER_DEVICE_H

#include "Common.h"

class DeviceManager
{
public:
    dds::sub::cond::QueryCondition* cond;
    uint32_t last_query_time;
    std::map<std::string, Device> devices;

    void init();
    void step();
    bool is_DeviceLose =0;

};

#endif // MANAGER_DEVICE_H
