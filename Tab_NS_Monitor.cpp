#include "Tab_NS_Monitor.h"
#include "ui_Tab_NS_Monitor.h"
#include "Manager_MDSConnectivity.h"
#include "MainDialog.h"
#include <QScrollBar>
#include <QMessageBox>
#include "qevent.h"
#include "Tab_Observations_mainPage_Widget.h"
#include "ui_Tab_Observations_mainPage_Widget.h"
#include "Tab_Mapping_Widget.h"
#include "ui_Tab_Mapping_Widget.h"
#include "Tab_Utilities_alarmHistoryPage_Widget.h"
#include "ui_Tab_Utilities_alarmHistoryPage_Widget.h"
#include "ui_Tab_Utilities_exporterPage_Widget.h"
#include "Tab_Observations_historyPage_Widget.h"
#include "ui_Tab_Devices_Widget.h"
#include "Tab_Observations_metricItemsDisplayConfigPage_Widget.h"
#include "Common.h"
#include "mc_visualization_setting_item.h"

void Tab_NS_Monitor::init()
{
    Common* common = Common::instance();
    common->monitor_page= this;
    ui->template_patient->hide();
    ui->template_patient_2->hide();
    ui->template_patient_6->hide();

    selected_patient = -1;
    if(common->is_server)
    {
        if (QFile::exists("../config/setbed.json"))
        {
            QJsonObject jsonAppConfig = common->RetrieveJson(("../config/setbed.json"));
             Bed_size = jsonAppConfig["bed_size"].toString();
             Set_bed = jsonAppConfig["bed_id"].toArray();
             Card_spacing= jsonAppConfig["Card_spacing"].toInt();
             Card_width= jsonAppConfig["Card_width"].toInt();
             Card_height= jsonAppConfig["Card_height"].toInt();
//              Card_spacing= 25;
//              Card_width= 351;
//              Card_height= 236;
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Debug by Philo\nPlease check setbed.json.");
            msgBox.exec();
            exit(1);
        }
    }


}

Tab_NS_Monitor::Tab_NS_Monitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab_NS_Monitor)
{
    ui->setupUi(this);
    mc_visualization_setting_item* e = new mc_visualization_setting_item(ui->scrollAreaWidgetContents);
    e->setGeometry(100, 50,500, 500);
    e->hide();
}
Tab_NS_Monitor::~Tab_NS_Monitor()
{
    delete ui;
}
void Tab_NS_Monitor::update_MDS()
{
    Common* common = Common::instance();
    for(int i=0;i<(int)cards.size();i++)
        delete cards[i];
    cards.clear();
    if(common->md->mdsm.mdsconnectivity.size()==0 )
        return;
    int r = 0;
    int c = 0;
    for(auto it=common->md->mdsm.mdsconnectivity.begin();it!=common->md->mdsm.mdsconnectivity.end();it++)
    {
        std::string real_RR="";
        std::string real_VT="";
        std::string real_IE="";
        std::string real_MV="";
        std::string querystr2 = "channel_id MATCH '";
        querystr2.append(it->second.vmd_id);
        querystr2.append("'");
        dds::sub::cond::QueryCondition cond2(
                    dds::sub::Query(common->devcon_reader, querystr2),
                    dds::sub::status::DataState(
                    dds::sub::status::SampleState::any(),
                    dds::sub::status::ViewState::any(),
                    dds::sub::status::InstanceState::alive()));
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples2 = common->devcon_reader.select().condition(cond2).read();
        if(samples2.length() > 0)
        {
            std::string querystr = "patient_id MATCH '";
            querystr.append(it->second.patient_id);
            querystr.append("'");
            dds::sub::cond::QueryCondition cond(
                        dds::sub::Query(common->observation_reader, querystr),
                        dds::sub::status::DataState(
                        dds::sub::status::SampleState::any(),
                        dds::sub::status::ViewState::any(),
                        dds::sub::status::InstanceState::alive()));
            dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples = common->observation_reader.select().condition(cond).read();
            if(samples.length() > 0)
            {
                std::multimap<std::string, mc_entry> real_time;
                float vt=0;
                float rr=0;
                float mv=0;
                for(auto& sample : samples)
                {
                    if(sample.info().valid())
                    {
                        dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
                        mc_entry entry;
                        entry.desc= data.value<std::string>("description");
                        entry.val=data.value<float>("value");
                        real_time.emplace(entry.desc, entry);
                    }
                float I_Part;
                float E_Part;
                bool has_I,has_E,has_RR,has_VT=0;
                auto it = real_time.find("I:E I-Part");
                if(it != real_time.end())
                    {
                    has_I = 1;
                    I_Part = it->second.val;
                    }
                it = real_time.find("I:E E-Part");
                if(it != real_time.end())
                    {
                    has_E = 1;
                    E_Part= it->second.val;
                    }
                if(has_I ==1&& has_E==1)
                    {
                    char tbuf[64];
                    sprintf(tbuf, "%.1f:%.1f", I_Part, E_Part);
                    real_IE=tbuf;
                    }
                it = real_time.find("Respiratory rate");
                if(it != real_time.end())
                    {
                    has_RR = 1;
                    rr=it->second.val;
                    QString new_rr =QString::number(rr, 'f', 1);
                    real_RR= new_rr.toStdString();
                    }
                it = real_time.find("Tidal volume in mL");
                if(it != real_time.end())
                    {
                    has_VT = 1;
                    vt=it->second.val;
                    QString new_VT =QString::number(vt, 'f', 1);
                    real_VT= new_VT.toStdString();
                    }
                if(has_RR ==1&& has_VT==1)
                    {
                    mv=(vt/1000.0)*rr;
                    QString new_mv= QString::number(mv, 'f', 2);
                    real_MV= new_mv.toStdString();
                    }
                 }

            }
            //????????????real-time??????
        }

        Manager_MDSConnectivity_Card *card = new Manager_MDSConnectivity_Card(ui->scrollAreaWidgetContents);
        card->setGeometry(Card_spacing+(Card_width+Card_spacing)*c,
                                       (Card_height+Card_spacing)*r,
                          Card_width, Card_height);
        std::string querystr3 = "patient_id MATCH '";
        querystr3.append(it->second.patient_id);
        querystr3.append("'");
        dds::sub::cond::QueryCondition cond3(
                    dds::sub::Query(common->patient_reader, querystr3),
                    dds::sub::status::DataState(
                    dds::sub::status::SampleState::any(),
                    dds::sub::status::ViewState::any(),
                    dds::sub::status::InstanceState::alive()));
        dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> patient_samples = common->patient_reader.select().condition(cond3).read();
        if(patient_samples.length() == 0)
            card->set_id("");
        else
            card->set_id(it->second.patient_id);
        card->set_bed(it->second.bed_id);
        card->set_vmd(it->second.vmd_id);
        card->set_RR(real_RR);
        card->set_IE(real_IE);
        card->set_MV(real_MV);
        card->set_VT(real_VT);

        if(common->md->pm.patients.size()!=0)
        {
            auto it2 = common->md->pm.patients.find(it->second.bed_id);
            if(it2->second.gender.compare("female") == 0)
                card->set_is_male(0);
            else
                card->set_is_male(1);
        }
        Common::set_inactive_colors(card);
        card->update_but();
        card->show();
        connect(card, SIGNAL(clicked()), this, SLOT(patient_clicked()));
        c++;
        if(c >= 5)
        {
            r++;
            c = 0;
        }

        cards.push_back(card);
    }
}
void Tab_NS_Monitor::patient_clicked()
{
    if(selected_patient != -1)
        return;
    Common* common = Common::instance();
    if(common->md->pm.patients.size()==0) return;
    int i;
    for(i=0;i<(int)cards.size();i++)
    {
        if(cards[i] == sender())
            break;
    }
    if(i==(int)cards.size())
        return;
    selected_patient = i;
    update();
    on_itemSelectionChanged();
}
void Tab_NS_Monitor::on_itemSelectionChanged()
{
    Common* common = Common::instance();
    common->mapping_tab->ui->releasebtn->setEnabled(true);
    for(int k=0;k<(int)cards.size();k++)
    {
        if(k==selected_patient)
        {
            cards[k]->set_selected(1);
            common->vmd_id = cards[k]->get_vmd();
        }
        else
            cards[k]->set_selected(0);
    }
/*    int currentRow = selected_patient+1;
    QString bed;
    if(currentRow<10)
        bed = "00";
    else
        bed = "0";
    bed.append(QString::number(currentRow));*/
    QJsonArray data = Set_bed;
    QString bed = data[selected_patient].toString();

    auto it=common->md->pm.patients.find(bed.toStdString());
    if(it->second.gender.compare("female") == 0)
        common->mapping_tab->ui->pic_label->setPixmap(QPixmap(":/icons/female.png"));
    else
        common->mapping_tab->ui->pic_label->setPixmap(QPixmap(":/icons/male.png"));
    common->mapping_tab->ui->bid_label->setText(it->second.bed.c_str());
    common->mapping_tab->ui->mrn_label->setText(it->second.mrn.c_str());
    common->mapping_tab->ui->familyName_label->setText(it->second.family_name.c_str());
    common->mapping_tab->ui->birthday_label->setText(it->second.bod.c_str());
    if(it->second.gender.compare("male") == 0)
        common->mapping_tab->ui->sex_label->setText("???");
    else if(it->second.gender.compare("female") == 0)
        common->mapping_tab->ui->sex_label->setText("???");
    else if(it->second.gender.compare("other") == 0)
        common->mapping_tab->ui->sex_label->setText("??????");
    else
        common->mapping_tab->ui->sex_label->setText("??????");

    common->md->ui->mRNLineEdit->setText(it->second.mrn.c_str());
    common->md->ui->bIDLineEdit->setText(it->second.bed.c_str());
    common->md->ui->familyNameLineEdit->setText(it->second.family_name.c_str());
    common->patient_id = it->second.id;

    common->mapping_tab->mapping_UI_reset();

    emit bindingChanged();
    common->md->ui->nav->set_enabled(1);
    common->bed_id = it->second.bed;

    std::string dummy;
    std::string sql = "SELECT meta(_).id FROM _ WHERE data_source='MDSConnectivity' AND vmd_id='";
    sql.append(common->vmd_id);
    sql.append("'");
    cbl::ResultSet results = common->cbl->queryDocuments(common->db, sql, dummy);
    int error=0;while (dummy!="IP200"&&error<5)
        {
        results = common->cbl->queryDocuments(common->db, sql, dummy);
        qDebug()<<QString::fromStdString(dummy);
        fflog_out(common->log,dummy.c_str());error++;
        }
    for(auto& result: results)
    {
        std::string id = result.valueAtIndex(0).asstring();
        cbl::Document d = common->db.getDocument(id);
        common->db.deleteDocument(d);
    }

    common->md->ui->main_stackedWidget->setCurrentIndex(1);
    common->observation_widget_page->changeToPrevPage();
    common->md->ui->nav->set_current_tab(3);
    std::string dummy1;
    std::string sql1 = "SELECT checked FROM _ WHERE data_source='NumericDeviceSelection' AND patient_id='";
    sql1.append(common->patient_id);
    sql1.append("' AND meta(_).expiration IS NOT VALUED AND expired=0");
    cbl::ResultSet results1 = common->cbl->queryDocuments(common->display_items_db, sql1, dummy);;
    error=0;while (dummy!="IP200"&&error<5)
        {
        results1 = common->cbl->queryDocuments(common->display_items_db, sql1, dummy);
        qDebug()<<QString::fromStdString(dummy);
        fflog_out(common->log,dummy.c_str());error++;
        }
    for(auto& result: results1)
    {
        uint8_t checked = result.valueAtIndex(0).asUnsigned();
        if (!checked)
        {
            common->device_settings_page->has_checked=0;
            common->observation_widget_page->changeToMetricItemsDisplayConfigPage();
        }
    }
    common->md->ui->nav->repaint();
}

