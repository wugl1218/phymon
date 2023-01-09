#ifndef MC_LOOP_H
#define MC_LOOP_H

#include "qtimer.h"
#include <QWidget>
#include <vector>

class mc_loop : public QWidget
{
    Q_OBJECT
public:
    explicit mc_loop(QWidget *parent = nullptr);
    void add_point(double x, double y);
    void clear_points();
    void save_points_as_ref();
    void clear_ref_points();
    int get_num_points();
    int get_num_ref_points();

    void set_view_range_min_x(double x);
    void set_view_range_max_x(double x);
    double get_view_range_min_x();
    double get_view_range_max_x();

    void set_view_range_min_y(double val);
    void set_view_range_max_y(double val);
    double get_view_range_min_y();
    double get_view_range_max_y();

    void set_margin_x(int pixels);
    void set_margin_y(int pixels);

    void animation_start(uint32_t duration_ms);

    void set_x_label(const char* str);
    void set_y_label(const char* str);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void on_press();
    void animation_finished();

private slots:
    void worker();

private:
    std::vector<QPointF> points;
    uint8_t animating;
    uint32_t start_time;
    uint32_t end_time;
    std::vector<QPointF> ref_points;
    double view_range_min_x;
    double view_range_max_x;
    double view_range_min_y;
    double view_range_max_y;
    int margin_x;
    int margin_y;
    QTimer timer;
    std::vector<QPointF> scratch;
    std::string x_label;
    std::string y_label;
};

#endif // MC_LOOP_H
