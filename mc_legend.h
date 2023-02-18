#ifndef MC_LEGEND_H
#define MC_LEGEND_H

#include "qtimer.h"
#include <map>
#include <vector>
#include <QWidget>

#define MC_SELECT_SERIES 0
#define MC_SELECT_POINT  1
#define MC_SELECT_NONE   2

class mc_legend_entry
{
public:
    std::string name;
    std::string model;
    std::string unit;
    float val;

    QColor color;
};

class mc_legend : public QWidget
{
    Q_OBJECT
public:
    mc_legend(QWidget *parent = nullptr);
    void set_left_margin(int margin);
    int get_left_margin();
    void set_top_margin(int margin);
    int get_top_margin();
    void set_vertical_spacing(int spacing);
    int get_vertical_spacing();
    void set_square_width(int width);
    int get_square_width();
    void set_font_size(int size);
    int get_font_size();
    void set_text_color(QColor color);
    QColor get_text_color();

    void set_series_color(int series_index, QColor color);
    QColor get_series_color(int series_index);
    void set_series_text(int series_index,
                         std::string text,
                         std::string model,
                         std::string unit,
                         float val);
    std::string get_series_text(int series_index);
    void remove_series(int series_index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void on_series_select(int series_index);

private:
    int left_margin;
    int top_margin;
    int square_width;
    int square_height;

    int vertical_spacing;
    int font_size;
    QColor text_color;
    std::vector<mc_legend_entry> entries;
};

#endif // MC_LEGEND_H
