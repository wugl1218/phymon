#include "Manager_Patient.h"
#include "MainDialog.h"

void Manager_Patient::init()
{
    running = 1;
    auto_rebind = 1;
    Common* common = Common::instance();
    patient_cond = new dds::sub::cond::ReadCondition(
                common->patient_reader,
                dds::sub::status::DataState(
                dds::sub::status::SampleState::any(),
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
    mds_cond = new dds::sub::cond::ReadCondition(
                common->mds_reader,
                dds::sub::status::DataState(
                dds::sub::status::SampleState::any(),
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
    last_query_time = 0;
    omit_observation_redirect = 0;

}

void Manager_Patient::step()
{

    Common* common = Common::instance();
    uint32_t current_time = Common::get_time_ms();
    if(Common::get_elapsed_time(current_time, last_query_time) > (uint32_t)common->patients_query_interval*1000)
    {
     qDebug()<<"Manager_Patient";
        fflog_out(common->log,"Manager_Patient");
        bound_patients.clear();
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> mds_samples = common->mds_reader.select().condition(*mds_cond).read();
        for(auto& sample : mds_samples)
        {
            if(sample.info().valid())
            {
                dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
                std::string pid = data.value<std::string>("patient_id");
                std::string vmd_id = data.value<std::string>("vmd_id");
                if(pid.size() > 0)
                    bound_patients.emplace(pid, vmd_id);
                /*
                else
                {
                    for(auto it=bound_patients.begin();it!=bound_patients.end();)
                    {
                        if(it->second.compare(vmd_id) == 0)
                            bound_patients.erase(it++);
                        else
                            it++;
                    }
                }*/
            }
        }

        if(common->patient_id.size() > 0)
        {
            auto it = bound_patients.find(common->patient_id);
            if(it == bound_patients.end())
            {
                if(common->is_server==1)
                    common->mapping_tab->force_return();
                else
                    {
                    common->md->ui->nav->set_current_tab(0);
                    std::string dummy;
                    common->md->ui->mapping_tab->perform_release(dummy);
                    }
            }
        }

        last_query_time = current_time;
        uint8_t patient_exists = 0;
        uint8_t switched_beds = 0;
        uint8_t rebind_triggered = 0;
        std::string rebind_patient;
        std::string rebind_bed;
        uint8_t can_rebind = 0;
        int bind_index = 0;
        patients.clear();
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples = common->patient_reader.select().condition(*patient_cond).read();
        if(samples.length() > 0 && auto_rebind && !common->is_server)
        {
            std::string dummy;
            std::string sql = "SELECT patient_id,bed_id FROM _ WHERE data_source='MDSConnectivity' AND vmd_id='";
            sql.append(common->vmd_id);
            sql.append("'");
            cbl::ResultSet results = common->cbl->queryDocuments(common->db, sql, dummy);
            for(auto& result: results)
            {
                rebind_patient = result.valueAtIndex(0).asstring();
                rebind_bed = result.valueAtIndex(1).asstring();
                printf("has rebind patient=%s, bed=%s\n", rebind_patient.c_str(), rebind_bed.c_str());
            }
            rebind_triggered = 1;
            omit_observation_redirect = 1;
            uint8_t skip_rebind = 0;
            if(bound_patients.find(rebind_patient) != bound_patients.end())
                skip_rebind = 1;
            if(rebind_patient.size() == 0 || skip_rebind)
            {
                printf("rebind patient empty\n");
                rebind_triggered = 0;
                auto_rebind = 0;
                omit_observation_redirect = 0;
                if(skip_rebind)
                {
                    common->msg.setText("Unable to rebind.\nBound by another station.");
                    common->msg.exec();
                }
            }
        }
        for(auto& sample : samples)
        {
            if(sample.info().valid())
            {
                dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
                Patient p;
                p.id = data.value<std::string>("patient_id");
                p.mrn = data.value<std::string>("mrn");
                p.district = data.value<std::string>("hospital_district");
                p.room = data.value <std::string>("room_id");
                p.bed = data.value<std::string>("bed_id");
                p.family_name = data.value<std::string>("family_name");
                p.given_name = data.value<std::string>("given_name");
                p.bod = data.value<std::string>("bod");
                p.gender = data.value<std::string>("gender");
                p.vehicle = data.value<std::string>("vehicle_id");
                if(common->patient_id.size() > 0 && common->patient_id.compare(p.id) == 0)
                {
                    patient_exists = 1;
                    if(common->bed_id.compare(p.bed) != 0)
                        switched_beds = 1;
                }
/*
                std::string querystr = "patient_id MATCH '";
                querystr.append(p.id);
                querystr.append("'");
                dds::sub::cond::QueryCondition cond(
                            dds::sub::Query(common->mds_reader, querystr),
                            dds::sub::status::DataState(
                            dds::sub::status::SampleState::any(),
                            dds::sub::status::ViewState::any(),
                            dds::sub::status::InstanceState::alive()));
                dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> mds_samples = common->mds_reader.select().condition(cond).read();
                if(mds_samples.length() > 0)
                    p.bound = 1;
                else
                    p.bound = 0;*/

                p.bound = 0;
                auto m = bound_patients.find(p.id);
                if(m != bound_patients.end())
                    p.bound = 1;
                patients.emplace(p.bed, p);
            }
        }
        if(!common->is_server)
            common->mapping_tab->update_patients();
        if(rebind_triggered)
        {
            rebind_triggered = 0;
            auto_rebind = 0;
            int i=0;
            for(auto it=patients.begin();it!=patients.end();it++)
            {
                if(it->second.id.compare(rebind_patient) == 0 &&
                   it->second.bed.compare(rebind_bed) == 0)
                {
                    can_rebind = 1;
                    bind_index = i;
                }
                i++;
            }
            if(can_rebind)
            {
                printf("can rebind. bind_index=%d\n", (int)bind_index);
                common->mapping_tab->select_patient(bind_index);
            }
            else
                force_release();
        }
        else if((common->patient_id.size() > 0 && !patient_exists) || switched_beds)
            force_release();


    }
}

void Manager_Patient::force_release()
{
    Common* common = Common::instance();
    // we need to wait...
    client = new QTcpSocket;
    client->connectToHost(common->md->network->url, 80);
    if(client->waitForConnected(500))
    {
        common->md->ui->nav->set_current_tab(0);
        if(common->is_server)
            common->md->ui->mapping_tab->force_return();
        else
        common->md->ui->mapping_tab->force_release();
/*        if (!is_checking)
            {
                is_checking=1;
//                QTimer::singleShot(3000, common->md,  [=](){
                    if (!common->md->nm.is_network_error)
                        {
                            common->md->ui->nav->set_current_tab(0);
                            if(common->is_server)
                                common->md->ui->mapping_tab->force_return();
                            else
                            common->md->ui->mapping_tab->force_release();
                        }
                    is_checking=0;
                });
            }
    }
    else
    {
            return;
    }*/
        if(client->isOpen())
            client->close();
    }
}
