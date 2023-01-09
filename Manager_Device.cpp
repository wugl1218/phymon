#include "Manager_Device.h"
#include "Tab_Devices_Widget.h"
#include "Tab_Observations_mainPage_Widget.h"
#include "Tab_Utilities_alarmHistoryPage_Widget.h"
#include "Tab_Observations_metricItemsDisplayConfigPage_Widget.h"
#include "MainDialog.h"

void DeviceManager::init()
{
    last_query_time = 0;
}

void DeviceManager::step()
{
    Common* common = Common::instance();
    std::string querystr1 = "state MATCH 'READY'";
        querystr1.append("AND vmd_id MATCH '");
        querystr1.append(common->vmd_id);
        querystr1.append("'");
    dds::sub::cond::QueryCondition cond(
                dds::sub::Query(common->devcon_reader, querystr1),
                dds::sub::status::DataState(
                dds::sub::status::SampleState::any(),
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
    uint32_t current_time = Common::get_time_ms();
    if(common->is_server && common->patient_id.size()==0)return;
    if(Common::get_elapsed_time(current_time, last_query_time) > (uint32_t)common->devices_query_interval*1000)
    {
     qDebug()<<"DeviceManager";
        fflog_out(common->log,"DeviceManager");
        last_query_time = current_time;
        devices.clear();
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples = common->devcon_reader.select().condition(cond).read();
        for(auto& sample : samples)
        {
            if(sample.info().valid())
            {

                dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
                Device p;
                p.channel_id = data.value<std::string>("channel_id");

                if(p.channel_id.compare(common->vmd_id) == 0)
                    continue;
                std::string querystr = "vmd_id MATCH '";
                querystr.append(common->vmd_id);
                querystr.append("' AND channel_id MATCH '");
                querystr.append(p.channel_id);
                querystr.append("'");
                dds::sub::cond::QueryCondition cond2(
                            dds::sub::Query(common->devid_reader, querystr),
                            dds::sub::status::DataState(
                            dds::sub::status::SampleState::any(),
                            dds::sub::status::ViewState::any(),
                            dds::sub::status::InstanceState::alive()));
                dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> devid_samples = common->devid_reader.select().condition(cond2).read();
                if(devid_samples.length() > 0)
                {

                    auto i=devid_samples.begin();
                    p.manufacturer = i->data().value<std::string>("manufacturer");
                    p.model = i->data().value<std::string>("model");
                    p.serial_no = i->data().value<std::string>("serial_no");
                    p.desc = i->data().value<std::string>("description");
                    devices.emplace(p.channel_id, p);
                }

            }
        }

        common->devices_page->update_devices(&devices);
        common->alarm_page->update_devices(&devices);
        common->alarm_page_2->update_devices(&devices);
        common->device_settings_page->update_devices();
        common->mapping_tab->update_devices();
        if (devices.size()==0) //philo 11/25
        {
            if(common->is_server)
                {
                common->mapping_tab->force_return();
                }
            else
            {
                if(!is_DeviceLose)
                    common->md->ui->mapping_tab->hide_windows();

                common->md->ui->nav->set_current_tab(0);
                common->md->ui->nav->repaint();
                is_DeviceLose = 1;
                common->observation_main_page->clear_points();
            }
        }
        else if(is_DeviceLose &&devices.size()>0)
        {
            common->md->ui->nav->repaint();
            is_DeviceLose = 0;
        }
        if(common->is_server && common->patient_id!="")
        {
            std::string querystr = "patient_id MATCH '";
            querystr.append(common->patient_id);
            querystr.append("'");
            dds::sub::cond::QueryCondition cond1(
                        dds::sub::Query(common->mdsm_reader, querystr),
                        dds::sub::status::DataState(
                        dds::sub::status::SampleState::any(),
                        dds::sub::status::ViewState::any(),
                        dds::sub::status::InstanceState::alive()));
            dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> mds_samples = common->mdsm_reader.select().condition(cond1).read();
            if(mds_samples.length() == 0)
                common->md->ui->mapping_tab->force_return();
        }

    }
}
