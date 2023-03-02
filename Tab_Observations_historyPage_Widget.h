#ifndef TAB_OBSERVATIONS_HISTORYPAGE_WIDGET_H
#define TAB_OBSERVATIONS_HISTORYPAGE_WIDGET_H

#include "qtimer.h"
#include <QWidget>
#include "Common.h"

#define MC_JUMPER_STATE_IDLE            0
#define MC_JUMPER_STATE_HOUR_UP_SPD0    1
#define MC_JUMPER_STATE_HOUR_UP_SPD1    2
#define MC_JUMPER_STATE_HOUR_UP_SPD2    3
#define MC_JUMPER_STATE_HOUR_DOWN_SPD0  4
#define MC_JUMPER_STATE_HOUR_DOWN_SPD1  5
#define MC_JUMPER_STATE_HOUR_DOWN_SPD2  6
#define MC_JUMPER_STATE_MIN_UP_SPD0     7
#define MC_JUMPER_STATE_MIN_UP_SPD1     8
#define MC_JUMPER_STATE_MIN_UP_SPD2     9
#define MC_JUMPER_STATE_MIN_DOWN_SPD0   10
#define MC_JUMPER_STATE_MIN_DOWN_SPD1   11
#define MC_JUMPER_STATE_MIN_DOWN_SPD2   12

namespace Ui {
class Tab_Observations_historyPage_Widget;
}

class Tab_Observations_historyPage_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Tab_Observations_historyPage_Widget(QWidget *parent = nullptr);
    ~Tab_Observations_historyPage_Widget();
    void clear_selection();
    void reset_time_jumper();
    void step();
    void set_title_text(std::string mdccode,
                        std::string model,
                        std::string name,
                        std::string y_min,
                        std::string y_max,
                        std::string unit,
                        std::string datasource);
protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private:
    Ui::Tab_Observations_historyPage_Widget *ui;
    QTimer* test_timer;
    uint64_t current_time;
    QTimer update_timer;
    uint8_t reposition;
    std::multimap<uint64_t, float> pts;
    uint8_t right_locked;
    QTimer worker;
    uint64_t last_left_bound;
    uint64_t last_right_bound;
    int jumper_min;
    int jumper_hour;
    int jumper_state;
    uint32_t jumper_last_trigger_time;
    uint32_t jumper_state_switch_time;

    void render_jumper_min();
    void render_jumper_hour();
    void update_jumper_date();
    void check_and_modify_jumper_time();
    time_t get_jumper_timeval();
    void trigger_hour_up();
    void trigger_hour_down();
    void trigger_min_up();
    void trigger_min_down();

signals:
    void changeToPrevPage();

private slots:
    void on_return_pushButton_clicked();
    void test_timer_triggered();
    void on_series_select(int series_index);
    void on_point_select(int series_index, uint64_t timestamp, float val, int screenspace_x, int screenspace_y);
    void update_triggered();
    void on_PAWpushButton_clicked();
    void on_FLOWpushButton_clicked();
    void on_RVpushButton_clicked();
    void on_worker();
    void on_apply_btn_clicked();
    void on_hour_up_pressed();
    void on_hour_up_released();
    void on_hour_down_pressed();
    void on_hour_down_released();
    void on_min_up_pressed();
    void on_min_up_released();
    void on_min_down_pressed();
    void on_min_down_released();
    void on_day_dropdown_currentIndexChanged(int index);
};

#endif // TAB_OBSERVATIONS_HISTORYPAGE_WIDGET_H
