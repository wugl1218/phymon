#include "Dialog_visualizetionSelection.h"
#include "ui_Dialog_visualizetionSelection.h"
#include "Common.h"

VisualizetionSelectionDialog::VisualizetionSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VisualizetionSelectionDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);

}

VisualizetionSelectionDialog::~VisualizetionSelectionDialog()
{
    delete ui;
}/*
void VisualizetionSelectionDialog::step()
{
    Common* common = Common::instance();
    uint32_t current_time = Common::get_time_ms();
    switch(jumper_state)
    {
    case MC_JUMPER_STATE_IDLE:
        return;
    case MC_JUMPER_STATE_HIGHEST_UP_SPD0:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_state_duration0)
        {
            jumper_state = MC_JUMPER_STATE_HIGHEST_UP_SPD1;
            jumper_last_trigger_time = current_time;
            jumper_state_switch_time = current_time;
            trigger_highest_up();
        }
        break;
    case MC_JUMPER_STATE_HIGHEST_UP_SPD1:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_slow)
        {
            jumper_last_trigger_time = current_time;
            trigger_highest_up();
        }
        if(Common::get_elapsed_time(current_time, jumper_state_switch_time) >= common->button_hold_state_duration1)
        {
            jumper_state = MC_JUMPER_STATE_HIGHEST_UP_SPD2;
            jumper_state_switch_time = current_time;
        }
        break;
    case MC_JUMPER_STATE_HIGHEST_UP_SPD2:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_fast)
        {
            jumper_last_trigger_time = current_time;
            trigger_highest_up();
        }
        break;
    case MC_JUMPER_STATE_HIGHEST_DOWN_SPD0:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_state_duration0)
        {
            jumper_state = MC_JUMPER_STATE_HIGHEST_DOWN_SPD1;
            jumper_last_trigger_time = current_time;
            jumper_state_switch_time = current_time;
            trigger_highest_down();
        }
        break;
    case MC_JUMPER_STATE_HIGHEST_DOWN_SPD1:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_slow)
        {
            jumper_last_trigger_time = current_time;
            trigger_highest_down();
        }
        if(Common::get_elapsed_time(current_time, jumper_state_switch_time) >= common->button_hold_state_duration1)
        {
            jumper_state = MC_JUMPER_STATE_HIGHEST_DOWN_SPD2;
            jumper_state_switch_time = current_time;
        }
        break;
    case MC_JUMPER_STATE_HIGHEST_DOWN_SPD2:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_fast)
        {
            jumper_last_trigger_time = current_time;
            trigger_highest_down();
        }
        break;
    case MC_JUMPER_STATE_HIGHER_UP_SPD0:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_state_duration0)
        {
            jumper_state = MC_JUMPER_STATE_HIGHER_UP_SPD1;
            jumper_last_trigger_time = current_time;
            jumper_state_switch_time = current_time;
            trigger_higher_up();
        }
        break;
    case MC_JUMPER_STATE_HIGHER_UP_SPD1:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_slow)
        {
            jumper_last_trigger_time = current_time;
            trigger_higher_up();
        }
        if(Common::get_elapsed_time(current_time, jumper_state_switch_time) >= common->button_hold_state_duration1)
        {
            jumper_state = MC_JUMPER_STATE_HIGHER_UP_SPD2;
            jumper_state_switch_time = current_time;
        }
        break;
    case MC_JUMPER_STATE_HIGHER_UP_SPD2:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_fast)
        {
            jumper_last_trigger_time = current_time;
            trigger_higher_up();
        }
        break;
    case MC_JUMPER_STATE_HIGHER_DOWN_SPD0:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_state_duration0)
        {
            jumper_state = MC_JUMPER_STATE_HIGHER_DOWN_SPD1;
            jumper_last_trigger_time = current_time;
            jumper_state_switch_time = current_time;
            trigger_higher_down();
        }
        break;
    case MC_JUMPER_STATE_HIGHER_DOWN_SPD1:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_slow)
        {
            jumper_last_trigger_time = current_time;
            trigger_higher_down();
        }
        if(Common::get_elapsed_time(current_time, jumper_state_switch_time) >= common->button_hold_state_duration1)
        {
            jumper_state = MC_JUMPER_STATE_HIGHER_DOWN_SPD2;
            jumper_state_switch_time = current_time;
        }
        break;
    case MC_JUMPER_STATE_HIGHER_DOWN_SPD2:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_fast)
        {
            jumper_last_trigger_time = current_time;
            trigger_higher_down();
        }
        break;
    case MC_JUMPER_STATE_LOWER_UP_SPD0:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_state_duration0)
        {
            jumper_state = MC_JUMPER_STATE_LOWER_UP_SPD1;
            jumper_last_trigger_time = current_time;
            jumper_state_switch_time = current_time;
            trigger_lower_up();
        }
        break;
    case MC_JUMPER_STATE_LOWER_UP_SPD1:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_slow)
        {
            jumper_last_trigger_time = current_time;
            trigger_lower_up();
        }
        if(Common::get_elapsed_time(current_time, jumper_state_switch_time) >= common->button_hold_state_duration1)
        {
            jumper_state = MC_JUMPER_STATE_LOWER_UP_SPD2;
            jumper_state_switch_time = current_time;
        }
        break;
    case MC_JUMPER_STATE_LOWER_UP_SPD2:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_fast)
        {
            jumper_last_trigger_time = current_time;
            trigger_lower_up();
        }
        break;
    case MC_JUMPER_STATE_LOWER_DOWN_SPD0:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_state_duration0)
        {
            jumper_state = MC_JUMPER_STATE_LOWER_DOWN_SPD1;
            jumper_last_trigger_time = current_time;
            jumper_state_switch_time = current_time;
            trigger_lower_down();
        }
        break;
    case MC_JUMPER_STATE_LOWER_DOWN_SPD1:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_slow)
        {
            jumper_last_trigger_time = current_time;
            trigger_lower_down();
        }
        if(Common::get_elapsed_time(current_time, jumper_state_switch_time) >= common->button_hold_state_duration1)
        {
            jumper_state = MC_JUMPER_STATE_LOWER_DOWN_SPD2;
            jumper_state_switch_time = current_time;
        }
        break;
    case MC_JUMPER_STATE_LOWER_DOWN_SPD2:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_fast)
        {
            jumper_last_trigger_time = current_time;
            trigger_lower_down();
        }
        break;
    case MC_JUMPER_STATE_LOWEST_UP_SPD0:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_state_duration0)
        {
            jumper_state = MC_JUMPER_STATE_LOWEST_UP_SPD1;
            jumper_last_trigger_time = current_time;
            jumper_state_switch_time = current_time;
            trigger_lowest_up();
        }
        break;
    case MC_JUMPER_STATE_LOWEST_UP_SPD1:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_slow)
        {
            jumper_last_trigger_time = current_time;
            trigger_lowest_up();
        }
        if(Common::get_elapsed_time(current_time, jumper_state_switch_time) >= common->button_hold_state_duration1)
        {
            jumper_state = MC_JUMPER_STATE_LOWEST_UP_SPD2;
            jumper_state_switch_time = current_time;
        }
        break;
    case MC_JUMPER_STATE_LOWEST_UP_SPD2:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_fast)
        {
            jumper_last_trigger_time = current_time;
            trigger_lowest_up();
        }
        break;
    case MC_JUMPER_STATE_LOWEST_DOWN_SPD0:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_state_duration0)
        {
            jumper_state = MC_JUMPER_STATE_LOWEST_DOWN_SPD1;
            jumper_last_trigger_time = current_time;
            jumper_state_switch_time = current_time;
            trigger_lowest_down();
        }
        break;
    case MC_JUMPER_STATE_LOWEST_DOWN_SPD1:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_slow)
        {
            jumper_last_trigger_time = current_time;
            trigger_lowest_down();
        }
        if(Common::get_elapsed_time(current_time, jumper_state_switch_time) >= common->button_hold_state_duration1)
        {
            jumper_state = MC_JUMPER_STATE_LOWEST_DOWN_SPD2;
            jumper_state_switch_time = current_time;
        }
        break;
    case MC_JUMPER_STATE_LOWEST_DOWN_SPD2:
        if(Common::get_elapsed_time(current_time, jumper_last_trigger_time) >= common->button_hold_tick_interval_fast)
        {
            jumper_last_trigger_time = current_time;
            trigger_lowest_down();
        }
    default:
        break;
    }
}
*/
