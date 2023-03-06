#include "mc_chart.h"
#include "qevent.h"
#include "qpainter.h"
#include <math.h>
#include <map>
#include "Common.h"

#define Y_LABEL_WIDTH 50
#define X_LABEL_WIDTH 30
#define FONT_SIZE 12
#define LEGEND_VERTICAL_SPACING 30

mc_chart::mc_chart(QWidget* parent)
    :QWidget(parent),
      timer(this)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    view_range_min_x = 0;
    view_range_max_x = 0;
    view_range_min_y = 0.0f;
    view_range_max_y = 0.0f;
    num_labels_x = 0;
    num_labels_y = 0;
    margin_x = 5.0f;
    margin_y = 10.0f;
    scrollable = 1;
    bounds_max_x = 0;
    bounds_min_x = UINT64_MAX;
    press_last_left = 0.0;
    press_last_right = 0.0;
    zoomable = 1;
    selection_type = MC_SELECT_NONE;
    axis_visible = 1;
    max_zoom = 0;
    min_zoom = 0;
    velocity = 0.0;
    last_velocity = 0.0;
    friction = DEFAULT_FRICTION;
    has_selection = 0;
    visible = 1;
    custom_left_bound = 0;
    custom_right_bound = 0;
    setAttribute(Qt::WA_AcceptTouchEvents);
    connect(&timer, SIGNAL(timeout()), this, SLOT(worker()));
    //timer.start(16);
}

void mc_chart::add_point(int series_index, uint64_t timestamp, float val)
{
    if(series_index >= (int)points.size())
    {
        points.resize(series_index+1);
        colors.resize(series_index+1);
        widths.resize(series_index+1);
        colors[series_index].setRgb(0, 255, 0);
        widths[series_index] = 2;
    }
    uint8_t right_locked = 0;
    uint8_t maybe_right_locked = 0;
    if(view_range_max_x == bounds_max_x)
        right_locked = 1;
    else if(view_range_max_x > bounds_max_x)
        maybe_right_locked = 1;
    points[series_index].emplace(timestamp, val);
    update();
    if(timestamp > bounds_max_x)
    {
        bounds_max_x = timestamp;
        if(right_locked || (maybe_right_locked && timestamp > view_range_max_x))
        {
            uint64_t view_range = view_range_max_x - view_range_min_x;
            view_range_max_x = bounds_max_x;
            view_range_min_x = view_range_max_x - view_range;
        }
    }
    if(timestamp < bounds_min_x)
        bounds_min_x = timestamp;
}

uint64_t mc_chart::get_left_bounds()
{
    return bounds_min_x;
}

uint64_t mc_chart::get_right_bounds()
{
    return bounds_max_x;
}

void mc_chart::remove_point(int series_index, int index)
{
    if(series_index >= (int)points.size())
        return;
    int i=0;
    auto it=points[series_index].begin();
    for(;it!=points[series_index].end();it++)
    {
        if(i == index)
            break;
        i++;
    }
    if(it == points[series_index].end())
        return;
    points[series_index].erase(it);
    calc_bounds();
    update();
}

void mc_chart::calc_bounds()
{
    bounds_max_x = 0;
    bounds_min_x = UINT64_MAX;
    for(int i=0;i<(int)points.size();i++)
    {
        if(points[i].size() > 0)
        {
            auto it1 = points[i].begin();
            auto it2 = points[i].rbegin();
            if(it1->first < bounds_min_x)
                bounds_min_x = it1->first;
            if(it2->first > bounds_max_x)
                bounds_max_x = it2->first;
        }
    }
}

void mc_chart::clear_points(int series_index)
{
    if(series_index >= (int)points.size())
        return;
    points[series_index].clear();
    calc_bounds();
    update();
}

int mc_chart::get_num_points(int series_index)
{
    if(series_index >= (int)points.size())
        return 0;
    return (int)points[series_index].size();
}

void mc_chart::set_series_color(int series_index, QColor color)
{
    if(series_index >= (int)points.size())
    {
        points.resize(series_index+1);
        colors.resize(series_index+1);
        widths.resize(series_index+1);
        colors[series_index].setRgb(0, 255, 0);
        widths[series_index] = 2;
    }
    colors[series_index] = color;
}

QColor mc_chart::get_series_color(int series_index)
{
    if(series_index >= (int)points.size())
        return QColor(255,255,255,255);
    return colors[series_index];
}

void mc_chart::set_series_width(int series_index, int width)
{
    if(series_index >= (int)points.size())
    {
        points.resize(series_index+1);
        colors.resize(series_index+1);
        widths.resize(series_index+1);
        colors[series_index].setRgb(0, 255, 0);
        widths[series_index] = 2;
    }
    widths[series_index] = width;
}

int mc_chart::get_series_width(int series_index)
{
    if(series_index >= (int)points.size())
        return 0;
    return widths[series_index];
}

void mc_chart::set_view_range_min_x(uint64_t timestamp)
{
    view_range_min_x = timestamp;
    update();
}

void mc_chart::set_view_range_max_x(uint64_t timestamp)
{
    view_range_max_x = timestamp;
    update();
}

uint64_t mc_chart::get_view_range_min_x()
{
    return view_range_min_x;
}

uint64_t mc_chart::get_view_range_max_x()
{
    return view_range_max_x;
}

void mc_chart::set_view_range_min_y(float val)
{
    view_range_min_y = val;
    update();
}

void mc_chart::set_view_range_max_y(float val)
{
    view_range_max_y = val;
    update();
}

float mc_chart::get_view_range_min_y()
{
    return view_range_min_y;
}

float mc_chart::get_view_range_max_y()
{
    return view_range_max_y;
}

void mc_chart::set_num_labels_x(int n)
{
    num_labels_x = n;
    update();
}

void mc_chart::set_num_labels_y(int n)
{
    num_labels_y = n;
    update();
}

int mc_chart::get_num_labels_x()
{
    return num_labels_x;
}

int mc_chart::get_num_labels_y()
{
    return num_labels_y;
}

void mc_chart::set_margin_x(float pixels)
{
    margin_x = pixels;
    update();
}

void mc_chart::set_margin_y(float pixels)
{
    margin_y = pixels;
    update();
}

void mc_chart::set_visible(uint8_t visible)
{
    this->visible = visible;
    update();
}

uint8_t mc_chart::is_visible()
{
    return visible;
}
void mc_chart::set_line_break_delta(int line_break_delta)
{
    this->line_break_delta = line_break_delta;
    update();
}
void mc_chart::paintEvent(QPaintEvent*)
{
    if(view_range_max_x == 0 || num_labels_x < 2 || num_labels_y < 2 || !visible)
        return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen grid_pen(QColor(0xbc,0xcf,0xe2), 1, Qt::DashLine, Qt::RoundCap,  Qt::RoundJoin);
    QPen label_pen(QColor(0xbc,0xcf,0xe2), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QBrush no_brush(Qt::NoBrush);
    painter.setBrush(no_brush);

    double total_x_padding = Y_LABEL_WIDTH;
    double total_y_padding = X_LABEL_WIDTH;
    if(!axis_visible)
    {
        total_x_padding = 0.0;
        total_y_padding = 0.0;
    }
    else
    {
        painter.setPen(label_pen);
        scratch.clear();
        scratch.push_back(QPointF(margin_x+total_x_padding, margin_y));
        scratch.push_back(QPointF(margin_x+total_x_padding, height()-margin_y-total_y_padding));
        scratch.push_back(QPointF(width()-margin_x-total_x_padding, height()-margin_y-total_y_padding));
        scratch.push_back(QPointF(width()-margin_x-total_x_padding, margin_y));
        scratch.push_back(QPointF(margin_x+total_x_padding, margin_y));
        painter.drawPolyline(&scratch[0], scratch.size());
    }

    double area_height = height()-((double)margin_y*2)-total_y_padding;
    double area_width = width()-((double)margin_x*2)-total_x_padding*2;
    double grid_ylen = area_height/(num_labels_y-1);
    double grid_xlen = area_width/(num_labels_x-1);
    painter.setPen(grid_pen);
    if(axis_visible)
    {
        for(int i=0;i<num_labels_y-2;i++)
        {
            QPointF p1(margin_x+total_x_padding, margin_y+(i+1)*grid_ylen);
            QPointF p2(width()-margin_x-total_x_padding, margin_y+(i+1)*grid_ylen);
            painter.drawLine(p1, p2);
        }
        for(int i=0;i<num_labels_x-2;i++)
        {
            QPointF p1(margin_x+total_x_padding+(i+1)*grid_xlen, margin_y);
            QPointF p2(margin_x+total_x_padding+(i+1)*grid_xlen, height()-margin_y-total_y_padding);
            painter.drawLine(p1, p2);
        }

        painter.setPen(label_pen);
        for(int i=0;i<num_labels_y;i++)
        {
            char buf[64];
            double val = i * (grid_ylen/area_height) * (view_range_max_y - view_range_min_y) + view_range_min_y;
            sprintf(buf, "%.01f", val);
            QString str = buf;
            Common::draw_text(painter, margin_x + total_x_padding - 8 , margin_y + area_height - grid_ylen*i, Qt::AlignVCenter | Qt::AlignRight, str);
        }
        for(int i=0;i<num_labels_x;i++)
        {
            char buf[64];
            uint64_t val = (uint64_t)(i * (grid_xlen/area_width) * (view_range_max_x - view_range_min_x) + view_range_min_x);
            time_t tval = val/1000;
            struct tm t;
            localtime_r(&tval, &t);
            sprintf(buf, "%d/%d %02d:%02d:%02d", t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
            QString str = buf;
            Common::draw_text(painter, margin_x + total_x_padding + grid_xlen*i, height()-margin_y-total_y_padding+10, Qt::AlignTop | Qt::AlignHCenter, str);
        }
    }
    for(int i=0;i<(int)points.size();i++)
    {
        if(points[i].size() < 2)
            continue;
        QPen graph_pen(colors[i]);
        graph_pen.setWidth(widths[i]);
        painter.setPen(graph_pen);
        scratch.clear();
        for(auto it=points[i].begin();it!=points[i].end();it++)
        {
            if(it->first < view_range_min_x)
            {
                auto next = it;
                next++;
                if(next != points[i].end() && next->first > view_range_min_x)
                {
                    uint64_t delta_time = next->first - it->first;
                    if(delta_time >= line_break_delta)
                        continue;
                    float delta_val = next->second - it->second;
                    float ratio = (double)(view_range_min_x - it->first)/delta_time;
                    delta_val = delta_val*ratio + it->second;
                    scratch.push_back(QPointF(margin_x+total_x_padding, height() - margin_y - total_y_padding - ((delta_val -
                                              view_range_min_y)/(view_range_max_y - view_range_min_y))*area_height));
                }
                continue;
            }
            else if(it->first > view_range_max_x)
            {
                if(it == points[i].begin())
                    continue;
                auto prev = it;
                prev--;
                if(prev->first < view_range_max_x)
                {
                    uint64_t delta_time = it->first - prev->first;
                    if(delta_time >= line_break_delta)
                        break;
                    float delta_val = it->second - prev->second;
                    float ratio = (double)(view_range_max_x - prev->first)/delta_time;
                    delta_val = delta_val*ratio + prev->second;
                    scratch.push_back(QPointF(width() - margin_x - total_x_padding, height() - margin_y - total_y_padding - ((delta_val -
                                              view_range_min_y)/(view_range_max_y - view_range_min_y))*area_height));
                }
                continue;
            }
            if(it != points[i].begin() && it != points[i].end())
            {
                auto prev = it;
                prev--;
                uint64_t delta_time = it->first - prev->first;
                if(delta_time >= line_break_delta)
                {
                    painter.drawPolyline(&scratch[0], scratch.size());
                    scratch.clear();
                }
            }
            scratch.push_back(QPointF((((double)it->first-view_range_min_x)/(view_range_max_x - view_range_min_x))*area_width +
                                      margin_x+total_x_padding, height() - margin_y - total_y_padding - ((it->second -
                                      view_range_min_y)/(view_range_max_y - view_range_min_y))*area_height));
        }
        painter.drawPolyline(&scratch[0], scratch.size());
    }
    if(has_selection && selection_x >= view_range_min_x && selection_x <= view_range_max_x)
    {
        QPen selection_pen(Qt::red);
        painter.setPen(selection_pen);
        painter.drawEllipse(QPoint(view_to_screenspace_x(selection_x), view_to_screenspace_y(selection_y)), 10, 10);
    }
    emit on_paint();
}

void mc_chart::set_scrollable(uint8_t scrollable)
{
    this->scrollable = scrollable;
}

uint8_t mc_chart::is_scrollable()
{
    return scrollable;
}

void mc_chart::set_selection_type(uint8_t type)
{
    selection_type = type;
}

uint8_t mc_chart::get_selection_type()
{
    return selection_type;
}

void mc_chart::set_selection_width(double width)
{
    selection_width = width;
}

double mc_chart::get_selection_width()
{
    return selection_width;
}

void mc_chart::clear_selection()
{
    has_selection = 0;
}

void mc_chart::set_zoomable(uint8_t zoomable)
{
    this->zoomable = zoomable;
}

uint8_t mc_chart::is_zoomable()
{
    return zoomable;
}

void mc_chart::set_axis_visible(uint8_t visible)
{
    axis_visible = visible;
    update();
}

uint8_t mc_chart::axis_is_visible()
{
    return axis_visible;
}

void mc_chart::set_max_zoom(uint64_t viewable_range)
{
    max_zoom = viewable_range;
}

uint64_t mc_chart::get_max_zoom()
{
    return max_zoom;
}

void mc_chart::set_min_zoom(uint64_t viewable_range)
{
    min_zoom = viewable_range;
}

uint64_t mc_chart::get_min_zoom()
{
    return min_zoom;
}

void mc_chart::worker()
{
    if(velocity == 0)
        return;
    double total_x_padding = Y_LABEL_WIDTH;
    if(!axis_visible)
        total_x_padding = 0.0;
    double area_width = width()-((double)margin_x*2)-total_x_padding*2;
    double view_area_width = view_range_max_x - view_range_min_x;
    double delta_view = (velocity/area_width)*view_area_width;
    uint64_t new_min_x = view_range_min_x - delta_view;
    uint64_t new_max_x = view_range_max_x - delta_view;
    if(bounds_max_x - bounds_min_x <= view_range_max_x - view_range_min_x)
    {
        /*
        if(new_min_x > bounds_min_x)
        {
            new_min_x = bounds_min_x;
            new_max_x = view_range_max_x + (new_min_x - view_range_min_x);
        }
        else if(new_max_x < bounds_max_x)
        {
            new_max_x = bounds_max_x;
            new_min_x = view_range_min_x + (new_max_x - view_range_max_x);
        }*/
        new_max_x = bounds_max_x;
        new_min_x = view_range_min_x + (new_max_x - view_range_max_x);
    }
    else
    {
        if(new_min_x < bounds_min_x)
        {
            new_min_x = bounds_min_x;
            new_max_x = view_range_max_x + (new_min_x - view_range_min_x);
        }
        else if(new_max_x > bounds_max_x)
        {
            new_max_x = bounds_max_x;
            new_min_x = view_range_min_x + (new_max_x - view_range_max_x);
        }
    }
    view_range_max_x = new_max_x;
    view_range_min_x = new_min_x;
    if(velocity > 0.0)
    {
        velocity -= friction;
        if(velocity < 0.0)
            velocity = 0.0;
    }
    else
    {
        velocity += friction;
        if(velocity > 0.0)
            velocity = 0.0;
    }
    update();
}

void mc_chart::set_friction(double friction)
{
    this->friction = friction;
}

double mc_chart::get_friction()
{
    return friction;
}

void mc_chart::handle_press(float x, float y)
{
    press_start_x = x;
    press_start_y = y;
    press_last_x = press_start_x;
    press_last_y = press_start_y;
    press_is_stationary = 1;
    press_last_left = 0.0;
    press_last_right = 0.0;
    last_velocity = 0.0;
    velocity = 0.0;
    promoted_to_multitouch = 0;
    emit on_press();
}

void mc_chart::handle_move(float x, float y)
{
    if(!scrollable)
        return;
    float current_pos_x = x;
    float current_pos_y = y;
    if(Common::distance(current_pos_x, current_pos_y, press_start_x, press_start_y) > STATIONARY_CHECK_DISTANCE)
        press_is_stationary = 0;
    double delta_x = current_pos_x - press_last_x;
    double area_width = width()-((double)margin_x*2)-Y_LABEL_WIDTH*2;
    double view_delta = ((double)view_range_max_x - view_range_min_x)*(delta_x/area_width);

    uint64_t new_min_x = view_range_min_x - (uint64_t)view_delta;
    uint64_t new_max_x = view_range_max_x - (uint64_t)view_delta;
    //if(bounds_max_x - bounds_min_x <= view_range_max_x - view_range_min_x)
    //{
        /*
        if(new_min_x > bounds_min_x)
        {
            new_min_x = bounds_min_x;
            new_max_x = view_range_max_x + (new_min_x - view_range_min_x);
        }
        else if(new_max_x < bounds_max_x)
        {
            new_max_x = bounds_max_x;
            new_min_x = view_range_min_x + (new_max_x - view_range_max_x);
        }*/
        //new_max_x = bounds_max_x;
        //new_min_x = view_range_min_x + (new_max_x - view_range_max_x);
    //}
    //else
    //{
        if(custom_left_bound && custom_right_bound)
        {
            if(new_min_x < custom_left_bound)
            {


                new_min_x = custom_left_bound;
                new_max_x = view_range_max_x + (new_min_x - view_range_min_x);
            }
            else if(new_max_x > custom_right_bound)
            {

                new_max_x = custom_right_bound;
                new_min_x = view_range_min_x + (new_max_x - view_range_max_x);
            }
        }
        else
        {
            if(new_min_x < bounds_min_x)
            {
                new_min_x = bounds_min_x;
                new_max_x = view_range_max_x + (new_min_x - view_range_min_x);
            }
            else if(new_max_x > bounds_max_x)
            {

                new_max_x = bounds_max_x;
                new_min_x = view_range_min_x + (new_max_x - view_range_max_x);
            }
        }
    //}
    view_range_max_x = new_max_x;
    view_range_min_x = new_min_x;
    last_velocity = current_pos_x - press_last_x;
    press_last_x = current_pos_x;
    press_last_y = current_pos_y;
    update();
}

void mc_chart::handle_release(float x, float y)
{
    float current_pos_x = x;
    float current_pos_y = y;
    if(Common::distance(current_pos_x, current_pos_y, press_start_x, press_start_y) > STATIONARY_CHECK_DISTANCE)
        press_is_stationary = 0;
    if(press_is_stationary)
    {
        handle_select(current_pos_x, current_pos_y);
        update();
    }
    else
        velocity = last_velocity*VELOCITY_MULTIPLIER;
}

void mc_chart::mouseMoveEvent(QMouseEvent *event)
{
    handle_move(event->pos().x(), event->pos().y());
}

void mc_chart::mousePressEvent(QMouseEvent *event)
{
    handle_press(event->pos().x(), event->pos().y());
    emit clicked();

}

void mc_chart::mouseReleaseEvent(QMouseEvent *event)
{
    handle_release(event->pos().x(), event->pos().y());

}

bool mc_chart::event(QEvent* event)
{
    switch(event->type())
    {
    case QEvent::TouchBegin:
    {
        QPointerEvent* e = (QPointerEvent*)event;
        auto it = e->points().begin();
        handle_press(it->position().x(), it->position().y());
        return true;
    }
    case QEvent::TouchUpdate:
    {
        QPointerEvent* e = (QPointerEvent*)event;
        if(e->pointCount() == 1)
        {
            if(promoted_to_multitouch)
                return false;
            auto it = e->points().begin();
            handle_move(it->position().x(), it->position().y());
        }
        else
        {
            if(!scrollable)
                return false;
            if(!zoomable)
                return false;
            if(!promoted_to_multitouch)
            {
                promoted_to_multitouch = 1;
                std::map<uint64_t, QPointF> pts;
                auto it = e->points().begin();
                for(;it!=e->points().end();it++)
                    pts.emplace(it->position().x(), it->position());
                press_last_left = pts.begin()->first;
                press_last_right = pts.rbegin()->first;
                return true;
            }
            std::map<uint64_t, QPointF> pts;
            auto it = e->points().begin();
            for(;it!=e->points().end();it++)
                pts.emplace(it->position().x(), it->position());
            double press_left = pts.begin()->first;
            double press_right = pts.rbegin()->first;
            double delta_left = press_left - press_last_left;
            double delta_right = press_right - press_last_right;
            double area_width = width()-((double)margin_x*2)-Y_LABEL_WIDTH*2;
            uint64_t new_min_x = view_range_min_x - (delta_left/area_width)*(view_range_max_x - view_range_min_x);
            uint64_t new_max_x = view_range_max_x - (delta_right/area_width)*(view_range_max_x - view_range_min_x);
            if(max_zoom != 0 && min_zoom != 0)
            {
                if(new_max_x - new_min_x < max_zoom)
                {
                    new_min_x = view_range_min_x;
                    new_max_x = new_min_x + max_zoom;
                }
                else if(new_max_x - new_min_x > min_zoom)
                {
                    new_min_x = view_range_min_x;
                    new_max_x = new_min_x + min_zoom;
                }
            }
            view_range_min_x = new_min_x;
            view_range_max_x = new_max_x;
            press_last_left = press_left;
            press_last_right = press_right;
            update();
        }
        return true;
    }
    case QEvent::TouchEnd:
    {
        QPointerEvent* e = (QPointerEvent*)event;
        if(e->pointCount() == 1)
        {
            if(promoted_to_multitouch)
                return false;
            auto it = e->points().begin();
            handle_release(it->position().x(), it->position().y());
        }
        return true;
    }
    default:
        return QWidget::event(event);
    }
}

void mc_chart::handle_select(double x, double y)
{
    if(selection_type == MC_SELECT_NONE)
        return;
    double total_y_padding = X_LABEL_WIDTH;
    double total_x_padding = Y_LABEL_WIDTH;
    if(!axis_visible)
    {
        total_y_padding = 0.0;
        total_x_padding = 0.0;
    }
    double area_height = height()-((double)margin_y*2)-total_y_padding;
    double area_width = width()-((double)margin_x*2)-total_x_padding*2;
    double view_area_width = view_range_max_x - view_range_min_x;
    double view_area_height = view_range_max_y - view_range_min_y;
    double selection_radius = selection_width/2.0;
    if(selection_type == MC_SELECT_SERIES)
    {
        for(int i=0;i<(int)points.size();i++)
        {
            if(points[i].size() < 2)
                continue;
            for(auto it=points[i].begin();it!=points[i].end();it++)
            {
                auto next = it;
                next++;
                if(next != points[i].end())
                {
                    double screenspace_x1 = area_width*((it->first-view_range_min_x)/view_area_width) + margin_x + total_x_padding;
                    double screenspace_x2 = area_width*((next->first-view_range_min_x)/view_area_width) + margin_x + total_x_padding;
                    double screenspace_top1 = height() - (margin_y + total_y_padding) - area_height*((it->second-view_range_min_y)/view_area_height) - selection_radius;
                    double screenspace_bot1 = height() - (margin_y + total_y_padding) - area_height*((it->second-view_range_min_y)/view_area_height) + selection_radius;
                    double screenspace_top2 = height() - (margin_y + total_y_padding) - area_height*((next->second-view_range_min_y)/view_area_height) - selection_radius;
                    double screenspace_bot2 = height() - (margin_y + total_y_padding) - area_height*((next->second-view_range_min_y)/view_area_height) + selection_radius;
                    if(point_is_in_quad(x,y,screenspace_x1,screenspace_top1,screenspace_x1,screenspace_bot1,
                                     screenspace_x2, screenspace_top2,  screenspace_x2, screenspace_bot2))
                    {
                        emit on_series_select(i);
                        return;
                    }
                }
            }
        }
    }
    else
    {
        for(int i=0;i<(int)points.size();i++)
        {
            if(points[i].size() < 2)
                continue;
            for(auto it=points[i].begin();it!=points[i].end();it++)
            {
                if(it->first < view_range_min_x || it->first > view_range_max_x)
                    continue;
                double screenspace_x = (double)view_to_screenspace_x(it->first);
                double screenspace_y = (double)view_to_screenspace_y(it->second);
                if(Common::distance(screenspace_x, screenspace_y, x, y) < 10.0)
                {
                    selection_x = it->first;
                    selection_y = it->second;
                    has_selection = 1;
                    emit on_point_select(i, it->first, it->second, screenspace_x, screenspace_y);
                    return;
                }
                /*
                auto next = it;
                next++;
                auto prev = it;
                if(prev == points[i].begin())
                    prev = points[i].end();
                else
                    prev--;
                double screenspace_x1;
                double screenspace_x2 = area_width*((it->first-view_range_min_x)/view_area_width) + margin_x + total_x_padding;
                double screenspace_x3;
                double screenspace_top1;
                double screenspace_bot1;
                double screenspace_top2;
                double screenspace_bot2;
                double screenspace_top3;
                double screenspace_bot3;
                if(prev != points[i].end())
                {
                    screenspace_x1 = ((((it->first - prev->first)/2.0 + prev->first)-view_range_min_x)/view_area_width)*area_width + margin_x + total_x_padding;
                    screenspace_top1 = height() - (margin_y+total_y_padding) - area_height*(((((it->second - prev->second)/2.0)+prev->second)-view_range_min_y)/view_area_height) - selection_radius;
                    screenspace_bot1 = height() - (margin_y+total_y_padding) - area_height*(((((it->second - prev->second)/2.0)+prev->second)-view_range_min_y)/view_area_height) + selection_radius;
                }
                else
                {
                    screenspace_x1 = area_width*((it->first-view_range_min_x)/view_area_width) + margin_x + total_x_padding - selection_radius;
                    screenspace_top1 = height() - (margin_y+total_y_padding) - area_height*((it->second - view_range_min_y)/view_area_height) - selection_radius;
                    screenspace_bot1 = height() - (margin_y+total_y_padding) - area_height*((it->second - view_range_min_y)/view_area_height) + selection_radius;
                }
                screenspace_top2 = height() - (margin_y+total_y_padding) - area_height*((it->second - view_range_min_y)/view_area_height) - selection_radius;
                screenspace_bot2 = height() - (margin_y+total_y_padding) - area_height*((it->second - view_range_min_y)/view_area_height) + selection_radius;
                if(next != points[i].end())
                {
                    screenspace_x3 = ((((next->first - it->first)/2.0 + it->first)-view_range_min_x)/view_area_width)*area_width + margin_x + total_x_padding;
                    screenspace_top3 = height() - (margin_y+total_y_padding) - area_height*(((((next->second - it->second)/2.0)+it->second)-view_range_min_y)/view_area_height) - selection_radius;
                    screenspace_bot3 = height() - (margin_y+total_y_padding) - area_height*(((((next->second - it->second)/2.0)+it->second)-view_range_min_y)/view_area_height) + selection_radius;
                }
                else
                {
                    screenspace_x3 = area_width*((it->first-view_range_min_x)/view_area_width) + margin_x + total_x_padding + selection_radius;
                    screenspace_top3 = height() - (margin_y+total_y_padding) - area_height*((it->second - view_range_min_y)/view_area_height) - selection_radius;
                    screenspace_bot3 = height() - (margin_y+total_y_padding) - area_height*((it->second - view_range_min_y)/view_area_height) + selection_radius;
                }
                if(point_is_in_quad(x,y,screenspace_x1,screenspace_top1,screenspace_x1,screenspace_bot1,
                                 screenspace_x2, screenspace_bot2,screenspace_x2, screenspace_top2))
                {
                    selection_x = it->first;
                    selection_y = it->second;
                    has_selection = 1;
                    emit on_point_select(i, it->first, it->second, (int)screenspace_x2, (int)((screenspace_bot2-screenspace_top2)/2.0+screenspace_top2));
                    return;
                }
                if(point_is_in_quad(x,y,screenspace_x2,screenspace_top2,screenspace_x2,screenspace_bot2,
                                 screenspace_x3, screenspace_bot3,screenspace_x3, screenspace_top3))
                {
                    selection_x = it->first;
                    selection_y = it->second;
                    has_selection = 1;
                    emit on_point_select(i, it->first, it->second, (int)screenspace_x2, (int)((screenspace_bot2-screenspace_top2)/2.0+screenspace_top2));
                    return;
                }*/
            }
        }
    }
}

int mc_chart::view_to_screenspace_x(uint64_t timestamp)
{
    if(timestamp < view_range_min_x || timestamp > view_range_max_x)
        return -1;
    double total_x_padding = Y_LABEL_WIDTH;
    if(!axis_visible)
        total_x_padding = 0.0;
    double area_width = width()-((double)margin_x*2)-total_x_padding*2;
    double view_area_width = view_range_max_x - view_range_min_x;
    return (int)(((timestamp - view_range_min_x)/view_area_width)*area_width + margin_x + total_x_padding);
}

int mc_chart::view_to_screenspace_y(float val)
{
    if(val < view_range_min_y || val > view_range_max_y)
        return -1;
    double total_y_padding = X_LABEL_WIDTH;
    if(!axis_visible)
        total_y_padding = 0.0;
    double area_height = height()-((double)margin_y*2)-total_y_padding;
    double view_area_height = view_range_max_y - view_range_min_y;
    return height() - margin_y - total_y_padding - ((val - view_range_min_y)/view_area_height)*area_height;
}

double mc_chart::get_triangle_area(double x1, double y1, double x2, double y2, double x3, double y3)
{
    return 0.5*fabs(x1*(y2-y3)+x2*(y3-y1)+x3*(y1-y2));
}

uint8_t mc_chart::point_is_in_quad(double px, double py, double x1, double y1, double x2, double y2,
                         double x3, double y3, double x4, double y4)
{
    double total_area = 0;
    total_area += get_triangle_area(px,py,x1,y1,x2,y2);
    total_area += get_triangle_area(px,py,x2,y2,x3,y3);
    total_area += get_triangle_area(px,py,x3,y3,x4,y4);
    total_area += get_triangle_area(px,py,x4,y4,x1,y1);

    double quad_area = 0;
    quad_area += get_triangle_area(x1,y1,x2,y2,x4,y4);
    quad_area += get_triangle_area(x4,y4,x2,y2,x3,y3);

    if(total_area > quad_area + Common::distance(x1,y1,x3,y3) * 0.001)
        return 0;
    return 1;
}

void mc_chart::trim_left()
{
    for(int i=0;i<(int)points.size();i++)
    {
        for(auto it=points[i].begin();it!=points[i].end();)
        {
            if(it->first < view_range_min_x)
            {
                auto next = it;
                next++;
                if(next == points[i].end())
                {
                    points[i].erase(it++);
                    continue;
                }
                else if(next->first > view_range_min_x)
                    break;
                else
                {
                    points[i].erase(it++);
                    continue;
                }
            }
            else
                break;
            it++;
        }
    }
}

void mc_chart::set_custom_left_bound(uint64_t left_bound)
{
    custom_left_bound = left_bound;
}

uint64_t mc_chart::get_custom_left_bound()
{
    return custom_left_bound;
}

void mc_chart::set_custom_right_bound(uint64_t right_bound)
{
    custom_right_bound = right_bound;
}

uint64_t mc_chart::get_custom_right_bound()
{
    return custom_right_bound;
}
