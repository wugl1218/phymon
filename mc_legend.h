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
    std::string y_max;
    std::string y_min;
    std::string datasource;
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
    void set_square_height(int height);

    void set_font_size(int size);
    int get_font_size();
    void set_text_color(QColor color);
    QColor get_text_color();

    void set_series_color(QColor color);
    QColor get_series_color();
    void set_series_text(std::string text,
                         std::string model,
                         std::string unit,
                         std::string mdccode,
                         float val,
                         std::string y_min,
                         std::string y_max,
                         std::string datasource);
    std::string get_series_text();
    void remove_series();
    std::string get_name(){return entries.name;};
    std::string get_unit(){return entries.unit;};
    std::string get_mdccode(){return entries.mdccode;};
    std::string get_model(){return entries.model;};
    std::string get_y_min(){return entries.y_min;};
    std::string get_y_max(){return entries.y_max;};
    std::string get_datasource(){return entries.datasource;};


protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void on_series_select(std::string name,std::string model,std::string mdccode,std::string y_min,std::string y_max,
                          std::string unit,std::string datasource);

private:
    int square_width;
    int square_height;
    mc_legend_entry entries;
    int vertical_spacing;
    int font_size;
    QColor text_color;
};

#endif // MC_LEGEND_H
