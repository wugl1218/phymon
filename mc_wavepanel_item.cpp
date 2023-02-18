#include "mc_wavepanel_item.h"

mc_wavepanel_item::mc_wavepanel_item(QWidget *parent)
    : QWidget{parent}
{
    controls_on = 0;
    main_layout = new QHBoxLayout(this);
    this->setLayout(main_layout);
    main_layout->setContentsMargins(0,0,0,0);
    main_layout->setSpacing(0);
}

void mc_wavepanel_item::set_controls_enabled(uint8_t enabled)
{
    if(controls_on)
    {
        if(enabled == 0)
        {
            controls_on = 0;
            disable_controls();
        }
    }
    else
    {
        if(enabled == 1)
        {
            controls_on = 1;
            enable_controls();
        }
    }
}

void mc_wavepanel_item::enable_controls()
{

}

void mc_wavepanel_item::disable_controls()
{

}
