#include "exporter.h"
#include "MainDialog.h"
#include <QTime>
#include <QDateTime>
#include <QDate>

class obs_entry
{
public:
    std::string mdc_code;
    float value;
    std::string unit;
    std::string description;
    std::string timestamp;
};

void Exporter::init()
{
    exporting = 0;
    last_time = 0;
    qcond = NULL;
}

void Exporter::step()
{
    Common* common = Common::instance();
    if(!exporting)
        return;
    uint32_t current_time = common->get_time_ms();
    if(common->get_elapsed_time(current_time, last_time) > interval)
    {
        last_time = current_time;
        auto pit = common->md->pm.patients.find(common->bed_id);
        if(pit == common->md->pm.patients.end())
        {
            printf("bedid=%s not found\n", common->bed_id.c_str());
            return;
        }
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples = common->exporter_observation_reader.select().condition(*qcond).take();
        if(samples.length() == 0)
        {
            //printf("nodata\n");
            return;
        }
        common->exporter_page->clear_log();
        //else
           // printf("got %d samples\n", (int)samples.length());
        FILE* fp = fopen(filepath.c_str(), "a");
        if(fp == NULL)
        {
            printf("Unable to open %s for writing.\n", filepath.c_str());
            return;
        }
        std::map<std::string, std::vector<obs_entry>> obs_data;
        for(auto& sample : samples)
        {
            if(sample.info().valid())
            {
                dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
                std::string channel_id = data.value<std::string>("channel_id");
                auto dit = common->md->dm.devices.find(channel_id);
                if(dit == common->md->dm.devices.end())
                    continue;
                obs_entry e;
                e.mdc_code = data.value<std::string>("mdc_code");
                e.value = data.value<float>("value");
                e.unit = data.value<std::string>("unit");
                e.description = data.value<std::string>("description");
                rti::core::xtypes::LoanedDynamicData loaned_member = data.loan_value("source_timestamp");
                int32_t sec = loaned_member.get().value<int32_t>("sec");
                struct tm t;
                char buf1[256];
                time_t timestamp = (time_t)sec;
                localtime_r(&timestamp, &t);
                sprintf(buf1, "%d-%d-%d %02d:%02d:%02d", t.tm_year+1900, t.tm_mon+1,
                        t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
                loaned_member.return_loan();
                e.timestamp = buf1;
                auto eit = obs_data.find(channel_id);
                if(eit == obs_data.end())
                {
                    std::vector<obs_entry> v;
                    v.push_back(e);
                    obs_data.emplace(channel_id,v);
                }
                else
                    eit->second.push_back(e);
            }
        }
        rapidjson::Document d;
        d.SetObject();
        d.AddMember("patient_id", rapidjson::Value().SetString(common->patient_id.c_str(), d.GetAllocator()), d.GetAllocator());
        d.AddMember("mrn", rapidjson::Value().SetString(pit->second.mrn.c_str(), d.GetAllocator()), d.GetAllocator());
        d.AddMember("given_name", rapidjson::Value().SetString(pit->second.given_name.c_str(), d.GetAllocator()), d.GetAllocator());
        d.AddMember("family_name", rapidjson::Value().SetString(pit->second.family_name.c_str(), d.GetAllocator()), d.GetAllocator());
        std::string gender;
        if(pit->second.gender.compare("M") == 0)
            gender = "Male";
        else if(pit->second.gender.compare("F") == 0)
            gender = "Female";
        else
            gender = "Other";
        d.AddMember("gender", rapidjson::Value().SetString(gender.c_str(), d.GetAllocator()), d.GetAllocator());
        rapidjson::Value v;
        v.SetArray();
        for(auto oit=obs_data.begin();oit!=obs_data.end();oit++)
        {
            rapidjson::Value c;
            c.SetObject();
            c.AddMember("channel_id", rapidjson::Value().SetString(oit->first.c_str(), d.GetAllocator()), d.GetAllocator());
            rapidjson::Value obs;
            obs.SetArray();
            for(int i=0;i<(int)oit->second.size();i++)
            {
                rapidjson::Value e;
                e.SetObject();
                e.AddMember("mdc_code", rapidjson::Value().SetString(oit->second[i].mdc_code.c_str(), d.GetAllocator()), d.GetAllocator());
                e.AddMember("value", oit->second[i].value, d.GetAllocator());
                e.AddMember("unit", rapidjson::Value().SetString(oit->second[i].unit.c_str(), d.GetAllocator()), d.GetAllocator());
                e.AddMember("description", rapidjson::Value().SetString(oit->second[i].description.c_str(), d.GetAllocator()), d.GetAllocator());
                e.AddMember("source_timestamp", rapidjson::Value().SetString(oit->second[i].timestamp.c_str(), d.GetAllocator()), d.GetAllocator());
                obs.PushBack(e, d.GetAllocator());
            }
            c.AddMember("observation", obs, d.GetAllocator());
            v.PushBack(c, d.GetAllocator());
        }
        d.AddMember("observations", v, d.GetAllocator());
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        d.Accept(writer);
        std::string str = buffer.GetString();
        str.push_back('\n');
        fwrite(str.c_str(), 1, str.size(), fp);
        time_t now = time(NULL);
        struct tm nowtm;
        localtime_r(&now, &nowtm);
        rapidjson::StringBuffer prettybuf;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> prettywriter(prettybuf);
        d.Accept(prettywriter);
        str = prettybuf.GetString();
        str.push_back('\n');
        common->exporter_page->log(str.c_str());
        char buf[256];
        sprintf(buf, "[%d/%d/%d %02d:%02d:%02d]", nowtm.tm_year+1900, nowtm.tm_mon+1,
                nowtm.tm_mday, nowtm.tm_hour, nowtm.tm_min, nowtm.tm_sec);
        common->exporter_page->log(buf);

        fclose(fp);
    }
}

void Exporter::start_exporting(std::string path, uint32_t interval_ms)
{
    Common* common = Common::instance();
    exporting = 1;
    if(qcond)
        delete qcond;
    std::string querystr = "vmd_id MATCH '";
    querystr.append(common->vmd_id);
    querystr.append("' AND patient_id MATCH '");
    querystr.append(common->patient_id);
    querystr.append("'");
    qcond = new dds::sub::cond::QueryCondition(
                dds::sub::Query(common->exporter_observation_reader, querystr),
                dds::sub::status::DataState(
                dds::sub::status::SampleState::any(),
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
    filepath = path;
    interval = interval_ms;
    last_time = 0;
}

void Exporter::stop_exporting()
{
    exporting = 0;
}
