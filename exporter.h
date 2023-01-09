#ifndef EXPORTER_H
#define EXPORTER_H
#include "Common.h"

class Exporter
{
public:
    uint8_t exporting;
    std::string filepath;
    uint32_t interval;
    uint32_t last_time;
    dds::sub::cond::QueryCondition* qcond;

    void init();
    void step();
    void start_exporting(std::string path, uint32_t interval_ms);
    void stop_exporting();
};

#endif // EXPORTER_H
