#include "MainDialog.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include "rapidjson/document.h"
#include <time.h>
#include "Common.h"

uint8_t* mc_mapping_active = NULL;

static void mc_build_charmap(char* m)
{
    memset(m, '0', 512);
    m[0x27] = '0';
    m[0x1E] = '1';
    m[0x1F] = '2';
    m[0x20] = '3';
    m[0x21] = '4';
    m[0x22] = '5';
    m[0x23] = '6';
    m[0x24] = '7';
    m[0x25] = '8';
    m[0x26] = '9';
    m[0x04] = 'a';
    m[0x05] = 'b';
    m[0x06] = 'c';
    m[0x07] = 'd';
    m[0x08] = 'e';
    m[0x09] = 'f';
    m[0x0A] = 'g';
    m[0x0B] = 'h';
    m[0x0C] = 'i';
    m[0x0D] = 'j';
    m[0x0E] = 'k';
    m[0x0F] = 'l';
    m[0x10] = 'm';
    m[0x11] = 'n';
    m[0x12] = 'o';
    m[0x13] = 'p';
    m[0x14] = 'q';
    m[0x15] = 'r';
    m[0x16] = 's';
    m[0x17] = 't';
    m[0x18] = 'u';
    m[0x19] = 'v';
    m[0x1A] = 'w';
    m[0x1B] = 'x';
    m[0x1C] = 'y';
    m[0x1D] = 'z';
    m[0x104] = 'A';
    m[0x105] = 'B';
    m[0x106] = 'C';
    m[0x107] = 'D';
    m[0x108] = 'E';
    m[0x109] = 'F';
    m[0x10A] = 'G';
    m[0x10B] = 'H';
    m[0x10C] = 'I';
    m[0x10D] = 'J';
    m[0x10E] = 'K';
    m[0x10F] = 'L';
    m[0x110] = 'M';
    m[0x111] = 'N';
    m[0x112] = 'O';
    m[0x113] = 'P';
    m[0x114] = 'Q';
    m[0x115] = 'R';
    m[0x116] = 'S';
    m[0x117] = 'T';
    m[0x118] = 'U';
    m[0x119] = 'V';
    m[0x11A] = 'W';
    m[0x11B] = 'X';
    m[0x11C] = 'Y';
    m[0x11D] = 'Z';
    m[0x2C] = ' ';
    m[0x12E] = '+';
    m[0x2D] = '-';
    m[0x125] = '*';
    m[0x38] = '/';
    m[0x2E] = '=';
    m[0x138] = '?';
    m[0x120] = '#';
    m[0x124] = '&';
    m[0x123] = '^';
    m[0x121] = '$';
    m[0x11F] = '@';
    m[0x11E] = '!';
    m[0x126] = '(';
    m[0x127] = ')';
    m[0x12D] = '_';
}

//Returns 0 on success and 1 if devpath not found.
static int mc_get_devpath(std::string* out_path)
{
    char buf[256];
    memset(buf, 0, 256);
    FILE* fp = popen("dmesg | grep 'Keyboard \\[Telink BLE Remote KMA Dongle\\]'", "r");
    while(fgets(buf, 256, fp)){}
    pclose(fp);
    if(buf[0] == 0)
        return 1;
    char* p = strstr(buf, "hidraw");
    if(p == NULL)
        return 1;
    p += 6;
    *(p+1) = '\0';
    *out_path = "/dev/hidraw";
    out_path->append(p);
    if(Common::path_exists(out_path->c_str()))
        return 0;
    return 1;
}

static int mc_buf_to_key(uint8_t* buf)
{
    int ret = buf[2];
    if(buf[0] == 0x20)
        ret += 0x100;
    return ret;
}

static void mc_dds_write_mds(const char* str)
{
    printf("patient_id=%s\n", str);
    Common* common = Common::instance();
    std::string patient_id = str;
    uint8_t found = 0;
    int i=0;
    auto it = common->md->pm.patients.begin();
    for(;it!=common->md->pm.patients.end();it++)
    {
        if(it->second.id.compare(patient_id) == 0)
        {
            found = 1;
            break;
        }
        i++;
    }
    if(!found)
    {
        return;
    }
    common->mapping_tab->select_patient(i);
}

static void mc_dds_write_alert()
{
    //printf("alert\n");
    Common* common = Common::instance();
    if(common->is_server)
        return;
    dds::core::xtypes::DynamicData sample(common->techalert_type);
    sample.value<std::string>("vmd_id", common->vmd_id);
    sample.value<std::string>("patient_id", common->patient_id);
    sample.value<std::string>("alarm_no", Common::get_alarmno());
    std::string str = "ED700";
    sample.value<std::string>("alarm_code", str);
    str = "Barcode scanner dongle not connected.";
    sample.value<std::string>("alarm_description", str);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    rti::core::xtypes::LoanedDynamicData loaned_member = sample.loan_value("source_timestamp");
    loaned_member.get().value("sec", (int32_t)ts.tv_sec);
    loaned_member.get().value("nanosec", (uint32_t)ts.tv_nsec);
    loaned_member.return_loan();
    //common->techalert_writer.write(sample);
}

void BarcodeDaemon::init(MainDialog* app)
{
    this->app = app;
    running = 1;
    mc_build_charmap(m);
    last_devpath_time = 0;
    last_read_time = 0;
    last_alert_time = 0;
    fd = -1;
    active = 0;
    mc_mapping_active = &active;
}

void BarcodeDaemon::step()
{
    if(!running)
        return;
    Common* common = Common::instance();


    uint32_t current_time = Common::get_time_ms();
    if(Common::get_elapsed_time(current_time, last_devpath_time) >= 2000)
    {
        last_devpath_time = current_time;
        if(mc_get_devpath(&devpath))
        {
            devpath.clear();
            last_devpath.clear();
            if(fd >= 0)
            {
                close(fd);
                fd = -1;
            }
        }
        else if(last_devpath.compare(devpath) != 0)
        {
            last_devpath = devpath;
            if(fd >= 0)
                close(fd);
            fd = open(devpath.c_str(), O_RDONLY | O_NONBLOCK);
        }
    }
    if(fd >= 0)
    {
        uint8_t buf[8];
        if(Common::get_elapsed_time(current_time, last_read_time) < 200)
        {
            while(read(fd, buf, 8) == 8)
            {
                last_read_time = current_time;
                if(buf[2] != 0)
                    current_text.push_back(m[mc_buf_to_key(buf)]);
            }
        }
        else
        {
            if(current_text.size() > 0)
            {
                //common->patient_id = current_text;
                if(active)
                    mc_dds_write_mds(current_text.c_str());
                current_text.clear();
            }
            while(read(fd, buf, 8) == 8)
            {
                last_read_time = current_time;
                if(buf[2] != 0)
                    current_text.push_back(m[mc_buf_to_key(buf)]);
            }
        }
    }
    else
    {
        if(Common::get_elapsed_time(current_time, last_alert_time) >= (uint32_t)common->barcode_alert_interval*1000)
        {
            last_alert_time = current_time;
            mc_dds_write_alert();
        }
    }
}
