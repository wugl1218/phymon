#include "mc_loop.h"
#include "Common.h"
#include "qpainter.h"

#define Y_LABEL_WIDTH 50
#define X_LABEL_WIDTH 30

mc_loop::mc_loop(QWidget *parent)
    : QWidget{parent},
      timer(this)
{
    //connect(&timer, SIGNAL(timeout()), this, SLOT(worker()));
    //timer.start(16);
    view_range_min_x = 0.0;
    view_range_max_x = 0.0;
    view_range_min_y = 0.0;
    view_range_max_y = 0.0;
    margin_x = 10;
    margin_y = 30;
    animating = 0;
}

void mc_loop::add_point(double x, double y)
{
    QPointF p(x, y);
    points.push_back(p);
    update();
}

void mc_loop::clear_points()
{
    points.clear();
    animating = 0;
    update();
}

void mc_loop::save_points_as_ref()
{
    ref_points = points;
    update();
}

void mc_loop::clear_ref_points()
{
    ref_points.clear();
    update();
}

int mc_loop::get_num_points()
{
    return points.size();
}

int mc_loop::get_num_ref_points()
{
    return ref_points.size();
}

void mc_loop::set_view_range_min_x(double x)
{
    view_range_min_x = x;
    update();
}

void mc_loop::set_view_range_max_x(double x)
{
    view_range_max_x = x;
    update();
}

double mc_loop::get_view_range_min_x()
{
    return view_range_min_x;
}

double mc_loop::get_view_range_max_x()
{
    return view_range_max_x;
}

void mc_loop::set_view_range_min_y(double val)
{
    view_range_min_y = val;
    update();
}

void mc_loop::set_view_range_max_y(double val)
{
    view_range_max_y = val;
    update();
}

double mc_loop::get_view_range_min_y()
{
    return view_range_min_y;
}

double mc_loop::get_view_range_max_y()
{
    return view_range_max_y;
}

void mc_loop::set_margin_x(int pixels)
{
    margin_x = pixels;
    update();
}

void mc_loop::set_margin_y(int pixels)
{
    margin_y = pixels;
    update();
}

void mc_loop::animation_start(uint32_t duration_ms)
{
    animating = 1;
    start_time = Common::get_time_ms();
    end_time = start_time + duration_ms;
}

void mc_loop::paintEvent(QPaintEvent *event)
{
    if(view_range_max_x - view_range_min_x == 0.0 || view_range_max_y - view_range_min_y == 0.0)
        return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen grid_pen(QColor(0xbc,0xcf,0xe2), 1, Qt::DashLine, Qt::RoundCap,  Qt::RoundJoin);
    QPen label_pen(QColor(0xbc,0xcf,0xe2), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QBrush no_brush(Qt::NoBrush);
    painter.setBrush(no_brush);
    double total_x_padding = Y_LABEL_WIDTH;
    double total_y_padding = X_LABEL_WIDTH;
    painter.setPen(label_pen);
    scratch.clear();
    scratch.push_back(QPointF(margin_x+total_x_padding, margin_y));
    scratch.push_back(QPointF(margin_x+total_x_padding, height()-margin_y-total_y_padding));
    scratch.push_back(QPointF(width()-margin_x-total_x_padding, height()-margin_y-total_y_padding));
    scratch.push_back(QPointF(width()-margin_x-total_x_padding, margin_y));
    scratch.push_back(QPointF(margin_x+total_x_padding, margin_y));
    painter.drawPolyline(&scratch[0], scratch.size());
    double area_height = height()-((double)margin_y*2)-total_y_padding;
    double area_width = width()-((double)margin_x*2)-total_x_padding*2;
    if(0.0 > view_range_min_y && 0.0 < view_range_max_y)
    {
        painter.setPen(grid_pen);
        double k = height()-total_y_padding-margin_y-((0.0 - view_range_min_y)/(view_range_max_y - view_range_min_y))*area_height;
        QPointF p1(margin_x+total_x_padding, k);
        QPointF p2(width()-margin_x-total_x_padding, k);
        painter.drawLine(p1, p2);
    }
    if(0.0 > view_range_min_x && 0.0 < view_range_max_x)
    {
        painter.setPen(grid_pen);
        double k = margin_x+total_x_padding+((0.0-view_range_min_x)/(view_range_max_x - view_range_min_x))*area_width;
        QPointF p1(k, height()-total_y_padding-margin_y);
        QPointF p2(k, margin_y);
        painter.drawLine(p1, p2);
    }
    painter.setPen(label_pen);
    char buf[64];
    sprintf(buf, "%d", (int)view_range_min_y);
    Common::draw_text(painter, margin_x + total_x_padding - 8 , margin_y + area_height, Qt::AlignVCenter | Qt::AlignRight, buf);
    sprintf(buf, "%d", (int)view_range_max_y);
    Common::draw_text(painter, margin_x + total_x_padding - 8 , margin_y, Qt::AlignVCenter | Qt::AlignRight, buf);
    sprintf(buf, "%d", (int)view_range_min_x);
    Common::draw_text(painter, margin_x + total_x_padding, height()-margin_y-total_y_padding+10, Qt::AlignTop | Qt::AlignHCenter, buf);
    sprintf(buf, "%d", (int)view_range_max_x);
    Common::draw_text(painter, width()-total_x_padding-margin_x, height()-margin_y-total_y_padding+10, Qt::AlignTop | Qt::AlignHCenter, buf);
    if(y_label.size() > 0)
        Common::draw_text(painter, margin_x, margin_y/2.0, Qt::AlignVCenter | Qt::AlignLeft, y_label.c_str());
    if(x_label.size() > 0)
        Common::draw_text(painter, width()/2.0, height()-margin_y-total_y_padding+10, Qt::AlignTop | Qt::AlignHCenter, x_label.c_str());
    if(ref_points.size() > 1)
    {
        QPen graph_pen(QColor(100,100,100));
        graph_pen.setWidth(1);
        painter.setPen(graph_pen);
        scratch.clear();
        for(int i=0;i<(int)ref_points.size();i++)
        {
            scratch.push_back(QPointF(((ref_points[i].x()-view_range_min_x)/(view_range_max_x - view_range_min_x))*area_width +
                                      margin_x+total_x_padding, height() - margin_y - total_y_padding - ((ref_points[i].y() -
                                      view_range_min_y)/(view_range_max_y - view_range_min_y))*area_height));
        }
        painter.drawPolyline(&scratch[0], scratch.size());
    }
    if(points.size() > 1)
    {
        QPen graph_pen(QColor(0,255,0));
        graph_pen.setWidth(1);
        painter.setPen(graph_pen);
        scratch.clear();
        if(!animating)
        {
            draw_all:
            for(int i=0;i<(int)points.size();i++)
            {
                scratch.push_back(QPointF(((points[i].x()-view_range_min_x)/(view_range_max_x - view_range_min_x))*area_width +
                                          margin_x+total_x_padding, height() - margin_y - total_y_padding - ((points[i].y() -
                                          view_range_min_y)/(view_range_max_y - view_range_min_y))*area_height));
            }
            painter.drawPolyline(&scratch[0], scratch.size());
        }
        else
        {
            uint32_t current_time = Common::get_time_ms();
            uint32_t elapsed_time = Common::get_elapsed_time(current_time, start_time);
            uint32_t anim_duration = Common::get_elapsed_time(end_time, start_time);
            if(elapsed_time >= anim_duration)
            {
                animating = 0;
                emit animation_finished();
                goto draw_all;
            }
            double progress = (double)elapsed_time/(double)anim_duration;
            int num_pts = points.size()*progress;
            for(int i=0;i<num_pts;i++)
            {
                scratch.push_back(QPointF(((points[i].x()-view_range_min_x)/(view_range_max_x - view_range_min_x))*area_width +
                                          margin_x+total_x_padding, height() - margin_y - total_y_padding - ((points[i].y() -
                                          view_range_min_y)/(view_range_max_y - view_range_min_y))*area_height));
            }
            painter.drawPolyline(&scratch[0], scratch.size());
        }
    }
}

void mc_loop::mousePressEvent(QMouseEvent *event)
{
    emit on_press();
}

void mc_loop::worker()
{
    if(animating)
        update();
}

void mc_loop::set_x_label(const char* str)
{
    x_label = str;
    update();
}

void mc_loop::set_y_label(const char* str)
{
    y_label = str;
    update();
}
