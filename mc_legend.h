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
    std::string mdccode;
    float val;

    QColor color;
};

class mc_legend : public QWidget
{
    Q_OBJECT
public:
    mc_legend(QWidget *parent = nullptr);
    void set_vertical_spacing(int spacing);
    int get_vertical_spacing();
    void set_square_width(int width);
    int get_square_width();
    void set_font_size(int size);
    int get_font_size();
    void set_text_color(QColor color);
    QColor get_text_color();

    void set_series_color(QColor color);
    QColor get_series_color();
    void set_series_text(
                         std::string text,
                         std::string model,
                         std::string unit,
                         std::string mdccode,
                         float val);
    std::string get_series_text();
    void remove_series();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void on_series_select(std::string model,std::string mdccode);

private:
    int square_width;
    int square_height;

    int vertical_spacing;
    int font_size;
    QColor text_color;
    mc_legend_entry entries;
};

#endif // MC_LEGEND_H
