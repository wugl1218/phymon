#ifndef DIALOG_VISUALIZETIONSELECTION_H
#define DIALOG_VISUALIZETIONSELECTION_H

#include <QDialog>

#define MC_JUMPER_STATE_IDLE               0
#define MC_JUMPER_STATE_HIGHEST_UP_SPD0    1
#define MC_JUMPER_STATE_HIGHEST_UP_SPD1    2
#define MC_JUMPER_STATE_HIGHEST_UP_SPD2    3
#define MC_JUMPER_STATE_HIGHEST_DOWN_SPD0  4
#define MC_JUMPER_STATE_HIGHEST_DOWN_SPD1  5
#define MC_JUMPER_STATE_HIGHEST_DOWN_SPD2  6
#define MC_JUMPER_STATE_HIGHER_UP_SPD0     7
#define MC_JUMPER_STATE_HIGHER_UP_SPD1     8
#define MC_JUMPER_STATE_HIGHER_UP_SPD2     9
#define MC_JUMPER_STATE_HIGHER_DOWN_SPD0   10
#define MC_JUMPER_STATE_HIGHER_DOWN_SPD1   11
#define MC_JUMPER_STATE_HIGHER_DOWN_SPD2   12
#define MC_JUMPER_STATE_LOWER_UP_SPD0      13
#define MC_JUMPER_STATE_LOWER_UP_SPD1      14
#define MC_JUMPER_STATE_LOWER_UP_SPD2      15
#define MC_JUMPER_STATE_LOWER_DOWN_SPD0    16
#define MC_JUMPER_STATE_LOWER_DOWN_SPD1    17
#define MC_JUMPER_STATE_LOWER_DOWN_SPD2    18
#define MC_JUMPER_STATE_LOWEST_UP_SPD0     19
#define MC_JUMPER_STATE_LOWEST_UP_SPD1     20
#define MC_JUMPER_STATE_LOWEST_UP_SPD2     21
#define MC_JUMPER_STATE_LOWEST_DOWN_SPD0   22
#define MC_JUMPER_STATE_LOWEST_DOWN_SPD1   23
#define MC_JUMPER_STATE_LOWEST_DOWN_SPD2   24
namespace Ui {
class VisualizetionSelectionDialog;
}

class VisualizetionSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VisualizetionSelectionDialog(QWidget *parent = nullptr);
    ~VisualizetionSelectionDialog();
    void step();

private:
    Ui::VisualizetionSelectionDialog *ui;
    int jumper_state;
    int jumper_highest;
    int jumper_higher;
    int jumper_lower;
    int jumper_lowest;
    uint32_t jumper_last_trigger_time;
    uint32_t jumper_state_switch_time;
    time_t get_jumper_timeval();
    void check_and_modify_jumper_time();

    void trigger_highest_up();
    void trigger_highest_down();
    void trigger_higher_up();
    void trigger_higher_down();
    void trigger_lower_up();
    void trigger_lower_down();
    void trigger_lowest_up();
    void trigger_lowest_down();

};

#endif // DIALOG_VISUALIZETIONSELECTION_H
