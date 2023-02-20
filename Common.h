#ifndef COMMON_H
#define COMMON_H

#include "pluginmanager.h"
#include "ddsinterface.h"
#include "cblinterface.h"
#include "utilsinterface.h"
#include <dds/pub/ddspub.hpp>
#include <dds/core/ddscore.hpp>
#include <dds/sub/ddssub.hpp>
#include <stdint.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include <pthread.h>
#include "Tab_Mapping_Widget.h"
#include "ui_Tab_Mapping_Widget.h"
#include "log.h"

//by philo
#include "Dialog_msg.h"
#include "Dialog_release.h"
#include "Tab_Observations_Widget.h"
#include "Tab_Utilities_exporterPage_Widget.h"
#include <map>
#include "Tab_NS_Monitor.h"
#include "CSS_Infusion.h"

#define STATIONARY_CHECK_DISTANCE 10.0f
#define VELOCITY_MULTIPLIER 1.4
#define DEFAULT_FRICTION 0.35
#define LINE_BREAK_DELTA 3000

class MainDialog;
class Tab_Devices_Widget;
class Tab_Utilities_alarmHistoryPage_Widget;
class Tab_Observations_metricItemsDisplayConfigPage_Widget;
class Tab_Observations_mainPage_Widget;
class Tab_Observations_historyPage_Widget;

class mc_entry
{
public:
    std::string code;
    std::string model;
    std::string desc;
    std::string abbv;
    std::string unit;
    std::string y_max;
    std::string y_min;
    std::string display_index;
    std::string display_desc;
    float val;
    struct timespec ts;
};

class Device
{
public:
    std::string channel_id;
    std::string manufacturer;
    std::string model;
    std::string serial_no;
    std::string desc;
};

class mc_checkstate
{
public:
    uint8_t checked;
    int order;
};

class Common
{
public:
    MainDialog* md;
    Tab_Mapping_Widget* mapping_tab;
    Tab_Utilities_exporterPage_Widget* exporter_page;
    Tab_Utilities_alarmHistoryPage_Widget* alarm_page = NULL;
    Tab_Utilities_alarmHistoryPage_Widget* alarm_page_2 = NULL;
    Tab_Devices_Widget* devices_page;
    Tab_Observations_Widget* observation_widget_page;
    Tab_Observations_metricItemsDisplayConfigPage_Widget* device_settings_page;
    Tab_Observations_mainPage_Widget* observation_main_page;
    Tab_Observations_historyPage_Widget* history_page;
    Tab_NS_Monitor* monitor_page =NULL;
    int barcode_alert_interval;
    int patients_query_interval;
    int MDSloop_interval;
    int Network_Interval;
    int Alarmloop_interval;
    int devices_query_interval;
    uint32_t button_hold_state_duration0;
    uint32_t button_hold_state_duration1;
    uint32_t button_hold_tick_interval_slow;
    uint32_t button_hold_tick_interval_fast;
    std::string vmd_id;
    int domain_id;
    std::string patient_id;
    std::string bed_id;
    std::string room_id;
    cbl::Database db;
    cbl::Database alarm_db;
    cbl::Database csp_db;
    cbl::Replicator observation_puller;
    cbl::Replicator display_items_pusher;
    cbl::Replicator display_items_puller;
    cbl::Database display_items_db;
    std::string display_items_path;
    std::string display_items_url;
    std::string display_items_uid;
    std::string display_items_pwd;
    std::string vmd_url;
    std::string restful_API_url;
    PluginManager *pluginManager;
    DDSInterface *dds;
    CBLInterface *cbl;
    UtilsInterface *utils;
    Dialog_msg msg;
    Dialog_release rd;
    CSS css;
    int max_undo_history_entries;
    std::map<std::string, uint8_t> device_checkstate;
    std::map<std::string, mc_checkstate> item_checkstate;
    std::string qos_path;
    std::string log_path;
    std::string export_path;
    std::string history_model;
    std::string history_mdccode;
    std::string history_datasource;
    std::map<std::string, std::vector<std::string>> special_items;
    bool is_server=1;
    QByteArray ReadAsciiFile(const QString &filename);
    QJsonObject RetrieveJson(const QString &jsonFormatFile);
    QString get_PID();
    QString PID;
    QString sPID;
    fflog_t* log;
    QJsonArray Restful_API(char queryStartTime[64] , char queryEndTime[64], std::string dataSource);
    QJsonArray Restful_API_Orderby(char queryStartTime[64] , char queryEndTime[64], std::string dataSource, std::string model, std::string orderStr);

    dds::domain::DomainParticipant participant = nullptr;
    dds::pub::Publisher publisher = nullptr;
    dds::sub::Subscriber subscriber = nullptr;
    dds::core::QosProvider qos_provider = nullptr;

    dds::topic::Topic<dds::core::xtypes::DynamicData> mds_topic = nullptr;
    dds::pub::DataWriter<dds::core::xtypes::DynamicData> mds_writer = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> mds_reader = nullptr;
    dds::core::xtypes::DynamicType mds_type;

    dds::topic::Topic<dds::core::xtypes::DynamicData> m_DisplayItem_topic = nullptr;
    dds::pub::DataWriter<dds::core::xtypes::DynamicData> m_DisplayItem_writer = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> m_DisplayItem_reader = nullptr;
    dds::core::xtypes::DynamicType m_DisplayItem_type;

    dds::pub::DataWriter<dds::core::xtypes::DynamicData> mdsm_writer = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> mdsm_reader = nullptr;

    dds::topic::Topic<dds::core::xtypes::DynamicData> techalert_topic = nullptr;
    dds::pub::DataWriter<dds::core::xtypes::DynamicData> techalert_writer = nullptr;
    dds::core::xtypes::DynamicType techalert_type;
    dds::core::xtypes::DynamicType time_type;

    dds::topic::Topic<dds::core::xtypes::DynamicData> patient_topic = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> patient_reader = nullptr;
    dds::core::xtypes::DynamicType patient_type;

    dds::topic::Topic<dds::core::xtypes::DynamicData> devcon_topic = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> devcon_reader = nullptr;
    dds::core::xtypes::DynamicType devcon_type;

    dds::topic::Topic<dds::core::xtypes::DynamicData> devid_topic = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> devid_reader = nullptr;
    dds::core::xtypes::DynamicType devid_type;

    dds::topic::Topic<dds::core::xtypes::DynamicData> useractions_topic = nullptr;
    dds::pub::DataWriter<dds::core::xtypes::DynamicData> useractions_writer = nullptr;
    dds::core::xtypes::DynamicType useractions_type;

    dds::topic::Topic<dds::core::xtypes::DynamicData> observation_topic = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> exporter_observation_reader = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> history_observation_reader = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> visualizetion_observation_reader = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> observation_reader = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> observation_reader_2 = nullptr;
    dds::core::xtypes::DynamicType observation_type;

    dds::topic::Topic<dds::core::xtypes::DynamicData> chansettings_topic = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> chansettings_reader = nullptr;
    dds::core::xtypes::DynamicType chansettings_type;

    dds::topic::Topic<dds::core::xtypes::DynamicData> chansettingshis_topic = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> chansettingshis_reader = nullptr;
    dds::core::xtypes::DynamicType chansettingshis_type;

    dds::topic::Topic<dds::core::xtypes::DynamicData> rtobservation_topic = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> rtobservation_reader = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> rtobservation_reader_2 = nullptr;

    dds::core::xtypes::DynamicType rtobservation_type;

    dds::topic::Topic<dds::core::xtypes::DynamicData> topalarm_topic = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> topalarm_reader = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> topalarm_reader_2 = nullptr;
    dds::pub::DataWriter<dds::core::xtypes::DynamicData> topalarm_writer = nullptr;
    dds::core::xtypes::DynamicType topalarm_type;

    dds::topic::Topic<dds::core::xtypes::DynamicData> CapturedIssues_topic = nullptr;
    dds::pub::DataWriter<dds::core::xtypes::DynamicData> CapturedIssues_writer = nullptr;
    dds::pub::DataWriter<dds::core::xtypes::DynamicData> CapturedIssues_writer1 = nullptr;
    dds::sub::DataReader<dds::core::xtypes::DynamicData> CapturedIssues_reader = nullptr;
    dds::core::xtypes::DynamicType CapturedIssues_type;

    Common(MainDialog* m);
    ~Common();
    void init_dds(int domain_id);
    void commit_user_action(const char* user_id, const char* function_id, const char* action);
    void add_savina_items(std::string model, std::multimap<int, mc_entry>* entries, std::multimap<std::string, mc_entry>* left_over);
    void remove_savina_items(std::multimap<int, mc_entry>* entries);
    void populate_item_checkstate();
    void populate_device_checkstate();


    static Common* instance();
    static uint32_t get_time_ms();
    static uint32_t get_elapsed_time(uint32_t current_time, uint32_t last_time);
    static void sleep_ms(uint32_t ms);
    static int path_exists(const char* path);
    static void* alloc_from_filep(FILE* fp, int* size);
    static void* alloc_from_file(const char* path, int* size);
    static void string_split(std::vector<std::string>* list, std::string* str, char split_token);
    static void draw_text(QPainter & painter, qreal x, qreal y, Qt::Alignment flags,
                  const QString & text);
    static float distance(float x1, float y1, float x2, float y2);
    static void set_inactive_colors(QWidget* w);
    static std::string get_alarmno();
};

#endif // COMMON_H
