#include "Tab_Observations_historyPage_Widget.h"
#include "Tab_Observations_mainPage_Widget.h"
#include "qtimer.h"
#include "ui_Tab_Observations_historyPage_Widget.h"
#include <QScrollBar>
#include "Common.h"
#include <QButtonGroup>
#include "fleece/slice.hh"


Tab_Observations_historyPage_Widget::Tab_Observations_historyPage_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab_Observations_historyPage_Widget),
    update_timer(this),
    worker(this)
{
    ui->setupUi(this);
    Common* common = Common::instance();
    common->history_page = this;
    ui->chart->set_scrollable(1);
    ui->chart->set_num_labels_y(5);
    ui->chart->set_num_labels_x(5);
    ui->chart->set_min_zoom(30*60*1000);
    ui->chart->set_max_zoom(01*10*1000);
    ui->chart->set_axis_visible(1);
    ui->chart->set_zoomable(0);
    ui->RangeButton->hide();
    ui->chart->set_selection_width(40);
    ui->chart->set_selection_type(MC_SELECT_POINT);
    right_locked = 1;
    reposition = 0;
    jumper_last_trigger_time = 0;
    jumper_state_switch_time = 0;
    connect(&update_timer, SIGNAL(timeout()), this, SLOT(update_triggered()));
    connect(&worker, SIGNAL(timeout()), this, SLOT(on_worker()));
    connect(ui->chart, SIGNAL(on_point_select(int,uint64_t,float,int,int)), this, SLOT(on_point_select(int,uint64_t,float,int,int)));
/*
    ui->chart->set_scrollable(1);
    ui->chart->set_num_labels_y(5);
    ui->chart->set_num_labels_x(5);
    ui->chart->set_view_range_min_y(-100.0f);
    ui->chart->set_view_range_max_y(100.0f);
    ui->chart->set_axis_visible(1);
    ui->chart->set_selection_width(40);
    ui->chart->set_selection_type(MC_SELECT_POINT);

    connect(ui->chart, SIGNAL(on_series_select(int)), this, SLOT(on_series_select(int)));
    connect(ui->chart, SIGNAL(on_point_select(int,uint64_t,float,int,int)), this, SLOT(on_point_select(int,uint64_t,float,int,int)));
    current_time = ((uint64_t)1663667999-(8*60*60)+12*60*60)*1000;
    test_timer = new QTimer(this);
    connect(test_timer, SIGNAL(timeout()), this, SLOT(test_timer_triggered()));
    test_timer->start(2000);*/
    connect(ui->returnbtn, SIGNAL(clicked()), this, SLOT(on_return_pushButton_clicked()));
    //connect(ui->hour_up, SIGNAL(clicked()), this, SLOT(on_hour_up_clicked()));
    //connect(ui->hour_down, SIGNAL(clicked()), this, SLOT(on_hour_down_clicked()));
    //connect(ui->min_up, SIGNAL(clicked()), this, SLOT(on_min_up_clicked()));
    //connect(ui->min_down, SIGNAL(clicked()), this, SLOT(on_min_down_clicked()));
    ui->current_table->setShowGrid(0);
    ui->selection_table->setShowGrid(0);
    ui->selection_table->setColumnWidth(0, 500);
    ui->current_table->setColumnWidth(0, 500);
    reset_time_jumper();
    ui->chart->set_view_range_max_y(1000);
    ui->chart->set_view_range_min_y(0);
}

void Tab_Observations_historyPage_Widget::on_worker()
{
    Common* common = Common::instance();
    uint64_t now = time(NULL);
    if(ui->chart->get_view_range_max_x() != ui->chart->get_right_bounds() &&
       ui->chart->get_view_range_max_x() != ui->chart->get_custom_right_bound() && !reposition)
        right_locked = 0;
    if(!right_locked)
    {
        if(common->is_server)  //@NS
        {
            if((ui->chart->get_view_range_max_x() == ui->chart->get_right_bounds() ||
               ui->chart->get_view_range_max_x() == ui->chart->get_custom_right_bound()) &&
               ui->chart->get_right_bounds() >= (now-2)*1000)
                right_locked = 1;
            else if(ui->chart->get_view_range_min_x() < last_left_bound + 0.5*60*1000)
            {
                ui->chart->set_custom_left_bound(last_left_bound - (m_history_time)*60*1000);
                uint64_t custom_right_bound = last_left_bound - (m_history_time)*60*1000 + (m_history_time*2+1)*60*1000;
                if(custom_right_bound > now*1000)
                    custom_right_bound = now*1000;
                ui->chart->set_custom_right_bound(custom_right_bound);
                char timebuf[64];
                char timebuf1[64];
                sprintf(timebuf, "%llu", last_left_bound/1000 - (m_history_time)*60);
                sprintf(timebuf1, "%llu", last_left_bound/1000 + (m_history_time+1)*60);
                std::multimap<uint64_t, QJsonArray> vals;
                QJsonArray array;
                int line_break_delta;
                if(common->history_datasource=="Observation")
                    {
                    line_break_delta=common->Obs_line_break_delta;
                    ui->chart->set_line_break_delta(common->Obs_line_break_delta);
                    if(common->history_mdccode=="MATECARES_RSI")
                        array =common->Restful_API_RRandVT(timebuf,timebuf1,"Observation",common->history_model,0);
                    else if(common->history_mdccode=="MATECARES_MV")
                        array =common->Restful_API_RRandVT(timebuf,timebuf1,"Observation",common->history_model,1);
                    else
                        array =common->Restful_API(timebuf,timebuf1,"Observation");
                    }
                else
                    {
                    line_break_delta=common->RTO_line_break_delta;
                    ui->chart->set_line_break_delta(common->RTO_line_break_delta);
                    array =common->Restful_API(timebuf,timebuf1,"RTObservation");
                    }
                for (int i=0; i<array.count();++i)
                {
                    QJsonValue value = array.at(i).toObject();
                    QJsonValue source_timestamp =value["source_timestamp"];
                    uint32_t sec = source_timestamp["sec"].toInteger();
                    uint32_t nanosec = source_timestamp["nanosec"].toInteger();
                    uint64_t key = (uint64_t)sec*1000 + nanosec/1000000;
                    QJsonArray values;
                    if(common->history_datasource=="Observation")
                    {
                        float val = value["value"].toDouble();
                                            values.append(val);
                    }
                    else
                        values = value["values"].toArray();
                    vals.emplace(key, values);
                }
                std::multimap<uint64_t, float> new_pts;
                for(auto it=vals.begin();it!=vals.end();it++)
                {
                    if(it->second.count() == 0)
                        continue;
                    auto next = it;
                    next++;
                    if(next==vals.end()  || next->first - it->first > line_break_delta)
                    {
                        new_pts.emplace(it->first, it->second[0].toDouble());
                    }
                    else
                    {
                        uint64_t delta = (next->first - it->first)/it->second.count();
                        for(unsigned i=0;i<it->second.count();i++)
                        {
                            new_pts.emplace(it->first + delta*i, it->second[i].toDouble());
                        }
                    }
                }
                uint64_t a = ui->chart->get_view_range_min_x();
                uint64_t b = ui->chart->get_view_range_max_x();
                //ui->chart->clear_points(0);
                for(auto it=new_pts.begin();it!=new_pts.end();it++)
                    ui->chart->add_point(0, it->first, it->second);
                ui->chart->set_view_range_min_x(a);
                ui->chart->set_view_range_max_x(b);
                last_left_bound = ui->chart->get_custom_left_bound();
                last_right_bound = ui->chart->get_custom_right_bound();
            }
            else if(ui->chart->get_view_range_max_x() > last_right_bound - 0.5*60*1000)
            {                

                uint64_t new_right_bounds = last_right_bound + 30*1000;

                if(new_right_bounds > now*1000)
                     new_right_bounds = now*1000;

                ui->chart->set_custom_left_bound(new_right_bounds - (m_history_time)*60*1000);
                ui->chart->set_custom_right_bound(new_right_bounds);
                std::multimap<uint64_t, float> new_pts;
                //收集資料及畫圖
                {
                char timebuf[64];
                char timebuf1[64];
                sprintf(timebuf, "%llu", new_right_bounds/1000 - (m_history_time)*60);
                sprintf(timebuf1, "%llu", new_right_bounds/1000);
                std::multimap<uint64_t, QJsonArray> vals;
                QJsonArray array;
                int line_break_delta;
                if(common->history_datasource=="Observation")
                    {
                    line_break_delta=common->Obs_line_break_delta;
                    ui->chart->set_line_break_delta(common->Obs_line_break_delta);
                    if(common->history_mdccode=="MATECARES_RSI")
                        array =common->Restful_API_RRandVT(timebuf,timebuf1,"Observation",common->history_model,0);
                    else if(common->history_mdccode=="MATECARES_MV")
                        array =common->Restful_API_RRandVT(timebuf,timebuf1,"Observation",common->history_model,1);
                    else
                        array =common->Restful_API(timebuf,timebuf1,"Observation");
                    }
                else
                    {
                    line_break_delta=common->RTO_line_break_delta;
                    ui->chart->set_line_break_delta(common->RTO_line_break_delta);
                    array =common->Restful_API(timebuf,timebuf1,"RTObservation");
                    }
                for (int i=0; i<array.count();++i)
                {
                    QJsonValue value = array.at(i).toObject();
                    QJsonValue source_timestamp =value["source_timestamp"];
                    uint32_t sec = source_timestamp["sec"].toInteger();
                    uint32_t nanosec = source_timestamp["nanosec"].toInteger();
                    uint64_t key = (uint64_t)sec*1000 + nanosec/1000000;
                    QJsonArray values;
                    if(common->history_datasource=="Observation")
                    {
                        float val = value["value"].toDouble();
                                            values.append(val);

                    }
                    else
                        values = value["values"].toArray();
                    vals.emplace(key, values);
                }
                for(auto it=vals.begin();it!=vals.end();it++)
                {
                    if(it->second.count() == 0)
                        continue;
                    auto next = it;
                    next++;
                    if(next==vals.end() || next->first - it->first > line_break_delta)
                    {
                        new_pts.emplace(it->first, it->second[0].toDouble());
                    }
                    else
                    {
                        uint64_t delta = (next->first - it->first)/it->second.count();
                        for(unsigned i=0;i<it->second.count();i++)
                        {
                            new_pts.emplace(it->first + delta*i, it->second[i].toDouble());
                        }
                    }
                }
                }
                uint64_t a = ui->chart->get_view_range_min_x();
                uint64_t b = ui->chart->get_view_range_max_x();
                //ui->chart->clear_points(0);
                for(auto it=new_pts.begin();it!=new_pts.end();it++)
                    ui->chart->add_point(0, it->first, it->second);
                ui->chart->set_view_range_min_x(a);
                ui->chart->set_view_range_max_x(b);
                last_left_bound = ui->chart->get_custom_left_bound();
                last_right_bound = ui->chart->get_custom_right_bound();
            }
        }
        else//@BS
        {
            if((ui->chart->get_view_range_max_x() == ui->chart->get_right_bounds() ||
               ui->chart->get_view_range_max_x() == ui->chart->get_custom_right_bound()) &&
               ui->chart->get_right_bounds() >= (now-2)*1000)
                right_locked = 1;
            else if(ui->chart->get_view_range_min_x() < last_left_bound + 30*1000)
            {
                int line_break_delta;
                if(common->history_datasource=="Observation")
                    {
                    line_break_delta=common->Obs_line_break_delta;
                    ui->chart->set_line_break_delta(common->Obs_line_break_delta);
                    }
                else
                    {
                    line_break_delta=common->RTO_line_break_delta;
                    ui->chart->set_line_break_delta(common->RTO_line_break_delta);
                    }
                ui->chart->set_custom_left_bound(last_left_bound - (m_history_time)*60*1000);
                uint64_t custom_right_bound = last_left_bound - (m_history_time)*60*1000 + (m_history_time*2+1)*60*1000;
                if(custom_right_bound > now*1000)
                    custom_right_bound = now*1000;
                ui->chart->set_custom_right_bound(custom_right_bound);
                char timebuf[64];
                char timebuf1[64];
                sprintf(timebuf, "%llu", last_left_bound/1000 - (m_history_time)*60);
                sprintf(timebuf1, "%llu", last_left_bound/1000 + (m_history_time+1)*60);
                std::string dummy;
                std::string sql = "SELECT source_timestamp.sec,source_timestamp.nanosec,values,value,description FROM _ WHERE data_source='";
                sql.append(common->history_datasource);
                sql.append("' AND source_timestamp.sec>=");
                sql.append(timebuf);
                sql.append(" AND source_timestamp.sec<=");
                sql.append(timebuf1);
                if(common->history_mdccode=="MATECARES_RSI"||common->history_mdccode=="MATECARES_MV")
                {
                    sql.append(" AND (mdc_code='DRAEGER_MEASURED_CP1_TidalVolume' OR mdc_code='MDC_RESP_RATE') ");
                }
                else
                    {
                    sql.append(" AND mdc_code='");
                    sql.append(common->history_mdccode);
                    sql.append("' ");
                    }
                sql.append("AND patient_id='");
                sql.append(common->patient_id);
                sql.append("' AND model='");
                sql.append(common->history_model);
                sql.append("'");
                cbl::ResultSet results = common->cbl->queryDocuments(common->db, sql, dummy);
                int error=0;while (dummy!="IP200"&&error<5)
                    {
                    results = common->cbl->queryDocuments(common->db, sql, dummy);
                    qDebug()<<QString::fromStdString(dummy);
                    fflog_out(common->log,dummy.c_str());error++;
                    }
    //           printf("unlocked query took %u milliseconds.\n", query_end-query_start);*/

                std::multimap<uint64_t, std::vector<float>> vals;
                std::map<uint64_t,float> RR;
                std::map<uint64_t,float> VT;
                for(auto& result: results)
                {
                    if(common->history_mdccode=="MATECARES_RSI"||common->history_mdccode=="MATECARES_MV")
                    {
                        int32_t sec = result.valueAtIndex(0).asInt();
                        uint32_t nanosec = result.valueAtIndex(1).asUnsigned();
                        uint64_t key = (uint64_t)sec*1000 + nanosec/1000000;
                        float val = result.valueAtIndex(3).asFloat();
                        std::string desc = result.valueAtIndex(4).asstring();
                        if (desc=="Respiratory rate")
                            RR.emplace(key,val);
                        else if(desc=="Tidal volume in mL")
                            VT.emplace(key,val);
                        else if(desc=="Tidal volume")
                            VT.emplace(key,val);
                    }
                    else
                    {
                        int32_t sec = result.valueAtIndex(0).asInt();
                        uint32_t nanosec = result.valueAtIndex(1).asUnsigned();
                        fleece::Array values;
                        std::vector<float> value;
                        float val;
                        if(common->history_datasource=="RTObservation")
                        {
                            values = result.valueAtIndex(2).asArray();
                            for(int i=0;i<(int)values.count();++i)
                                value.push_back(values[i].asFloat());
                        }
                        else
                        {
                            val = result.valueAtIndex(3).asFloat();
                            value.push_back(val);

                        }
                        uint64_t key = (uint64_t)sec*1000 + nanosec/1000000;
                        vals.emplace(key, value);
                    }
                }
                if(common->history_mdccode=="MATECARES_RSI"||common->history_mdccode=="MATECARES_MV")
                {
                    for (auto it=RR.begin();it!=RR.end();++it)
                        {
                        auto it2=VT.find(it->first);
                        if(it2==VT.end())continue;

                        float RR_val=it->second;
                        float VT_val=it2->second;
                        float return_val;
                        if(common->history_mdccode=="MATECARES_MV")
                            return_val=(VT_val/1000.0)*RR_val;
                        else
                            return_val=VT_val/RR_val;
                        std::vector<float> value;
                        value.push_back(return_val);
                        vals.emplace(it->first,value);
                    }
                }
                std::multimap<uint64_t, float> new_pts;
                for(auto it=vals.begin();it!=vals.end();it++)
                {
                    if(it->second.size() == 0)
                        continue;
                    auto next = it;
                    next++;
                    if(next==vals.end()  || next->first - it->first > LINE_BREAK_DELTA)
                    {
                        new_pts.emplace(it->first, it->second[0]);
                    }
                    else
                    {
                        uint64_t delta = (next->first - it->first)/it->second.size();
                        for(unsigned i=0;i<it->second.size();i++)
                        {
                            new_pts.emplace(it->first + delta*i, it->second[i]);
                        }
                    }
                }
                uint64_t a = ui->chart->get_view_range_min_x();
                uint64_t b = ui->chart->get_view_range_max_x();
                ui->chart->clear_points(0);
                for(auto it=new_pts.begin();it!=new_pts.end();it++)
                    {
                    ui->chart->add_point(0, it->first, it->second);
                    }
                ui->chart->set_view_range_min_x(a);
                ui->chart->set_view_range_max_x(b);
                last_left_bound = ui->chart->get_custom_left_bound();
                last_right_bound = ui->chart->get_custom_right_bound();
            }
            else if(ui->chart->get_view_range_max_x() > last_right_bound - 30*1000)
            {
                int line_break_delta;
                if(common->history_datasource=="Observation")
                    {
                    line_break_delta=common->Obs_line_break_delta;
                    ui->chart->set_line_break_delta(common->Obs_line_break_delta);
                    }
                else
                    {
                    line_break_delta=common->RTO_line_break_delta;
                    ui->chart->set_line_break_delta(common->RTO_line_break_delta);
                    }
                uint64_t new_right_bounds = last_right_bound + 30*1000;
                if(new_right_bounds > now*1000)
                    new_right_bounds = now*1000;
                ui->chart->set_custom_left_bound(new_right_bounds - (m_history_time)*60*1000);
                ui->chart->set_custom_right_bound(new_right_bounds);
                std::string dummy;
                std::string sql = "SELECT source_timestamp.sec,source_timestamp.nanosec,values,value,description FROM _ WHERE data_source='";
                sql.append(common->history_datasource);
                sql.append("' AND source_timestamp.sec>=");
                char timebuf[64];
                sprintf(timebuf, "%llu", new_right_bounds/1000 - (m_history_time)*60);
                sql.append(timebuf);
                sql.append(" AND source_timestamp.sec<=");
                sprintf(timebuf, "%llu", new_right_bounds/1000);
                sql.append(timebuf);
                if(common->history_mdccode=="MATECARES_RSI"||common->history_mdccode=="MATECARES_MV")
                {
                    sql.append(" AND (mdc_code='DRAEGER_MEASURED_CP1_TidalVolume' OR mdc_code='MDC_RESP_RATE') ");
                }
                else
                    {
                    sql.append(" AND mdc_code='");
                    sql.append(common->history_mdccode);
                    sql.append("' ");
                    }
                sql.append("AND patient_id='");
                sql.append(common->patient_id);
                sql.append("' AND model='");
                sql.append(common->history_model);
                sql.append("' AND vmd_id='");
                sql.append(common->vmd_id);
                sql.append("'");
    //            uint32_t query_start = Common::get_time_ms();
                cbl::ResultSet results = common->cbl->queryDocuments(common->db, sql, dummy);
                int error=0;while (dummy!="IP200"&&error<5)
                    {
                    results = common->cbl->queryDocuments(common->db, sql, dummy);
                    qDebug()<<QString::fromStdString(dummy);
                    fflog_out(common->log,dummy.c_str());error++;
                    }

    //            uint32_t query_end = Common::get_time_ms();
    //            printf("unlocked query took %u milliseconds.\n", query_end-query_start);

                std::multimap<uint64_t, std::vector<float>> vals;
                std::map<uint64_t,float> RR;
                std::map<uint64_t,float> VT;
                for(auto& result: results)
                {
                    if(common->history_mdccode=="MATECARES_RSI"||common->history_mdccode=="MATECARES_MV")
                    {
                        int32_t sec = result.valueAtIndex(0).asInt();
                        uint32_t nanosec = result.valueAtIndex(1).asUnsigned();
                        uint64_t key = (uint64_t)sec*1000 + nanosec/1000000;
                        float val = result.valueAtIndex(3).asFloat();
                        std::string desc = result.valueAtIndex(4).asstring();
                        if (desc=="Respiratory rate")
                            RR.emplace(key,val);
                        else if(desc=="Tidal volume in mL")
                            VT.emplace(key,val);
                        else if(desc=="Tidal volume")
                            VT.emplace(key,val);
                    }
                    else
                    {
                        int32_t sec = result.valueAtIndex(0).asInt();
                        uint32_t nanosec = result.valueAtIndex(1).asUnsigned();
                        fleece::Array values;
                        std::vector<float> value;
                        float val;
                        if(common->history_datasource=="RTObservation")
                        {
                            values = result.valueAtIndex(2).asArray();
                            for(int i=0;i<(int)values.count();++i)
                                value.push_back(values[i].asFloat());
                        }
                        else
                        {
                            val = result.valueAtIndex(3).asFloat();
                            value.push_back(val);

                        }
                        uint64_t key = (uint64_t)sec*1000 + nanosec/1000000;
                        vals.emplace(key, value);
                    }
                }
                if(common->history_mdccode=="MATECARES_RSI"||common->history_mdccode=="MATECARES_MV")
                {
                    for (auto it=RR.begin();it!=RR.end();++it)
                        {
                        auto it2=VT.find(it->first);
                        if(it2==VT.end())continue;

                        float RR_val=it->second;
                        float VT_val=it2->second;
                        float return_val;
                        if(common->history_mdccode=="MATECARES_MV")
                            return_val=(VT_val/1000.0)*RR_val;
                        else
                            return_val=VT_val/RR_val;
                        std::vector<float> value;
                        value.push_back(return_val);
                        vals.emplace(it->first,value);
                    }
                }
                std::multimap<uint64_t, float> new_pts;
                for(auto it=vals.begin();it!=vals.end();it++)
                {
                    if(it->second.size() == 0)
                        continue;
                    auto next = it;
                    next++;
                    if(next==vals.end() || next->first - it->first > LINE_BREAK_DELTA)
                    {
                        new_pts.emplace(it->first, it->second[0]);
                    }
                    else
                    {
                        uint64_t delta = (next->first - it->first)/it->second.size();
                        for(unsigned i=0;i<it->second.size();i++)
                        {
                            new_pts.emplace(it->first + delta*i, it->second[i]);
                        }
                    }
                }
                uint64_t a = ui->chart->get_view_range_min_x();
                uint64_t b = ui->chart->get_view_range_max_x();
                ui->chart->clear_points(0);
                for(auto it=new_pts.begin();it!=new_pts.end();it++)
                    {
                    ui->chart->add_point(0, it->first, it->second);
                    }
                ui->chart->set_view_range_min_x(a);
                ui->chart->set_view_range_max_x(b);
                last_left_bound = ui->chart->get_custom_left_bound();
                last_right_bound = ui->chart->get_custom_right_bound();
            }
        }
    }
}

void Tab_Observations_historyPage_Widget::update_triggered()
{
    Common* common = Common::instance();
    if(right_locked)
    {
        if(common->is_server)//NS
        {
            uint64_t now = time(NULL);
            ui->chart->set_custom_right_bound(now*1000);
            ui->chart->set_custom_left_bound((now-m_history_time*60)*1000);
            now -=m_history_time*60;
            char timebuf[64];
            char timebuf1[64];
            sprintf(timebuf, "%llu", now);
            sprintf(timebuf1, "%llu", now+m_history_time*60);
            std::multimap<uint64_t, QJsonArray> vals;
            QJsonArray array;
            int line_break_delta;
            if(common->history_datasource=="Observation")
                {
                line_break_delta=common->Obs_line_break_delta;
                ui->chart->set_line_break_delta(common->Obs_line_break_delta);
                if(common->history_mdccode=="MATECARES_RSI")
                    array =common->Restful_API_RRandVT(timebuf,timebuf1,"Observation",common->history_model,0);
                else if(common->history_mdccode=="MATECARES_MV")
                    array =common->Restful_API_RRandVT(timebuf,timebuf1,"Observation",common->history_model,1);
                else
                    array =common->Restful_API(timebuf,timebuf1,"Observation");
                }
            else
                {
                line_break_delta=common->RTO_line_break_delta;
                ui->chart->set_line_break_delta(common->RTO_line_break_delta);
                array =common->Restful_API(timebuf,timebuf1,"RTObservation");
                }
            for (int i=0; i<array.count();++i)
            {
                QJsonValue value = array.at(i).toObject();
                QJsonValue source_timestamp =value["source_timestamp"];
                uint32_t sec = source_timestamp["sec"].toInteger();
                uint32_t nanosec = source_timestamp["nanosec"].toInteger();
                uint64_t key = (uint64_t)sec*1000 + nanosec/1000000;
                QJsonArray values;
                if(common->history_datasource=="Observation")
                {
                    float val = value["value"].toDouble();
                    values.append(val);
                }
                else
                    values = value["values"].toArray();
                vals.emplace(key, values);
            }
            std::multimap<uint64_t, float> new_pts;
            for(auto it=vals.begin();it!=vals.end();it++)
            {
                if(it->second.count() == 0)
                    continue;
                auto next = it;
                next++;
                if(next==vals.end() || next->first - it->first > common->RTO_line_break_delta)
                {
                    new_pts.emplace(it->first, it->second[0].toDouble());
                }
                else
                {
                    uint64_t delta = (next->first - it->first)/it->second.count();
                    for(unsigned i=0;i<it->second.count();i++)
                    {
                        new_pts.emplace(it->first + delta*i, it->second[i].toDouble());
                    }
                }
            }
            ui->chart->clear_points(0);
            for(auto it=new_pts.begin();it!=new_pts.end();it++)
                {
                ui->chart->add_point(0, it->first, it->second);
                }
            uint64_t viewable = ui->chart->get_view_range_max_x() - ui->chart->get_view_range_min_x();
            ui->chart->set_view_range_max_x(ui->chart->get_right_bounds());
            ui->chart->set_view_range_min_x(ui->chart->get_right_bounds() - viewable);
            last_right_bound = ui->chart->get_right_bounds();
            //last_left_bound = ui->chart->get_right_bounds()-180*1000;

        }
        else//BS
        {

            uint64_t now = time(NULL);
            ui->chart->set_custom_right_bound(now*1000);
            ui->chart->set_custom_left_bound((now-m_history_time*60)*1000);
            now -=m_history_time*60;
            char timebuf[64];
            sprintf(timebuf, "%llu", now);
            std::string dummy;
            std::string sql = "SELECT source_timestamp.sec,source_timestamp.nanosec,values,value,description FROM _ WHERE data_source='";
            sql.append(common->history_datasource);
            sql.append("' AND source_timestamp.sec>=");
            sql.append(timebuf);
            if(common->history_mdccode=="MATECARES_RSI"||common->history_mdccode=="MATECARES_MV")
            {
                sql.append(" AND (mdc_code='DRAEGER_MEASURED_CP1_TidalVolume' OR mdc_code='MDC_RESP_RATE') ");
            }
            else
                {
                sql.append(" AND mdc_code='");
                sql.append(common->history_mdccode);
                sql.append("' ");
                }
            sql.append("AND patient_id='");
            sql.append(common->patient_id);
            sql.append("' AND model='");
            sql.append(common->history_model);
            sql.append("' AND vmd_id='");
            sql.append(common->vmd_id);
            sql.append("'");
            uint32_t query_start = Common::get_time_ms();
            cbl::ResultSet results2 = common->cbl->queryDocuments(common->db, sql, dummy);
            int error=0;while (dummy!="IP200"&&error<5)
                {
                results2 = common->cbl->queryDocuments(common->db, sql, dummy);
                qDebug()<<QString::fromStdString(dummy);
                fflog_out(common->log,dummy.c_str());error++;
                }
            std::multimap<uint64_t, std::vector<float>> vals;
            std::map<uint64_t,float> RR;
            std::map<uint64_t,float> VT;
            for(auto& result: results2)
            {
                if(common->history_mdccode=="MATECARES_RSI"||common->history_mdccode=="MATECARES_MV")
                {
                    int32_t sec = result.valueAtIndex(0).asInt();
                    uint32_t nanosec = result.valueAtIndex(1).asUnsigned();
                    uint64_t key = (uint64_t)sec*1000 + nanosec/1000000;
                    float val = result.valueAtIndex(3).asFloat();
                    std::string desc = result.valueAtIndex(4).asstring();
                    if (desc=="Respiratory rate")
                        RR.emplace(key,val);
                    else if(desc=="Tidal volume in mL")
                        VT.emplace(key,val);
                    else if(desc=="Tidal volume")
                        VT.emplace(key,val);
                }
                else
                {
                    int32_t sec = result.valueAtIndex(0).asInt();
                    uint32_t nanosec = result.valueAtIndex(1).asUnsigned();
                    fleece::Array values;
                    std::vector<float> value;
                    float val;
                    if(common->history_datasource=="RTObservation")
                    {
                        values = result.valueAtIndex(2).asArray();
                        for(int i=0;i<(int)values.count();++i)
                            value.push_back(values[i].asFloat());
                    }
                    else
                    {
                        val = result.valueAtIndex(3).asFloat();
                        value.push_back(val);

                    }
                    uint64_t key = (uint64_t)sec*1000 + nanosec/1000000;
                    vals.emplace(key, value);
                }
            }
            if(common->history_mdccode=="MATECARES_RSI"||common->history_mdccode=="MATECARES_MV")
            {
                for (auto it=RR.begin();it!=RR.end();++it)
                    {
                    auto it2=VT.find(it->first);
                    if(it2==VT.end())continue;
                    float RR_val=it->second;
                    float VT_val=it2->second;
                    float return_val;
                    if(common->history_mdccode=="MATECARES_MV")
                        return_val=(VT_val/1000.0)*RR_val;
                    else
                        return_val=VT_val/RR_val;
                    std::vector<float> value;
                    value.push_back(return_val);
                    vals.emplace(it->first,value);
                }
            }
            std::multimap<uint64_t, float> new_pts;
            for(auto it=vals.begin();it!=vals.end();it++)
            {
                if(it->second.size() == 0)
                    continue;
                auto next = it;
                next++;
                if(next==vals.end() || next->first - it->first > 10000)
                {
                    new_pts.emplace(it->first, it->second[0]);
                }
                else
                {
                    uint64_t delta = (next->first - it->first)/it->second.size();
                    for(unsigned i=0;i<it->second.size();i++)
                    {
                        new_pts.emplace(it->first + delta*i, it->second[i]);
                    }
                }
            }
            ui->chart->clear_points(0);
            for(auto it=new_pts.begin();it!=new_pts.end();it++)
                {
                ui->chart->add_point(0, it->first, it->second);
                }
            uint64_t viewable = ui->chart->get_view_range_max_x() - ui->chart->get_view_range_min_x();
            ui->chart->set_view_range_max_x(ui->chart->get_right_bounds());
            ui->chart->set_view_range_min_x(ui->chart->get_right_bounds() - viewable);
            last_right_bound = ui->chart->get_right_bounds();
            //last_left_bound = ui->chart->get_right_bounds()-180*1000;
            last_left_bound = ui->chart->get_custom_left_bound();
        }
    }

    if(reposition)
    {
        if(ui->chart->get_num_points(0) < 2)
            goto observation;
        reposition = 0;
        /*
        ui->chart->set_view_range_max_x(ui->chart->get_right_bounds());
        ui->chart->set_view_range_min_x(ui->chart->get_right_bounds() - 60*1000);
        last_right_bound = ui->chart->get_right_bounds();
        last_left_bound = ui->chart->get_right_bounds() - 180*1000;
        ui->chart->set_custom_left_bound(last_left_bound);
        ui->chart->set_custom_right_bound(last_right_bound);*/
    }
observation:
    if(common->patient_id.size() == 0)
        return;
    //if(common->device_checkstate.size() == 0)
        common->populate_device_checkstate();
    //if(common->item_checkstate.size() == 0)
        common->populate_item_checkstate();

    std::string querystr = "vmd_id MATCH '";
    querystr.append(common->vmd_id);
    querystr.append("' AND patient_id MATCH '");
    querystr.append(common->patient_id);
    querystr.append("' AND model MATCH '");
    querystr.append(common->history_model);
    querystr.append("'");
    dds::sub::cond::QueryCondition qcond(
                dds::sub::Query(common->history_observation_reader, querystr),
                dds::sub::status::DataState(
                dds::sub::status::SampleState::any(),
                dds::sub::status::ViewState::any(),
                dds::sub::status::InstanceState::alive()));
    dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples = common->history_observation_reader.select().condition(qcond).read();
    std::multimap<int, mc_entry> entries;
    std::multimap<std::string, mc_entry> left_over;
    for(auto& sample : samples)
    {
        if(sample.info().valid())
        {
            dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
            mc_entry e;
            e.code = data.value<std::string>("code");
            e.desc = data.value<std::string>("description");
            e.unit = data.value<std::string>("unit");
            e.val = data.value<float>("value");
            e.abbv = data.value<std::string>("abbreviation");
            e.y_max = "200";
            rti::core::xtypes::LoanedDynamicData loaned_member = data.loan_value("source_timestamp");
            e.ts.tv_sec = loaned_member.get().value<int32_t>("sec");
            e.ts.tv_nsec = loaned_member.get().value<uint32_t>("nanosec");
            loaned_member.return_loan();
            if(common->item_checkstate.size() == 0)
            {
                auto it2 = common->special_items.find(common->history_model);
                if(it2 != common->special_items.end())
                {
                    int i=0;
                    for(;i<(int)it2->second.size();i++)
                    {
                        if(it2->second[i].compare(e.desc) == 0)
                            break;
                    }
                    if(i != (int)it2->second.size())
                        entries.emplace(i, e);
                    else
                        left_over.emplace(e.desc, e);
                }
            }
            else
            {
                std::string str = common->history_model;
                str.append(",");
                str.append(e.desc);
                auto it2 = common->item_checkstate.find(str);
                if(it2==common->item_checkstate.end())
                    {
                    entries.emplace(9999, e);
                    }
                else if(it2->second.checked)
                    {
                    entries.emplace(it2->second.order, e);
                    }
                else if(is_fold)
                    entries.emplace(common->item_checkstate.size()+1, e);
                else
                    left_over.emplace(e.desc, e);
                //if(it==common->item_checkstate.end() || it->second.checked)
                    //entries.emplace(code, e);
            }
        }
    }
    if(common->history_model.compare("Savina") == 0 ||
       common->history_model.compare("Savina 300") == 0)
    {
        common->remove_savina_items(&entries);
        common->add_savina_items(common->history_model, &entries, &left_over);
    }
    ui->current_table->clearContents();
    ui->current_table->setRowCount(entries.size());
    QString time= QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") ;
    QStringList InfHeader = (QStringList() << time << "Value");
    ui->current_table->setHorizontalHeaderLabels(InfHeader);
    int row=0;
    for(auto it=entries.begin();it!=entries.end();it++)
    {
        char tempbuf[128];
        QTableWidgetItem* item0;
        if(it->second.abbv.size() == 0)
            item0 = new QTableWidgetItem(it->second.desc.c_str());
        else
            item0 = new QTableWidgetItem(it->second.abbv.c_str());
        if(it->second.desc.compare("I:E Ratio") == 0)
            strcpy(tempbuf, it->second.code.c_str());
        else if(it->second.desc.compare("Flow peak") == 0)
        {
            it->second.unit = "L/min";
            it->second.val = (it->second.val*60.0)/1000.0;
            common->utils->mdcValueFormatter("Observation", it->second.code, it->second.val, it->second.unit, tempbuf);
        }
        else
            common->utils->mdcValueFormatter("Observation", it->second.code, it->second.val, it->second.unit, tempbuf);
        QTableWidgetItem* item1 = new QTableWidgetItem(tempbuf);
        ui->current_table->setItem(row, 0, item0);
        ui->current_table->setItem(row, 1, item1);
        item0->setFlags(Qt::ItemIsEnabled);
        item1->setFlags(Qt::ItemIsEnabled);
        if(row % 2 ==1)
            {
            item0->setBackground(QBrush(QColor(11, 42, 78)));
            item1->setBackground(QBrush(QColor(11, 42, 78)));
        }
        row++;
    }
    update_jumper_date();
}

void Tab_Observations_historyPage_Widget::test_timer_triggered()
{
    current_time += 60*60*1000;
    ui->chart->add_point(1, current_time, rand()%50);
}

void Tab_Observations_historyPage_Widget::on_series_select(int series_index)
{
    printf("series %d selected\n", series_index);
}

void Tab_Observations_historyPage_Widget::on_point_select(int series_index, uint64_t timestamp, float val, int screenspace_x, int screenspace_y)
{
    Common* common = Common::instance();

    timestamp /= 1000;
    uint64_t start_time = timestamp-10;
    char endbuf[64];
    char startbuf[64];
    sprintf(endbuf, "%llu", timestamp+1);
    sprintf(startbuf, "%llu", start_time);
    std::multimap<int, mc_entry> vals;
    std::multimap<std::string, mc_entry> left_over;
    if(common->is_server)
    {
        QJsonArray array = common->Restful_API_Orderby(startbuf,endbuf,"Observation",common->history_model,"source_timestamp.sec ASC");
        for (int i=0; i<array.count();++i)
        {
            QJsonValue value = array.at(i).toObject();
            mc_entry e;
            e.desc = value["description"].toString().toStdString();
            e.val = value["value"].toDouble();
            e.unit = value["unit"].toString().toStdString();
            e.code = value["code"].toString().toStdString();
            std::string str = common->history_model;
            str.append(",");
            str.append(e.desc);
            auto it2 = common->item_checkstate.find(str);
            if(it2==common->item_checkstate.end())
            {
                auto it3 = left_over.find(e.code);
                if(it3 != left_over.end())
                    left_over.erase(it3);
                left_over.emplace(e.code, e);
            }
            else if(it2->second.checked)
            {
                auto it3 = vals.find(it2->second.order);
                if(it3 != vals.end())
                    vals.erase(it3);
                vals.emplace(it2->second.order, e);
            }
            else
            {
                auto it3 = left_over.find(e.code);
                if(it3 != left_over.end())
                    left_over.erase(it3);
                left_over.emplace(e.code, e);
            }
        }
    }
    else
    {
        std::string dummy;
        std::string sql = "SELECT description,value,unit,code FROM _ WHERE data_source='Observation' AND model='";
        sql.append(common->history_model);
        sql.append("' AND vmd_id='");
        sql.append(common->vmd_id);
        sql.append("' AND patient_id='");
        sql.append(common->patient_id);
        sql.append("' AND source_timestamp.sec<");
        sql.append(endbuf);
        sql.append(" AND source_timestamp.sec>=");
        sql.append(startbuf);
        sql.append(" ORDER BY source_timestamp.sec ASC");
        cbl::ResultSet results2 = common->cbl->queryDocuments(common->db, sql, dummy);
        int error=0;while (dummy!="IP200"&&error<5)
            {
            results2 = common->cbl->queryDocuments(common->db, sql, dummy);
            qDebug()<<QString::fromStdString(dummy);
            fflog_out(common->log,dummy.c_str());error++;
            }

        for(auto& result: results2)
        {
            mc_entry e;
            e.desc = result.valueAtIndex(0).asstring();
            e.val = result.valueAtIndex(1).asFloat();
            e.unit = result.valueAtIndex(2).asstring();
            e.code = result.valueAtIndex(3).asstring();
            std::string str = common->history_model;
            str.append(",");
            str.append(e.desc);
            auto it2 = common->item_checkstate.find(str);
            if(it2==common->item_checkstate.end())
            {
                auto it3 = left_over.find(e.code);
                if(it3 != left_over.end())
                    left_over.erase(it3);
                left_over.emplace(e.code, e);
            }
            else if(it2->second.checked)
            {
                auto it3 = vals.find(it2->second.order);
                if(it3 != vals.end())
                    vals.erase(it3);
                vals.emplace(it2->second.order, e);
            }
            else
            {
                auto it3 = left_over.find(e.code);
                if(it3 != left_over.end())
                    left_over.erase(it3);
                left_over.emplace(e.code, e);
            }
        }
    }
    if(vals.size() == 0 && left_over.size() == 0)
    {
        ui->chart->clear_selection();
        ui->selection_table->clearContents();
        QStringList InfHeader = (QStringList() << "historical time" << "Value");
        ui->selection_table->setHorizontalHeaderLabels(InfHeader);
        return;
    }
    if(common->history_model.compare("Savina") == 0 ||
       common->history_model.compare("Savina 300") == 0)
        common->add_savina_items(common->history_model, &vals, &left_over);
    ui->selection_table->clearContents();
    ui->selection_table->setRowCount(vals.size()+left_over.size());
    int row=0;
    for(auto it=vals.begin();it!=vals.end();it++)
    {
        char tempbuf[128];
        QTableWidgetItem* item0 = new QTableWidgetItem(it->second.desc.c_str());
        common->utils->mdcValueFormatter("Observation", it->second.code, it->second.val, it->second.unit, tempbuf);
        QTableWidgetItem* item1 = new QTableWidgetItem(tempbuf);
        ui->selection_table->setItem(row, 0, item0);
        ui->selection_table->setItem(row, 1, item1);
        item0->setFlags(Qt::ItemIsEnabled);
        item1->setFlags(Qt::ItemIsEnabled);
        if(row % 2 ==1)
            {
            item0->setBackground(QBrush(QColor(11, 42, 78)));
            item1->setBackground(QBrush(QColor(11, 42, 78)));
        }
        row++;
    }

    for(auto it=left_over.begin();it!=left_over.end();it++)
    {
        char tempbuf[128];
        QTableWidgetItem* item0 = new QTableWidgetItem(it->second.desc.c_str());
        common->utils->mdcValueFormatter("Observation", it->second.code, it->second.val, it->second.unit, tempbuf);
        QTableWidgetItem* item1 = new QTableWidgetItem(tempbuf);
        ui->selection_table->setItem(row, 0, item0);
        ui->selection_table->setItem(row, 1, item1);
        item0->setFlags(Qt::ItemIsEnabled);
        item1->setFlags(Qt::ItemIsEnabled);
        if(row % 2 ==1)
            {
            item0->setBackground(QBrush(QColor(11, 42, 78)));
            item1->setBackground(QBrush(QColor(11, 42, 78)));
        }
        row++;
    }
    QString time= QDateTime::fromMSecsSinceEpoch(timestamp*1000).toString("yyyy-MM-dd hh:mm:ss") ;
    QStringList InfHeader = (QStringList() << time << "Value");
    ui->selection_table->setHorizontalHeaderLabels(InfHeader);
}

void Tab_Observations_historyPage_Widget::clear_selection()
{
    ui->chart->clear_selection();
    ui->selection_table->clearContents();
    QStringList InfHeader = (QStringList() << "historical time" << "Value");
    ui->selection_table->setHorizontalHeaderLabels(InfHeader);
}

Tab_Observations_historyPage_Widget::~Tab_Observations_historyPage_Widget()
{
    delete ui;
}

void Tab_Observations_historyPage_Widget::showEvent(QShowEvent *event)
{
    Common* common = Common::instance();
    update_timer.start(1000);
    worker.start(33);
    reposition = 1;
    //common->history_model = "Savina";
    //common->history_mdccode = "MDC_PRESS_AWAY";
//    if(common->history_model.compare("Savina") == 0)
//    {
//        if(common->history_mdccode.compare("MDC_PRESS_AWAY") == 0)
//        {
//            ui->chart->set_view_range_max_y(60.0f);
//            ui->chart->set_view_range_min_y(0.0f);
//            ui->chart->set_series_color(0, QColor(0xe8, 0xcc, 0xac));
//        }
//        else if(common->history_mdccode.compare("MDC_FLOW_AWAY") == 0)
//        {
//            ui->chart->set_view_range_max_y(100.0f);
//            ui->chart->set_view_range_min_y(-100.0f);
//            ui->chart->set_series_color(0, QColor(0x5c, 0xe5, 0x5f));
//        }
//        else if(common->history_mdccode.compare("FOYA_MEASURED_VolumeInspirationBegan") == 0)
//        {
//            ui->chart->set_view_range_max_y(1000);
//            ui->chart->set_view_range_min_y(0);
//            ui->chart->set_series_color(0, QColor(0xce, 0x5c, 0x00));
//        }
//    }
//    else if(common->history_model.compare("Savina 300") == 0)
//    {
//        if(common->history_mdccode.compare("MDC_PRESS_AWAY") == 0)
//        {
//            ui->chart->set_view_range_max_y(60.0f);
//            ui->chart->set_view_range_min_y(-5.0f);
//            ui->chart->set_series_color(0, QColor(0xe8, 0xcc, 0xac));
//        }
//        else if(common->history_mdccode.compare("MDC_FLOW_AWAY") == 0)
//        {
//            ui->chart->set_view_range_max_y(100.0f);
//            ui->chart->set_view_range_min_y(-100.0f);
//            ui->chart->set_series_color(0, QColor(0x5c, 0xe5, 0x5f));
//        }
//        else if(common->history_mdccode.compare("FOYA_MEASURED_VolumeInspirationBegan") == 0)
//        {
//            ui->chart->set_view_range_max_y(1000);
//            ui->chart->set_view_range_min_y(0);
//            ui->chart->set_series_color(0, QColor(0xce, 0x5c, 0x00));
//        }
//    }
    ui->chart->clear_points(0);
    ui->chart->clear_selection();
    pts.clear();
    right_locked = 1;
}

void Tab_Observations_historyPage_Widget::hideEvent(QHideEvent *event)
{
    update_timer.stop();
    worker.stop();
}

void Tab_Observations_historyPage_Widget::on_return_pushButton_clicked()
{
    emit changeToPrevPage();
}
void Tab_Observations_historyPage_Widget::on_RangeButton_clicked()
{
    Common* common = Common::instance();
    std::vector<btn> btn;
//    for(auto it=common->observation_main_page->legends.begin();it!=common->observation_main_page->legends.end();++it)
    {
        class btn b;
        b.name="10 secs";
        b.index=0;
        b.is_select=0;
        btn.push_back(b);
        b.name="30 secs";
        b.index=1;
        btn.push_back(b);
        b.name="1 min";
        b.index=2;
        btn.push_back(b);
        b.name="10 mins";
        b.index=3;
        btn.push_back(b);
        b.name="30 mins";
        b.index=4;
        btn.push_back(b);
        b.name="1 hour";
        b.index=5;
        btn.push_back(b);
        b.name="2 hours";
        b.index=6;
        btn.push_back(b);
    }
    common->select_menu.make_btn(btn);
    common->select_menu.exec();
    int i = common->select_menu.get_btn();
    uint64_t max_x =ui->chart->get_view_range_max_x();
    if(i==0)
    {
        ui->chart->set_view_range_max_x(max_x);
        ui->chart->set_view_range_min_x(max_x-10*1000);
    }
    else if(i==1)
    {
        ui->chart->set_view_range_max_x(max_x);
        ui->chart->set_view_range_min_x(max_x-30*1000);

    }
    else if(i==2)
    {
        ui->chart->set_view_range_max_x(max_x);
        ui->chart->set_view_range_min_x(max_x-1*60*1000);

    }
    else if(i==3)
    {
        ui->chart->set_view_range_max_x(max_x);
        ui->chart->set_view_range_min_x(max_x-10*60*1000);

    }
    else if(i==4)
    {
        ui->chart->set_view_range_max_x(max_x);
        ui->chart->set_view_range_min_x(max_x-30*60*1000);

    }
    else if(i==5)
    {
        ui->chart->set_view_range_max_x(max_x);
        ui->chart->set_view_range_min_x(max_x-1*60*60*1000);

    }
    else if(i==6)
    {
        ui->chart->set_view_range_max_x(max_x);
        ui->chart->set_view_range_min_x(max_x-2*60*60*1000);

    }
    ui->chart->update();
    //ui->chart->repaint();
}
void Tab_Observations_historyPage_Widget::on_MenuButton_clicked()
{
    Common* common = Common::instance();
    std::vector<btn> btn;
//    for(auto it=common->observation_main_page->legends.begin();it!=common->observation_main_page->legends.end();++it)
    qDebug()<<"mdc_code===="<<QString::fromStdString(common->history_datasource);

    if(common->history_datasource=="Observation")
    {
        for(int i=0;i<common->observation_main_page->legends.size();++i)
        {
            class btn b;
            b.name=common->observation_main_page->legends[i]->get_series_text();
            b.index=i;
            if(common->observation_main_page->legends[i]->get_mdccode()==common->history_mdccode)
                b.is_select=1;
            else
                b.is_select=0;
            btn.push_back(b);
        }
        common->select_menu.make_btn(btn);
        common->select_menu.exec();
        int i = common->select_menu.get_btn();
        auto it=common->observation_main_page->legends[i];
        set_title_text(it->get_mdccode(),
                       it->get_model(),
                       it->get_name(),
                       it->get_y_min(),
                       it->get_y_max(),
                       it->get_unit(),
                       it->get_datasource());
        ui->chart->clear_points(0);
    }
    else
    {
        for(int i=0;i<common->observation_main_page->m_wavepanel->m_nurse_items.size();++i)
        {
            class btn b;
            b.name=common->observation_main_page->m_wavepanel->m_nurse_items[i].display_desc;
            if (b.name == LOOPS_NAME)
                continue;
            b.index=i;
            if(common->observation_main_page->m_wavepanel->m_nurse_items[i].mdc_code
                    == common->history_mdccode)
                b.is_select=1;
            else
                b.is_select=0;
            btn.push_back(b);
        }
        common->select_menu.make_btn(btn);
        common->select_menu.exec();
        int i = common->select_menu.get_btn();
        auto it=common->observation_main_page->m_wavepanel->m_nurse_items[i];
        set_title_text(it.mdc_code,
                       it.model,
                       it.display_desc,
                       QString::number(it.y_min).toStdString(),
                       QString::number(it.y_max).toStdString(),
                       "",
                       "RTObservation");
        ui->chart->clear_points(0);
    }
}


void Tab_Observations_historyPage_Widget::on_apply_btn_clicked()
{
    ui->apply_btn->setEnabled(false);
    QTimer::singleShot(500, this,  [=](){
        ui->apply_btn->setEnabled(true);
});
    check_and_modify_jumper_time();
    uint64_t viewable = ui->chart->get_view_range_max_x() - ui->chart->get_view_range_min_x();
    uint64_t target_time = get_jumper_timeval()*1000;
    uint64_t now = (time(NULL)-1)*1000;
    time_t target_sec = get_jumper_timeval();
    time_t now_sec = time(NULL);
    time_t past_sec = now_sec - 48*60*60;
    struct tm target_sec_tm;
    struct tm now_sec_tm;
    struct tm past_sec_tm;
    localtime_r(&target_sec, &target_sec_tm);
    localtime_r(&now_sec, &now_sec_tm);
    localtime_r(&past_sec, &past_sec_tm);
    if(target_time + viewable/2 >= now ||
       (target_sec_tm.tm_year == now_sec_tm.tm_year &&
        target_sec_tm.tm_mon == now_sec_tm.tm_mon &&
        target_sec_tm.tm_mday == now_sec_tm.tm_mday &&
        target_sec_tm.tm_hour == now_sec_tm.tm_hour &&
        target_sec_tm.tm_min == now_sec_tm.tm_min))
    {
        right_locked = 1;
        update_triggered();
        return;
    }
    right_locked = 0;
    if(target_sec_tm.tm_year == past_sec_tm.tm_year &&
       target_sec_tm.tm_mon == past_sec_tm.tm_mon &&
       target_sec_tm.tm_mday == past_sec_tm.tm_mday &&
       target_sec_tm.tm_hour == past_sec_tm.tm_hour &&
       target_sec_tm.tm_min == past_sec_tm.tm_min)
        ui->chart->set_view_range_max_x(target_time + viewable);
    else
        ui->chart->set_view_range_max_x(target_time + viewable/2);
    ui->chart->set_view_range_min_x(ui->chart->get_view_range_max_x() - viewable);
    last_left_bound = ui->chart->get_view_range_min_x();

}

static uint8_t mc_is_too_far_past(time_t timeval)
{
    time_t now = time(NULL);
    time_t past = now - 48*60*60;
    struct tm timevaltm;
    struct tm pasttm;
    localtime_r(&timeval, &timevaltm);
    localtime_r(&past, &pasttm);
    if(timevaltm.tm_year < pasttm.tm_year)
        return 1;
    if(timevaltm.tm_year > pasttm.tm_year)
        return 0;
    if(timevaltm.tm_mon < pasttm.tm_mon)
        return 1;
    if(timevaltm.tm_mon > pasttm.tm_mon)
        return 0;
    if(timevaltm.tm_mday < pasttm.tm_mday)
        return 1;
    if(timevaltm.tm_mday > pasttm.tm_mday)
        return 0;
    if(timevaltm.tm_hour < pasttm.tm_hour)
        return 1;
    if(timevaltm.tm_hour > pasttm.tm_hour)
        return 0;
    if(timevaltm.tm_min < pasttm.tm_min)
        return 1;
    return 0;
}

void Tab_Observations_historyPage_Widget::step()
{
    Common* common = Common::instance();
    uint32_t current_time = Common::get_time_ms();
    switch(jumper_state)
    {
    case MC_JUMPER_STATE_IDLE:
        return;
    case MC_JUMPER_STATE_HOUR_UP_SPD0:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_state_duration0)
        {
            jumper_state = MC_JUMPER_STATE_HOUR_UP_SPD1;
            jumper_last_trigger_time = current_time;
            jumper_state_switch_time = current_time;
            trigger_hour_up();
        }
        break;
    case MC_JUMPER_STATE_HOUR_UP_SPD1:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_slow)
        {
            jumper_last_trigger_time = current_time;
            trigger_hour_up();
        }
        if(Common::get_elapsed_time(current_time, jumper_state_switch_time) >= common->button_hold_state_duration1)
        {
            jumper_state = MC_JUMPER_STATE_HOUR_UP_SPD2;
            jumper_state_switch_time = current_time;
        }
        break;
    case MC_JUMPER_STATE_HOUR_UP_SPD2:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_fast)
        {
            jumper_last_trigger_time = current_time;
            trigger_hour_up();
        }
        break;
    case MC_JUMPER_STATE_HOUR_DOWN_SPD0:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_state_duration0)
        {
            jumper_state = MC_JUMPER_STATE_HOUR_DOWN_SPD1;
            jumper_last_trigger_time = current_time;
            jumper_state_switch_time = current_time;
            trigger_hour_down();
        }
        break;
    case MC_JUMPER_STATE_HOUR_DOWN_SPD1:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_slow)
        {
            jumper_last_trigger_time = current_time;
            trigger_hour_down();
        }
        if(Common::get_elapsed_time(current_time, jumper_state_switch_time) >= common->button_hold_state_duration1)
        {
            jumper_state = MC_JUMPER_STATE_HOUR_DOWN_SPD2;
            jumper_state_switch_time = current_time;
        }
        break;
    case MC_JUMPER_STATE_HOUR_DOWN_SPD2:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_fast)
        {
            jumper_last_trigger_time = current_time;
            trigger_hour_down();
        }
        break;
    case MC_JUMPER_STATE_MIN_UP_SPD0:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_state_duration0)
        {
            jumper_state = MC_JUMPER_STATE_MIN_UP_SPD1;
            jumper_last_trigger_time = current_time;
            jumper_state_switch_time = current_time;
            trigger_min_up();
        }
        break;
    case MC_JUMPER_STATE_MIN_UP_SPD1:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_slow)
        {
            jumper_last_trigger_time = current_time;
            trigger_min_up();
        }
        if(Common::get_elapsed_time(current_time, jumper_state_switch_time) >= common->button_hold_state_duration1)
        {
            jumper_state = MC_JUMPER_STATE_MIN_UP_SPD2;
            jumper_state_switch_time = current_time;
        }
        break;
    case MC_JUMPER_STATE_MIN_UP_SPD2:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_fast)
        {
            jumper_last_trigger_time = current_time;
            trigger_min_up();
        }
        break;
    case MC_JUMPER_STATE_MIN_DOWN_SPD0:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_state_duration0)
        {
            jumper_state = MC_JUMPER_STATE_MIN_DOWN_SPD1;
            jumper_last_trigger_time = current_time;
            jumper_state_switch_time = current_time;
            trigger_min_down();
        }
        break;
    case MC_JUMPER_STATE_MIN_DOWN_SPD1:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_slow)
        {
            jumper_last_trigger_time = current_time;
            trigger_min_down();
        }
        if(Common::get_elapsed_time(current_time, jumper_state_switch_time) >= common->button_hold_state_duration1)
        {
            jumper_state = MC_JUMPER_STATE_MIN_DOWN_SPD2;
            jumper_state_switch_time = current_time;
        }
        break;
    case MC_JUMPER_STATE_MIN_DOWN_SPD2:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_fast)
        {
            jumper_last_trigger_time = current_time;
            trigger_min_down();
        }
        break;
    default:
        break;
    }
}

void Tab_Observations_historyPage_Widget::on_hour_up_pressed()
{
    trigger_hour_up();
    jumper_last_trigger_time = Common::get_time_ms();
    jumper_state_switch_time = jumper_last_trigger_time;
    jumper_state = MC_JUMPER_STATE_HOUR_UP_SPD0;
}

void Tab_Observations_historyPage_Widget::on_hour_up_released()
{
    jumper_state = MC_JUMPER_STATE_IDLE;
}

void Tab_Observations_historyPage_Widget::on_hour_down_pressed()
{
    trigger_hour_down();
    jumper_last_trigger_time = Common::get_time_ms();
    jumper_state_switch_time = jumper_last_trigger_time;
    jumper_state = MC_JUMPER_STATE_HOUR_DOWN_SPD0;
}

void Tab_Observations_historyPage_Widget::on_hour_down_released()
{
    jumper_state = MC_JUMPER_STATE_IDLE;
}

void Tab_Observations_historyPage_Widget::on_min_up_pressed()
{
    trigger_min_up();
    jumper_last_trigger_time = Common::get_time_ms();
    jumper_state_switch_time = jumper_last_trigger_time;
    jumper_state = MC_JUMPER_STATE_MIN_UP_SPD0;
}

void Tab_Observations_historyPage_Widget::on_min_up_released()
{
    jumper_state = MC_JUMPER_STATE_IDLE;
}

void Tab_Observations_historyPage_Widget::on_min_down_pressed()
{
    trigger_min_down();
    jumper_last_trigger_time = Common::get_time_ms();
    jumper_state_switch_time = jumper_last_trigger_time;
    jumper_state = MC_JUMPER_STATE_MIN_DOWN_SPD0;
}

void Tab_Observations_historyPage_Widget::on_min_down_released()
{
    jumper_state = MC_JUMPER_STATE_IDLE;
}

void Tab_Observations_historyPage_Widget::trigger_hour_up()
{
    jumper_hour++;
    if(jumper_hour > 23)
        jumper_hour = 0;
    check_and_modify_jumper_time();
}

void Tab_Observations_historyPage_Widget::trigger_hour_down()
{
    int orig = jumper_hour;
    jumper_hour--;
    if(jumper_hour < 0)
        jumper_hour = 23;
    time_t timeval = get_jumper_timeval();
    if(mc_is_too_far_past(timeval))
        jumper_hour = orig;
    else
        check_and_modify_jumper_time();
}

void Tab_Observations_historyPage_Widget::trigger_min_up()
{
    jumper_min++;
    if(jumper_min > 59)
        jumper_min = 0;
    check_and_modify_jumper_time();
}

void Tab_Observations_historyPage_Widget::trigger_min_down()
{
    int orig = jumper_min;
    jumper_min--;
    if(jumper_min < 0)
        jumper_min = 59;
    time_t timeval = get_jumper_timeval();
    if(mc_is_too_far_past(timeval))
        jumper_min = orig;
    else
        check_and_modify_jumper_time();
}

void Tab_Observations_historyPage_Widget::check_and_modify_jumper_time()
{
    time_t timeval = get_jumper_timeval();
    time_t now = time(NULL);
    time_t past = now - 48*60*60;
    uint8_t unchanged = 0;
    if(timeval > now)
        timeval = now;
    else if(timeval < past)
        timeval = past;
    else
        unchanged = 1;
    if(!unchanged)
    {
        struct tm timevaltm;
        localtime_r(&timeval, &timevaltm);
        jumper_hour = timevaltm.tm_hour;
        jumper_min = timevaltm.tm_min;
        update_jumper_date();
        char buf[16];
        sprintf(buf, "%d/%d", timevaltm.tm_mon+1, timevaltm.tm_mday);
        for(int i=0;i<ui->day_dropdown->count();i++)
        {
            if(ui->day_dropdown->itemText(i).compare(buf) == 0)
            {
                ui->day_dropdown->setCurrentIndex(i);
                break;
            }
        }
    }
    render_jumper_min();
    render_jumper_hour();
}

void Tab_Observations_historyPage_Widget::reset_time_jumper()
{
    ui->day_dropdown->setCurrentIndex(0);
    jumper_hour = 0;
    jumper_min = 0;
    render_jumper_min();
    render_jumper_hour();
}

void Tab_Observations_historyPage_Widget::render_jumper_min()
{
    char buf[32];
    sprintf(buf, "%02d", jumper_min);
    ui->min_label->setText(buf);
}

void Tab_Observations_historyPage_Widget::render_jumper_hour()
{
    char buf[32];
    sprintf(buf, "%02d", jumper_hour);
    ui->hour_label->setText(buf);
}

time_t Tab_Observations_historyPage_Widget::get_jumper_timeval()
{
    time_t timeval = ui->day_dropdown->currentData().toUInt();
    struct tm timevaltm;
    localtime_r(&timeval, &timevaltm);
    timevaltm.tm_hour = jumper_hour;
    timevaltm.tm_min = jumper_min;
    timevaltm.tm_sec = 0;
    return mktime(&timevaltm);
}

void Tab_Observations_historyPage_Widget::update_jumper_date()
{
    time_t now = time(NULL);
    struct tm nowtm;
    localtime_r(&now, &nowtm);
    time_t past24 = now - 24*60*60;
    struct tm past24tm;
    localtime_r(&past24, &past24tm);
    time_t past48 = now - 48*60*60;
    struct tm past48tm;
    localtime_r(&past48, &past48tm);
    char nowbuf[16];
    char past24buf[16];
    char past48buf[16];
    sprintf(nowbuf, "  %d/%d", nowtm.tm_mon+1, nowtm.tm_mday);
    sprintf(past24buf,"  %d/%d", past24tm.tm_mon+1, past24tm.tm_mday);
    sprintf(past48buf, "  %d/%d", past48tm.tm_mon+1, past48tm.tm_mday);
    if(ui->day_dropdown->count() == 3)
    {
        if(ui->day_dropdown->itemText(0).compare(nowbuf) == 0 &&
           ui->day_dropdown->itemText(1).compare(past24buf) == 0 &&
           ui->day_dropdown->itemText(2).compare(past48buf) == 0)
            return;
    }
    ui->day_dropdown->clear();
    ui->day_dropdown->addItem(nowbuf, QVariant((uint)now));
    ui->day_dropdown->addItem(past24buf, QVariant((uint)past24));
    ui->day_dropdown->addItem(past48buf, QVariant((uint)past48));

}

void Tab_Observations_historyPage_Widget::on_day_dropdown_currentIndexChanged(int index)
{
    check_and_modify_jumper_time();
}
void Tab_Observations_historyPage_Widget::set_title_text(std::string mdccode,
                                                         std::string model,
                                                         std::string name,
                                                         std::string y_min,
                                                         std::string y_max,
                                                         std::string unit,
                                                         std::string datasource)
{
    Common* common = Common::instance();
    common->history_mdccode    = mdccode;
    common->history_model      = model;
    common->history_name       = name;
    ui->chart->set_view_range_min_y(QString::fromStdString(y_min).toInt());
    ui->chart->set_view_range_max_y(QString::fromStdString(y_max).toInt());
    common->history_unit       = unit;
    common->history_datasource = datasource;


    QString qstr = QString::fromStdString(model)
            +"("+QString::fromStdString(name);
    if(common->history_unit.size()>0)
            qstr += "("+QString::fromStdString(unit)+")";
    if(datasource=="RTObservation")
        qstr += ",Wave";
    qstr += ")";
    uint64_t now = time(NULL);
    now*=1000;

    if(datasource=="RTObservation")
    {
        ui->chart->set_line_break_delta(common->RTO_line_break_delta);
        ui->chart->set_view_range_max_x(now);
        ui->chart->set_view_range_min_x(now-1*60*1000);
        set_history_time(3);
    }
    else
    {
        ui->chart->set_line_break_delta(common->Obs_line_break_delta);
        ui->chart->set_view_range_max_x(now);
        ui->chart->set_view_range_min_x(now-30*60*1000);
        set_history_time(125);
    }
    ui->label->setText(qstr);

    ui->label->update();
}


void Tab_Observations_historyPage_Widget::on_Fold_btn_clicked()
{
    if(is_fold)
    {
        ui->Fold_btn->setStyleSheet("QPushButton {color: rgb(255, 255, 255);font-style:\"Arial\";background-color: rgb(8, 98, 202);border-style: solid;border-width:1px;border-radius:20px;border-color:  rgb(11, 42, 78);border:2px groove gray;padding:10px 26px;border: none;}QPushButton:checked{background-color: rgb(11, 42, 78);};");
        is_fold=0;
    }
    else
    {
        ui->Fold_btn->setStyleSheet("QPushButton {color: rgb(255, 255, 255);font-style:\"Arial\";background-color: rgb(11, 42, 78);border-style: solid;border-width:1px;border-radius:20px;border-color:  rgb(11, 42, 78);border:2px groove gray;padding:10px 26px;border: none;}QPushButton:checked{background-color: rgb(11, 42, 78);};");
        is_fold=1;
    }
}
void Tab_Observations_historyPage_Widget::mapping_UI_reset()
{
    clear_selection();
    reset_time_jumper();
    ui->Fold_btn->setStyleSheet("QPushButton {color: rgb(255, 255, 255);font-style:\"Arial\";background-color: rgb(11, 42, 78);border-style: solid;border-width:1px;border-radius:20px;border-color:  rgb(11, 42, 78);border:2px groove gray;padding:10px 26px;border: none;}QPushButton:checked{background-color: rgb(11, 42, 78);};");
    is_fold=1;
}
