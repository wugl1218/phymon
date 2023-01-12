#include "Common.h"
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "MainDialog.h"
#include "qpainter.h"

Common* _common = NULL;
//123


Common::Common(MainDialog* m)
    : msg(m),
      rd(m),
    mds_type(dds::core::xtypes::StructType("")),
    techalert_type(dds::core::xtypes::StructType("")),
    time_type(dds::core::xtypes::StructType("")),
    patient_type(dds::core::xtypes::StructType("")),
    devcon_type(dds::core::xtypes::StructType("")),
    devid_type(dds::core::xtypes::StructType("")),
    useractions_type(dds::core::xtypes::StructType("")),
    observation_type(dds::core::xtypes::StructType("")),
    chansettings_type(dds::core::xtypes::StructType("")),
    chansettingshis_type(dds::core::xtypes::StructType("")),
    rtobservation_type(dds::core::xtypes::StructType("")),
    topalarm_type(dds::core::xtypes::StructType("")),
    CapturedIssues_type(dds::core::xtypes::StructType(""))
{
    _common = this;
    md = m;

    rd.setParent(m);
    int filesize;
    char* cfg = (char*)Common::alloc_from_file("../config/config.json", &filesize);
    rapidjson::Document d;
    d.Parse(cfg);
    free(cfg);
    barcode_alert_interval = d["AlertInterval"].GetInt();
    patients_query_interval = d["GetPatientsInterval"].GetInt();
    Network_Interval=d["NetworkInterval"].GetInt();
    MDSloop_interval= d["GetMDSloopInterval"].GetInt();
    devices_query_interval = d["GetDevicesInterval"].GetInt();
    Alarmloop_interval= d["GetTopalarmInterval"].GetInt();
    max_undo_history_entries = d["MaxUndoHistoryEntries"].GetInt();
    std::string db_path = d["DBPath"].GetString();
    std::string plugins_path = d["PluginsPath"].GetString();
    qos_path = d["QosPath"].GetString();
    export_path = d["ExportPath"].GetString();
    display_items_path = d["DisplayItemsDBPath"].GetString();
    display_items_url = d["DisplayItemsUrl"].GetString();
    display_items_uid = d["DisplayItemsUid"].GetString();
    display_items_pwd = d["DisplayItemsPwd"].GetString();
    vmd_url = d["VMDUrl"].GetString();
    button_hold_state_duration0 = d["ButtonHoldStateDuration0"].GetUint();
    button_hold_state_duration1 = d["ButtonHoldStateDuration1"].GetUint();
    button_hold_tick_interval_slow = d["ButtonHoldTickIntervalSlow"].GetUint();
    button_hold_tick_interval_fast = d["ButtonHoldTickIntervalFast"].GetUint();
    log_path = d["LogPath"].GetString();;

    rti::config::Verbosity verbosity = static_cast<rti::config::Verbosity::inner_enum>(0);
    // Starting RTI DDS...
    // rti::config::Verbosity verbosity(rti::config::Verbosity::WARNING);
    // NDDSConfigLogger::get_instance()->set_verbosity(verbosity);
    //rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_LOCAL); //顯示dds訊息
    pluginManager = PluginManager::instance();
    pluginManager->loadAllPlugins(plugins_path.c_str());

    QString pluginName = "DDSPlugin";
    dds = qobject_cast<DDSInterface *>(pluginManager->getPluginObject(pluginName));
    pluginName = "CBLPlugin";
    cbl = qobject_cast<CBLInterface *>(pluginManager->getPluginObject(pluginName));
    pluginName = "UtilsPlugin";
    utils = qobject_cast<UtilsInterface*>(pluginManager->getPluginObject(pluginName));
    db = cbl->getAppDB("VMD.cbl", db_path, false);

    display_items_db = cbl->getAppDB("DisplayItems.cbl", display_items_path, false);
    std::string dummy;
    display_items_pusher = cbl->getReplicatorBasicPnP(display_items_db, display_items_url, display_items_uid, display_items_pwd, dummy, false);
    display_items_pusher.start();
    //display_items_puller = cbl->getReplicatorBasicPullOnly(display_items_db, display_items_url, display_items_uid, display_items_pwd, dummy);
    //display_items_puller.start();

    if (QFile::exists("../../vmd/bin/config/app.cfg"))
    {
        QJsonObject jsonAppConfig = RetrieveJson(("../../vmd/bin/config/app.cfg"));
        sPID = jsonAppConfig["_PID"].toString();       
        PID = get_PID();
        if((PID == sPID) && (sPID !=""))
            is_server = 0;
    }
    if(!is_server)
    {
        cbl::ResultSet results = cbl->queryDocuments(db, "SELECT vmd_id FROM _ WHERE data_source='VMD' ORDER BY source_timestamp DESC", dummy);
        for(auto& result: results)
        {
            vmd_id = result.valueForKey("vmd_id").asstring();
            break;
        }
        if(vmd_id.size() == 0)
        {
            printf("Unable to get vmd_id\n");
            exit(1);
        }
        md->nd.is_server=0;
    }
    else
        md->nd.is_server=1;
    cbl::ResultSet dresults = cbl->queryDocuments(db, "SELECT domain_id FROM _ WHERE data_source='DOMAIN' ORDER BY source_timestamp DESC", dummy);
    domain_id = -1;
    for(auto& result: dresults)
    {
        domain_id = result.valueAtIndex(0).asInt();
        break;
    }
    log = fflog_create(log_path.c_str(),5,100*1024*1024);
    fflog_out(log,"start");
    std::vector<std::string> savina_special;
    savina_special.push_back("Tidal volume in mL");
    savina_special.push_back("MV");
    savina_special.push_back("Respiratory rate");
    savina_special.push_back("I:E Ratio");
    savina_special.push_back("Peak airway pressure");
    savina_special.push_back("Plateau pressure");
    savina_special.push_back("RSI");
    special_items.emplace("Savina", savina_special);

    std::vector<std::string> savina300_special;
    savina300_special.push_back("Tidal volume");
    savina300_special.push_back("MV");
    savina300_special.push_back("Respiratory rate");
    savina300_special.push_back("I:E Ratio");
    savina300_special.push_back("Peak airway pressure");
    savina300_special.push_back("Plateau pressure");
    savina300_special.push_back("RSI");
    special_items.emplace("Savina 300", savina300_special);
}

Common::~Common()
{
    pluginManager->unloadAllPlugins();
}

void Common::init_dds(int domain_id)
{


    participant = dds->getDomainParticipant(domain_id, qos_path, "VMD_Library::profile");
    publisher = dds->getPublisher();
    subscriber = dds->getSubscrier();

    mds_type = dds->getTypeObject("MonitoringStation::MDSConnectivity");
    mds_topic = dds->getTopic("TP_MDSConnectivity", mds_type, "");
    mds_writer = dds->getWriter(mds_topic, "VMD_Library::profile::dwMDSConnectivity");
    mds_reader = dds->getReader(mds_topic, "VMD_Library::profile::drMDSConnectivity");

    mdsm_writer = dds->getWriter(mds_topic, "VMD_Library::profile::dwMDSConnectivity");
    mdsm_reader = dds->getReader(mds_topic, "VMD_Library::profile::drMDSConnectivity");

    techalert_type = dds->getTypeObject("common::Alert");
    techalert_topic = dds->getTopic("TP_TechnicalAlert", techalert_type, "");
    techalert_writer = dds->getWriter(techalert_topic, "VMD_Library::profile::dwTechnicalAlert");

    patient_type = dds->getTypeObject("dds_collector::Patient");
    patient_topic = dds->getTopic("TP_Patient", patient_type, "");
    patient_reader = dds->getReader(patient_topic, "VMD_Library::profile::drPatient");

    devcon_type = dds->getTypeObject("dds_collector::DeviceConnectivity");
    devcon_topic = dds->getTopic("TP_DeviceConnectivity", devcon_type, "");
    devcon_reader = dds->getReader(devcon_topic, "VMD_Library::profile::drDeviceConnectivity");

    devid_type = dds->getTypeObject("dds_collector::DeviceIdentity");
    devid_topic = dds->getTopic("TP_DeviceIdentity", devid_type, "");
    devid_reader = dds->getReader(devid_topic, "VMD_Library::profile::drDeviceIdentity");

    useractions_type = dds->getTypeObject("common::UserActions");
    useractions_topic = dds->getTopic("TP_UserActions", useractions_type, "");
    useractions_writer = dds->getWriter(useractions_topic, "VMD_Library::profile::dwUserActions");

    observation_type = dds->getTypeObject("dds_collector::Observation");
    observation_topic = dds->getTopic("TP_Observation", observation_type, "");
    exporter_observation_reader = dds->getReader(observation_topic, "VMD_Library::profile::drObservation");
    history_observation_reader = dds->getReader(observation_topic, "VMD_Library::profile::drObservation");
    observation_reader = dds->getReader(observation_topic, "VMD_Library::profile::drObservation");

    chansettings_type = dds->getTypeObject("dds_collector::ChannelSettings");
    chansettings_topic = dds->getTopic("TP_ChannelSettings", chansettings_type, "");
    chansettings_reader = dds->getReader(chansettings_topic, "VMD_Library::profile::drChannelSettings");

    chansettingshis_type = dds->getTypeObject("MonitoringStation::ChannelSettingsHis");
    chansettingshis_topic = dds->getTopic("TP_ChannelSettingsHis", chansettingshis_type, "");
    chansettingshis_reader = dds->getReader(chansettingshis_topic, "VMD_Library::profile::drChannelSettingsHis");

    rtobservation_type = dds->getTypeObject("dds_collector::RTObservation");
    rtobservation_topic = dds->getTopic("TP_RTObservation", rtobservation_type, "");
    rtobservation_reader = dds->getReader(rtobservation_topic, "VMD_Library::profile::drPatientAlert");

    topalarm_type = dds->getTypeObject("common::Alert");
    topalarm_topic = dds->getTopic("TP_PatientAlert", topalarm_type, "");
    topalarm_reader = dds->getReader(topalarm_topic, "VMD_Library::profile::drPatientAlert");
    topalarm_writer = dds->getWriter(topalarm_topic, "VMD_Library::profile::dwPatientAlert");

    topalarm_reader_2 = dds->getReader(techalert_topic, "VMD_Library::profile::drTechnicalAlert");

    CapturedIssues_type = dds->getTypeObject("MonitoringStation::CapturedIssues");
    CapturedIssues_topic = dds->getTopic("TP_CapturedIssues", CapturedIssues_type, "");
    CapturedIssues_reader = dds->getReader(CapturedIssues_topic, "VMD_Library::profile::drCapturedIssues");
    CapturedIssues_writer = dds->getWriter(CapturedIssues_topic, "VMD_Library::profile::dwCapturedIssues");
    CapturedIssues_writer1 = dds->getWriter(CapturedIssues_topic, "VMD_Library::profile::dwCapturedIssues");

}

void Common::commit_user_action(const char* user_id, const char* function_id, const char* action)
{
    if(patient_id.size() == 0 || vmd_id.size() == 0)
        return;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    dds::core::xtypes::DynamicData sample(useractions_type);
    sample.value<std::string>("user_id", user_id);
    sample.value<std::string>("patient_id", patient_id);
    sample.value<std::string>("function_id", function_id);
    sample.value<std::string>("action", action);
    sample.value<std::string>("exec_code", "IM000");
    sample.value<std::string>("exec_msg", action);
    rti::core::xtypes::LoanedDynamicData loaned_member = sample.loan_value("exec_timestamp");
    loaned_member.get().value("sec", (int32_t)ts.tv_sec);
    loaned_member.get().value("nanosec", (uint32_t)ts.tv_nsec);
    loaned_member.return_loan();
    useractions_writer.write(sample);

    rapidjson::Document d;
    d.SetObject();
    d.AddMember("user_id", rapidjson::Value().SetString(user_id, d.GetAllocator()), d.GetAllocator());
    d.AddMember("data_source", "UserActions", d.GetAllocator());
    d.AddMember("patient_id", rapidjson::Value().SetString(patient_id.c_str(), d.GetAllocator()), d.GetAllocator());
    d.AddMember("function_id", rapidjson::Value().SetString(function_id, d.GetAllocator()), d.GetAllocator());
    d.AddMember("action", rapidjson::Value().SetString(action, d.GetAllocator()), d.GetAllocator());
    d.AddMember("exec_code", rapidjson::Value().SetString("IM000", d.GetAllocator()), d.GetAllocator());
    d.AddMember("exec_msg", rapidjson::Value().SetString(action, d.GetAllocator()), d.GetAllocator());
    rapidjson::Value val;
    val.SetObject();
    val.AddMember("sec", ts.tv_sec, d.GetAllocator());
    val.AddMember("nanosec", ts.tv_nsec, d.GetAllocator());
    d.AddMember("exec_timestamp", val, d.GetAllocator());
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    std::string dummy;
    std::string dummy2;
    cbl->saveMutableDocument(db, buffer.GetString(), dummy2, dummy);
}

void Common::add_savina_items(std::string model, std::multimap<int, mc_entry>* entries, std::multimap<std::string, mc_entry>* left_over)
{
    uint8_t has_vt = 0;
    uint8_t has_rr = 0;
    uint8_t has_ipart = 0;
    uint8_t has_epart = 0;
    float vt;
    float rr;
    float ipart;
    float epart;
    std::string vt_desc;
    int mv_order = 9999;
    int ie_order = 9999;
    int rsi_order = 9999;
    if(model.compare("Savina") == 0)
        vt_desc = "Tidal volume in mL";
    else
        vt_desc = "Tidal volume";
    auto it = left_over->find(vt_desc);
    if(it != left_over->end())
    {
        has_vt = 1;
        vt = it->second.val;
    }
    it = left_over->find("Respiratory rate");
    if(it != left_over->end())
    {
        has_rr = 1;
        rr = it->second.val;
    }
    it = left_over->find("I:E I-Part");
    if(it != left_over->end())
    {
        has_ipart = 1;
        ipart = it->second.val;
    }
    it = left_over->find("I:E E-Part");
    if(it != left_over->end())
    {
        has_epart = 1;
        epart = it->second.val;
    }
    for(auto it2=entries->begin();it2!=entries->end();it2++)
    {
        if(it2->second.desc.compare(vt_desc) == 0)
        {
            has_vt = 1;
            vt = it2->second.val;
        }
        else if(it2->second.desc.compare("Respiratory rate") == 0)
        {
            has_rr = 1;
            rr = it2->second.val;
        }
        else if(it2->second.desc.compare("I:E I-Part") == 0)
        {
            has_ipart = 1;
            ipart = it2->second.val;
        }
        else if(it2->second.desc.compare("I:E E-Part") == 0)
        {
            has_epart = 1;
            epart = it2->second.val;
        }
    }
    if(item_checkstate.size() == 0)
    {
        auto it2 = special_items.find(model);
        if(it2 != special_items.end())
        {
            for(int i=0;i<(int)it2->second.size();i++)
            {
                if(it2->second[i].compare("MV") == 0)
                    mv_order = i;
                else if(it2->second[i].compare("I:E Ratio") == 0)
                    ie_order = i;
                else if(it2->second[i].compare("RSI") == 0)
                    rsi_order = i;
            }
        }
    }
    else
    {
        std::string str = model;
        str.append(",MV");
        auto it2 = item_checkstate.find(str);
        if(it2!=item_checkstate.end())
            mv_order = it2->second.order;
        str = model;
        str.append(",I:E Ratio");
        it2 = item_checkstate.find(str);
        if(it2!=item_checkstate.end())
            ie_order = it2->second.order;
        str = model;
        str.append(",RSI");
        it2 = item_checkstate.find(str);
        if(it2!=item_checkstate.end())
            rsi_order = it2->second.order;
    }
    if(has_vt && has_rr)
    {
        mc_entry e;
        e.code = "MV";
        e.desc = "MV";
        e.unit = "L/min";
        e.val = (vt/1000.0)*rr;
        entries->emplace(mv_order, e);
        e.code = "RSI";
        e.desc = "RSI";
        e.unit = "";
        e.val = vt/rr;
        entries->emplace(rsi_order, e);
    }
    if(has_ipart && has_epart)
    {
        mc_entry e;
        e.desc = "I:E Ratio";
        e.unit = "";
        e.val = 0.0f;
        char tbuf[64];
        sprintf(tbuf, "%.1f:%.1f", ipart, epart);
        e.code = tbuf;
        entries->emplace(ie_order, e);
    }
}

void Common::remove_savina_items(std::multimap<int, mc_entry>* entries)
{
    auto it_smv1 = entries->end();
    auto it_smv2 = entries->end();
    auto it_rmv1 = entries->end();
    auto it_rmv2 = entries->end();
    for(auto it=entries->begin();it!=entries->end();it++)
    {
        if(it->second.code.compare("7A")==0 || it->second.code.compare("B7")==0)
        {
            if(it_smv1 == entries->end())
                it_smv1 = it;
            else if(it_smv2 == entries->end())
                it_smv2 = it;
        }
        else if(it->second.code.compare("B8")==0 || it->second.code.compare("B9")==0)
        {
            if(it_rmv1 == entries->end())
                it_rmv1 = it;
            else if(it_rmv2 == entries->end())
                it_rmv2 = it;
        }
    }
    if(it_smv2 != entries->end())
    {
        uint8_t smv1_is_newer = 0;
        if(it_smv1->second.ts.tv_sec > it_smv2->second.ts.tv_sec)
            smv1_is_newer = 1;
        else if(it_smv1->second.ts.tv_sec == it_smv2->second.ts.tv_sec)
        {
            if(it_smv1->second.ts.tv_nsec > it_smv2->second.ts.tv_nsec)
                smv1_is_newer = 1;
        }
        if(smv1_is_newer)
            entries->erase(it_smv2);
        else
            entries->erase(it_smv1);
    }
    if(it_rmv2 != entries->end())
    {
        uint8_t rmv1_is_newer = 0;
        if(it_rmv1->second.ts.tv_sec > it_rmv2->second.ts.tv_sec)
            rmv1_is_newer = 1;
        else if(it_rmv1->second.ts.tv_sec == it_rmv2->second.ts.tv_sec)
        {
            if(it_rmv1->second.ts.tv_nsec > it_rmv2->second.ts.tv_nsec)
                rmv1_is_newer = 1;
        }
        if(rmv1_is_newer)
            entries->erase(it_rmv2);
        else
            entries->erase(it_rmv1);
    }
}

Common* Common::instance()
{
    return _common;
}

uint32_t Common::get_time_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

uint32_t Common::get_elapsed_time(uint32_t current_time, uint32_t last_time)
{
    if(current_time >= last_time)
        return current_time - last_time;
    return UINT_MAX - last_time + current_time;
}

void Common::sleep_ms(uint32_t ms)
{
    while(ms >= 999)
    {
        usleep(999000); //usleep will refuse to sleep if argument is greater than or equal to 1000000.
        ms-=999;
    }
    if(ms)
        usleep(ms*1000);
}

int Common::path_exists(const char* path)
{
    struct stat path_stat;
    int r = stat(path, &path_stat);
    if(r == 0)
        return 1;
    else
        return 0;
}

void* Common::alloc_from_filep(FILE* fp, int* size)
{
    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if(*size <= 0)
        return NULL;
    uint8_t* ret = (uint8_t*)malloc(*size+1);
    memset(ret, 0, *size+1);
    if(!ret)
        return NULL;
    fread(ret, *size, 1, fp);
    return ret;
}

void* Common::alloc_from_file(const char* path, int* size)
{
    FILE* fp = fopen(path, "rb");
    if(!fp)
        return NULL;
    uint8_t* ret = (uint8_t*)alloc_from_filep(fp, size);
    fclose(fp);
    return ret;
}

void Common::string_split(std::vector<std::string>* list, std::string* str, char split_token)
{
    list->clear();
    int start_index = 0;
    int count = 0;
    unsigned i=0;
    for(;i<str->size();i++)
    {
        if((*str)[i] == split_token)
        {
            count = i-start_index;
            if(count > 0)
                list->push_back(str->substr(start_index, count));
            else
                list->push_back("");
            start_index = i+1;
        }
    }
    count = i-start_index;
    if(count > 0)
        list->push_back(str->substr(start_index));
    else
        list->push_back("");
}

void Common::draw_text(QPainter & painter, qreal x, qreal y, Qt::Alignment flags,
              const QString & text)
{
    const qreal size = 32767.0;
    QPointF corner(x, y - size);
    if (flags & Qt::AlignHCenter) corner.rx() -= size/2.0;
    else if (flags & Qt::AlignRight) corner.rx() -= size;
    if (flags & Qt::AlignVCenter) corner.ry() += size/2.0;
    else if (flags & Qt::AlignTop) corner.ry() += size;
    else flags |= Qt::AlignBottom;
    QRectF rect{corner.x(), corner.y(), size, size};
    painter.drawText(rect, flags, text, NULL);
}

float Common::distance(float x1, float y1, float x2, float y2)
{
    return sqrtf(powf(x2-x1,2) + powf(y2-y1,2));
}

void Common::set_inactive_colors(QWidget* w)
{
    QPalette p = w->palette();
    p.setColor(QPalette::Inactive, QPalette::WindowText, p.color(QPalette::Active, QPalette::WindowText));
    p.setColor(QPalette::Inactive, QPalette::Button, p.color(QPalette::Active, QPalette::Button));
    p.setColor(QPalette::Inactive, QPalette::Text, p.color(QPalette::Active, QPalette::Text));
    p.setColor(QPalette::Inactive, QPalette::ButtonText, p.color(QPalette::Active, QPalette::ButtonText));
    p.setColor(QPalette::Inactive, QPalette::Window, p.color(QPalette::Active, QPalette::Window));
    p.setColor(QPalette::Inactive, QPalette::Highlight, p.color(QPalette::Active, QPalette::Highlight));
    w->setPalette(p);
}

void Common::populate_item_checkstate()
{
    item_checkstate.clear();
    std::string dummy;
    std::string sql = "SELECT item,visibility,morder,model FROM _ WHERE data_source='NumericVisibility' AND patient_id='";
    sql.append(patient_id);
    sql.append("' AND meta(_).expiration IS NOT VALUED AND expired=0");
    cbl::ResultSet results2 = cbl->queryDocuments(display_items_db, sql, dummy);
    for(auto& result: results2)
    {
        std::string item = result.valueAtIndex(0).asstring();
        mc_checkstate cs;
        cs.checked = result.valueAtIndex(1).asUnsigned();
        cs.order = result.valueAtIndex(2).asInt();
        std::string model = result.valueAtIndex(3).asstring();
        item_checkstate.emplace(model+","+item, cs);
    }
    if(item_checkstate.size() == 0)
        return;
    mc_checkstate k;
    k.checked = 1;
    k.order = 1;
    item_checkstate.emplace("Savina,MV", k);
    item_checkstate.emplace("Savina 300,MV", k);
    k.order = 3;
    item_checkstate.emplace("Savina,I:E Ratio", k);
    item_checkstate.emplace("Savina 300,I:E Ratio", k);
    k.order = 6;
    item_checkstate.emplace("Savina,RSI", k);
    item_checkstate.emplace("Savina 300,RSI", k);
}

void Common::populate_device_checkstate()
{
    device_checkstate.clear();
    std::string dummy;
    std::string sql = "SELECT model,checked FROM _ WHERE data_source='NumericDeviceSelection' AND patient_id='";
    sql.append(patient_id);
    sql.append("' AND meta(_).expiration IS NOT VALUED AND expired=0");
    cbl::ResultSet results = cbl->queryDocuments(display_items_db, sql, dummy);
    for(auto& result: results)
    {
        std::string model = result.valueAtIndex(0).asstring();
        uint8_t checked = result.valueAtIndex(1).asUnsigned();
        device_checkstate.emplace(model, checked);
    }
}

std::string Common::get_alarmno()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    struct tm t;
    char date_time[64];
    strftime(date_time, sizeof(date_time), "%Y%m%d%H%M%S", localtime_r(&ts.tv_sec, &t));
    std::stringstream ss;
    ss << date_time << std::setw(3) << std::setfill('0') << 1;
    std::string result = ss.str();
    return result;
}
QByteArray Common::ReadAsciiFile(const QString &filename)
{
    QByteArray result;

    QFile file(filename);
        try
    {
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {

            result = file.readAll();
            file.close();
        }
        else
        {
            qDebug() << "Failed to open a file";
        }
    }
    catch (std::exception &ex)
    {
        if (file.isOpen()) file.close();

        throw ex;
    }

    return result;
}
QJsonObject Common::RetrieveJson(const QString &jsonFormatFile)
{
QJsonObject result;

    // Load Application Information file(JSON format)
    QByteArray jsonData = ReadAsciiFile(jsonFormatFile);
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);

    // Convert doc to json object
    result = doc.object();
return result;
}
QString Common::get_PID()
{
char line[10];
FILE *cmd = popen("pidof -s VMD", "r");
long pid = 0;

fgets(line, 10, cmd);
pid = strtoul(line, NULL, 10);

PID = QString::number(pid);

pclose(cmd);

return PID;
}
