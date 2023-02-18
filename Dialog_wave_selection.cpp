#include "Dialog_wave_selection.h"
#include "mc_btn_Clickable.h"
#include "ui_Dialog_wave_selection.h"

Dialog_wave_selection::Dialog_wave_selection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_wave_selection)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);
    ui->tabWidget->removeTab(0);
}

Dialog_wave_selection::~Dialog_wave_selection()
{
    delete ui;
}

void Dialog_wave_selection::clear_tabs()
{
    for(int i=ui->tabWidget->count()-1;i>=0;i--)
        ui->tabWidget->removeTab(i);
}

void Dialog_wave_selection::add_tab(const char* tab_name, std::vector<std::string>* items, uint8_t has_loop)
{
    QWidget* w = new QWidget();
    w->setStyleSheet("background-color: rgb(0,73,159);");
    ui->tabWidget->addTab(w, tab_name);
    QVBoxLayout* main_layout = new QVBoxLayout(w);
    main_layout->setContentsMargins(24,24,24,24);
    main_layout->setSpacing(11);
    QGridLayout* g = new QGridLayout();
    g->setContentsMargins(0,0,0,0);
    g->setSpacing(0);
    main_layout->addLayout(g, 1);
    int i=0;
    int r=0;
    int c=0;
    std::string even_style = "background-color: rgb(0,93,206); color: rgb(255,255,255);";
    std::string odd_style = "background-color: rgb(0,83,183); color: rgb(255,255,255);";
    for(;i<(int)items->size();i++)
    {
        if(i>=30)
            break;
        mc_btn_Clickable* b = new mc_btn_Clickable();
        g->addWidget(b, r, c);
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        b->setText((*items)[i].c_str());
        b->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        QFont f = b->font();
        f.setPixelSize(19);
        b->setFont(f);
        if(i%2 == 0)
            b->setStyleSheet(even_style.c_str());
        else
            b->setStyleSheet(odd_style.c_str());
        connect(b, SIGNAL(clicked()), this, SLOT(clicked()));
        c++;
        if(c>=3)
        {
            c = 0;
            r++;
        }
    }
    for(;i<30;i++)
    {
        QWidget* b = new QWidget();
        g->addWidget(b, r, c);
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        if(i%2 == 0)
            b->setStyleSheet(even_style.c_str());
        else
            b->setStyleSheet(odd_style.c_str());
        c++;
        if(c>=3)
        {
            c = 0;
            r++;
        }
    }
    if(has_loop)
    {
        mc_btn_Clickable* b = new mc_btn_Clickable();
        main_layout->addWidget(b, 0);
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        b->setText("Loops");
        b->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        b->setMargin(22);
        QFont f = b->font();
        f.setPixelSize(19);
        b->setFont(f);
        b->setStyleSheet(odd_style.c_str());
        connect(b, SIGNAL(clicked()), this, SLOT(clicked()));
    }
}

void Dialog_wave_selection::clicked()
{
    mc_btn_Clickable* b = (mc_btn_Clickable*)sender();
    selected_item = b->text().toStdString();
    selected_tab_name = ui->tabWidget->tabText(ui->tabWidget->currentIndex()).toStdString();
    close();
}

void Dialog_wave_selection::mousePressEvent(QMouseEvent *event)
{

}
