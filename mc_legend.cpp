#include "mc_legend.h"
#include "qevent.h"
#include "qpainter.h"
#include "Common.h"

mc_legend::mc_legend(QWidget *parent)
    :QWidget(parent)
{
    left_margin = 0;
    top_margin = 0;
    square_width = 220;
    square_height =70;
    vertical_spacing = 40;
    font_size = 16;
}

void mc_legend::set_left_margin(int margin)
{
    left_margin = margin;
    update();
}

int mc_legend::get_left_margin()
{
    return left_margin;
}

void mc_legend::set_top_margin(int margin)
{
    top_margin = margin;
    update();
}

int mc_legend::get_top_margin()
{
    return top_margin;
}

void mc_legend::set_vertical_spacing(int spacing)
{
    vertical_spacing = spacing;
    update();
}

int mc_legend::get_vertical_spacing()
{
    return vertical_spacing;
}

void mc_legend::set_square_width(int width)
{
    square_width = width;
    update();
}

int mc_legend::get_square_width()
{
    return square_width;
}

void mc_legend::set_font_size(int size)
{
    font_size = size;
    update();
}

int mc_legend::get_font_size()
{
    return font_size;
}

static void mc_legend_init(mc_legend_entry* e)
{
    e->color.setRgb(255,255,255,255);
}

void mc_legend::set_series_color(int series_index, QColor color)
{
    if(series_index >= (int)entries.size())
    {
        mc_legend_entry e;
        mc_legend_init(&e);
        entries.resize(series_index+1, e);
    }
    entries[series_index].color = color;
    update();
}

QColor mc_legend::get_series_color(int series_index)
{
    if(series_index >= (int)entries.size())
    {
        mc_legend_entry e;
        mc_legend_init(&e);
        return e.color;
    }
    return entries[series_index].color;
}

void mc_legend::set_series_text(int series_index, std::string text ,std::string model,std::string mdc_code)
{
    if(series_index >= (int)entries.size())
    {
        mc_legend_entry e;
        mc_legend_init(&e);
        entries.resize(series_index+1, e);
    }
    entries[series_index].name = text;
    entries[series_index].model = model;
    entries[series_index].mdc_code = mdc_code;

    update();
}

std::string mc_legend::get_series_text(int series_index)
{
    if(series_index >= (int)entries.size())
        return "";
    return entries[series_index].name;
}

void mc_legend::remove_series(int series_index)
{
    int i=0;
    for(auto it=entries.begin();it!=entries.end();it++)
    {
        if(i==series_index)
        {
            entries.erase(it);
            break;
        }
        i++;
    }
}

void mc_legend::set_text_color(QColor color)
{
    text_color = color;
    update();
}

QColor mc_legend::get_text_color()
{
    return text_color;
}

void mc_legend::paintEvent(QPaintEvent *event)
{
    Common* common = Common::instance();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen nopen(Qt::NoPen);
    QPen pen(text_color);
    painter.setPen(nopen);
    for(int i=0;i<(int)entries.size();i++)
    {
        QBrush brush(entries[i].color);
        painter.setBrush(brush);
        painter.setPen(nopen);
        painter.drawRoundedRect(0,top_margin + vertical_spacing*i,square_width,square_height,10,10);
        if(entries[i].name.size() > 0)
        {
            std::string name =entries[i].name;
            name.append(" (");
            name.append(entries[i].model);
            name.append(")");

            painter.setPen(pen);
            Common::draw_text(painter, left_margin +  10,
                              top_margin + vertical_spacing*i + 20,
                              Qt::AlignLeft | Qt::AlignVCenter, name.c_str());
            std::string querystr = "vmd_id MATCH '";
            querystr.append(common->vmd_id);
            querystr.append("' AND patient_id MATCH '");
            querystr.append(common->patient_id);
            querystr.append("' AND model MATCH '");
            querystr.append(entries[i].model);
            querystr.append("' AND mdc_code MATCH '");
            querystr.append(entries[i].mdc_code);
            querystr.append("'");
            dds::sub::cond::QueryCondition qcond(
                        dds::sub::Query(common->observation_reader_2, querystr),
                        dds::sub::status::DataState(
                        dds::sub::status::SampleState::any(),
                        dds::sub::status::ViewState::any(),
                        dds::sub::status::InstanceState::alive()));
            dds::sub::LoanedSamples<dds::core::xtypes::DynamicData> samples = common->observation_reader_2.select().condition(qcond).read();
            for(auto& sample : samples)
            {
                if(sample.info().valid())
                {
                    dds::core::xtypes::DynamicData& data = const_cast<dds::core::xtypes::DynamicData&>(sample.data());
                    //data.get_values("values", vals);
                    float val = data.value<float>("value");
//                    std::vector<float> vals;
//                    vals.push_back(val);
                    std::string unit =data.value<std::string>("unit");
                    int length1= unit.length()*10;
                    Common::draw_text(painter, left_margin +  10,
                                      top_margin + vertical_spacing*i + 50,
                                      Qt::AlignLeft | Qt::AlignVCenter, QString::number(val));
                    Common::draw_text(painter, left_margin +  135,
                                      top_margin + vertical_spacing*i + 50,
                                      Qt::AlignLeft | Qt::AlignVCenter, QString::fromStdString(unit));
                }
            }
        }
    }
}

void mc_legend::mouseMoveEvent(QMouseEvent *event)
{

}

void mc_legend::mousePressEvent(QMouseEvent *event)
{
    int i = (event->pos().y() - top_margin + ((vertical_spacing-square_width)/2.0))/vertical_spacing;
    if(i < (int)entries.size())
        emit on_series_select(i);
}

void mc_legend::mouseReleaseEvent(QMouseEvent *event)
{

}
