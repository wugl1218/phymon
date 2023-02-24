#include "Dialog_select_menu.h"
#include "qpushbutton.h"
#include "ui_Dialog_select_menu.h"
#include <Common.h>

Dialog_select_menu::Dialog_select_menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_select_menu)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);
    Btn_spacing =20;
    Btn_width =251;
    Btn_height =81;
}

Dialog_select_menu::~Dialog_select_menu()
{
    delete ui;
}
void Dialog_select_menu::make_btn(std::vector<btn> btn)
{
    Common* common = Common::instance();
    int row=0,column=0;
    int max_row=0,max_column=0;
    for(auto it=btn.begin();it!=btn.end();++it)
    {
        QPushButton *Pushbtn = new QPushButton(this);
        Pushbtn->setGeometry(Btn_spacing+(Btn_width+Btn_spacing)*column,
                             Btn_spacing+(Btn_height+Btn_spacing)*row,
                      Btn_width, Btn_height);
        QFont font ;
        font.setPixelSize(24);
        font.setFamily("Arial [Mono]");
        Pushbtn->setText(QString::fromStdString(it->name));
        Pushbtn->setFont(font);
        if(it->is_select)
            Pushbtn->setStyleSheet(common->css.Checked_ButtonStyle);
        else
            Pushbtn->setStyleSheet(common->css.unChecked_ButtonStyle);
        Pushbtn->setProperty("index", it->index);
        connect(Pushbtn, SIGNAL(clicked()), this, SLOT(btn_clicked()));
        row++;
        if(row >= 5)
        {
            column++;
            row = 0;
            max_row=5;
        }
        btns.push_back(Pushbtn);
    }
    max_column=column+1;
    this->resize(Btn_spacing+(Btn_width+Btn_spacing)*max_column,Btn_spacing+(Btn_height+Btn_spacing)*max_row);
}
void Dialog_select_menu::btn_clicked()
{
    QPushButton* s = (QPushButton*)sender();
    select_btn = s->property("index").value<int>();
    qDebug()<<"select_btn"<<select_btn;
    close();
}
void Dialog_select_menu::mousePressEvent(QMouseEvent *event)
{

}
