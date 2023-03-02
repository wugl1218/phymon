#ifndef TAB_DEVICES_WIDGET_H
#define TAB_DEVICES_WIDGET_H

#include <QWidget>
#include <QAbstractButton>
#include <vector>
#include <string>
#include "Common.h"

namespace Ui {
class Tab_Devices_Widget;
}

class Tab_Devices_Widget : public QWidget
{
    Q_OBJECT

public:
    uint8_t active;
    int dev_index;
    Ui::Tab_Devices_Widget *ui;

    explicit Tab_Devices_Widget(QWidget *parent = nullptr);
    ~Tab_Devices_Widget();
    void update_devices(std::map<std::string, Device>* btns);
    void mapping_UI_reset();

private slots:
    void btn_clicked();
    void table_return();


protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private:
    std::vector<QAbstractButton*> devices;
};

#endif // TAB_DEVICES_WIDGET_H
