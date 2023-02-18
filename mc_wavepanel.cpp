#include "mc_wavepanel.h"
#include "mc_wavepanel_add.h"

mc_wavepanel::mc_wavepanel(QWidget *parent)
    : QWidget{parent},
      menu(parent)
{
    controls_on = 1;
    main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(0,0,0,0);
    main_layout->setSpacing(0);
    header_layout = new QHBoxLayout();
    main_layout->addLayout(header_layout, 0);
    header_layout->setContentsMargins(0,0,0,0);
    header_layout->setSpacing(0);
    corner = new QLabel(this);
    header_layout->addWidget(corner,0);
    corner->setPixmap(QPixmap(":/icons/top_left_corner.png"));
    corner->setGeometry(0,0,42,56);
    header_bar = new QWidget(this);
    header_bar->setStyleSheet("background-color: rgb(19, 105, 205)");
    header_layout->addWidget(header_bar, 1);
    controls_btn = new mc_btn_Clickable(this);
    controls_btn->setPixmap(QPixmap(":/icons/controls_btn1.png"));
    controls_btn->setGeometry(0,0,59,56);
    controls_btn->setStyleSheet("background-color: rgb(19, 105, 205)");
    connect(controls_btn, SIGNAL(clicked()), this, SLOT(controls_clicked()));
    header_layout->addWidget(controls_btn, 0);
    item_layout = new QVBoxLayout();
    main_layout->addLayout(item_layout, 1);
    push_add_item();
}

void mc_wavepanel::add_clicked()
{
    menu.clear_tabs();
    std::vector<std::string> items;
    items.push_back("test1");
    items.push_back("test2");
    items.push_back("test3");
    items.push_back("test4");
    menu.add_tab("Savina", &items, 1);
    menu.exec();
    printf("selected item=%s, selected tab=%s\n", menu.selected_item.c_str(), menu.selected_tab_name.c_str());
}

static void mc_add_clicked(void* param)
{
    mc_wavepanel* wp = (mc_wavepanel*)param;
    wp->add_clicked();
}

void mc_wavepanel::push_add_item()
{
    mc_wavepanel_add* add_item = new mc_wavepanel_add(this, mc_add_clicked);
    add_item->setStyleSheet("background-color: rgb(9, 58, 115)");
    item_layout->addWidget(add_item, 1);
}

void mc_wavepanel::controls_clicked()
{
    controls_on = !controls_on;
    render_controls_btn();
}

void mc_wavepanel::render_controls_btn()
{
    if(controls_on)
        controls_btn->setPixmap(QPixmap(":/icons/controls_btn1.png"));
    else
        controls_btn->setPixmap(QPixmap(":/icons/controls_btn0.png"));
}
