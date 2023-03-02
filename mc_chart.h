#ifndef MC_CHART_H
#define MC_CHART_H

#include "qtimer.h"
#include <map>
#include <vector>
#include <QWidget>

#define MC_SELECT_SERIES 0
#define MC_SELECT_POINT  1
#define MC_SELECT_NONE   2

class mc_chart : public QWidget
{
    Q_OBJECT
public:
    mc_chart(QWidget *parent = nullptr);
    void add_point(int series_index, uint64_t timestamp, float val);
    void remove_point(int series_index, int index); //index=0 for most left
    void clear_points(int series_index);
    int get_num_points(int series_index);
    void set_series_color(int series_index, QColor color);
    QColor get_series_color(int series_index);
    void set_series_width(int series_index, int width);
    int get_series_width(int series_index);

    void set_view_range_min_x(uint64_t timestamp);
    void set_view_range_max_x(uint64_t timestamp);
    uint64_t get_view_range_min_x();
    uint64_t get_view_range_max_x();

    void set_view_range_min_y(float val);
    void set_view_range_max_y(float val);
    float get_view_range_min_y();
    float get_view_range_max_y();

    void set_num_labels_x(int n);
    void set_num_labels_y(int n);
    int get_num_labels_x();
    int get_num_labels_y();

    void set_margin_x(float pixels);
    void set_margin_y(float pixels);

    void set_scrollable(uint8_t scrollable);
    uint8_t is_scrollable();

    void set_selection_type(uint8_t type); //Use one of MC_SELECT_XXX defines.
    uint8_t get_selection_type();

    void set_selection_width(double width);
    double get_selection_width();

    void clear_selection();

    void set_zoomable(uint8_t zoomable);
    uint8_t is_zoomable();

    void set_axis_visible(uint8_t visible);
    uint8_t axis_is_visible();

    void set_max_zoom(uint64_t viewable_range);
    uint64_t get_max_zoom();
    void set_min_zoom(uint64_t viewable_range);
    uint64_t get_min_zoom();

    //Returns -1 if param falls outside view space.
    int view_to_screenspace_x(uint64_t timestamp);
    int view_to_screenspace_y(float val);

    void set_friction(double friction);
    double get_friction();

    uint64_t get_left_bounds();
    uint64_t get_right_bounds();

    void set_visible(uint8_t visible);
    uint8_t is_visible();
    void set_line_break_delta(int line_break_delta);

    void trim_left(); //Removes points left of view_range_min_x

    void set_custom_left_bound(uint64_t left_bound);
    uint64_t get_custom_left_bound();
    void set_custom_right_bound(uint64_t right_bound);
    uint64_t get_custom_right_bound();

protected:
    void paintEvent(QPaintEvent *event) override;
    bool event(QEvent* event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void on_series_select(int series_index);
    void on_point_select(int series_index, uint64_t timestamp, float val, int screenspace_x, int screenspace_y);
    void on_paint();
    void on_press();
    void clicked();

private slots:
    void worker();

private:
    int line_break_delta =0;
    std::vector<std::multimap<uint64_t, float>> points;
    std::vector<QColor> colors;
    std::vector<int> widths;
    uint64_t view_range_min_x;
    uint64_t view_range_max_x;
    float view_range_min_y;
    float view_range_max_y;
    int num_labels_x;
    int num_labels_y;
    float margin_x;
    float margin_y;
    std::vector<QPointF> scratch;
    uint8_t scrollable;
    uint8_t selection_type;
    double selection_width;
    uint8_t zoomable;
    uint8_t axis_visible;
    float press_start_x;
    float press_start_y;
    float press_last_x;
    float press_last_y;
    uint8_t press_is_stationary;
    uint64_t bounds_max_x;
    uint64_t bounds_min_x;
    double press_last_left;
    double press_last_right;
    uint8_t promoted_to_multitouch;
    uint64_t max_zoom;
    uint64_t min_zoom;
    double velocity;
    double friction;
    double last_velocity;
    QTimer timer;
    uint64_t selection_x;
    float selection_y;
    uint8_t has_selection;
    uint8_t visible;
    uint64_t custom_left_bound;
    uint64_t custom_right_bound;

    void handle_select(double x, double y);
    double get_triangle_area(double x1, double y1, double x2, double y2, double x3, double y3);
    uint8_t point_is_in_quad(double px, double py, double x1, double y1, double x2, double y2,
                             double x3, double y3, double x4, double y4);
    void calc_bounds();
    void handle_press(float x, float y);
    void handle_move(float x, float y);
    void handle_release(float x, float y);
};

#endif // MC_CHART_H
