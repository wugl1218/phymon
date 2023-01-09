#ifndef MANAGER_PATIENT_H
#define MANAGER_PATIENT_H

#include "Common.h"
#include <map>
#include <QTcpSocket>

class Patient
{
public:
    uint8_t bound;
    std::string id;
    std::string mrn;
    std::string district;
    std::string room;
    std::string bed;
    std::string family_name;
    std::string given_name;
    std::string bod;
    std::string gender;
    std::string vehicle;
};

class PatientManager
{
public:
    uint8_t running;
    uint8_t auto_rebind;
    uint8_t omit_observation_redirect;
    dds::sub::cond::ReadCondition* patient_cond;
    dds::sub::cond::ReadCondition* mds_cond;
    uint32_t last_query_time;
    std::map<std::string, Patient> patients;
    std::map<std::string, std::string> bound_patients;
    void init();
    void step();
    void force_release();
    bool is_checking=0 ;
    QTcpSocket *client = nullptr;


};

#endif // MANAGER_PATIENT_H
