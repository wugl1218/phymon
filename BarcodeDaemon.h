#ifndef BARCODEDAEMON_H
#define BARCODEDAEMON_H

#include <stdint.h>
#include <dds/pub/ddspub.hpp>
#include <dds/core/ddscore.hpp>

class MainDialog;

class BarcodeDaemon
{
public:
    MainDialog* app;
    uint8_t running;
    char m[512];
    std::string last_devpath;
    std::string devpath;
    std::string current_text;
    uint32_t last_devpath_time;
    uint32_t last_read_time;
    uint32_t last_alert_time;
    int fd;
    uint8_t active;

    void init(MainDialog* app);
    void step();
};

#endif // BARCODEDAEMON_H
