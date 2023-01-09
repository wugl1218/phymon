#ifndef TAB_OBSERVATIONS_METRICITEMSDISPLAYCONFIGPAGE_WIDGET_H
#define TAB_OBSERVATIONS_METRICITEMSDISPLAYCONFIGPAGE_WIDGET_H

#include "mc_selection_entry.h"
#include "qcheckbox.h"
#include "qpushbutton.h"
#include <QWidget>
#include <QScrollArea>
#include <list>

namespace Ui {
class Tab_Observations_metricItemsDisplayConfigPage_Widget;
}

#define HIS_TYPE_ITEM_CHK 0
#define HIS_TYPE_DEVICE_CHK 1
#define HIS_TYPE_CLEAR 2
#define HIS_TYPE_SELECTALL 3

class HistoryEntry
{
public:
    int cmd; //One of HIS_TYPE_XXX defines.
    std::string param_str[2];
    int param[2];
};

class Tab_Observations_metricItemsDisplayConfigPage_Widget : public QWidget
{
    Q_OBJECT
//====================================================================
struct deviceListStruct {
    bool visible;
    QString name;
    QString description;
};

struct deviceMetricStruct {
    bool visible;
    QString description;
};
//====================================================================
public:
    explicit Tab_Observations_metricItemsDisplayConfigPage_Widget(QWidget *parent = nullptr);
    ~Tab_Observations_metricItemsDisplayConfigPage_Widget();
    void update_devices();
    bool nosave_checked =1;
    bool has_checked =1;


private:
    Ui::Tab_Observations_metricItemsDisplayConfigPage_Widget *ui;
    std::map<std::string, std::vector<std::string>> supported_settings;
    //std::vector<QPushButton*> btns[4];
    std::vector<QWidget*> left_lines[4];
    std::vector<QWidget*> right_lines[4];
    std::string tab_names[4];
    QWidget* leftscrollarea[4];
    QWidget* rightscrollarea[4];
    uint8_t has_init;
    std::list<HistoryEntry> undo_history;
    int num_devices;
    QCheckBox* checkboxes[4];
    void checkbox_event(int tab, bool checked);
    void _toggleDeviceTabEnable(int row);
    void restoreButtonsStatus(QScrollArea* tab);
    void apply_savina_ordering(std::vector<QWidget*>* lines, std::string model, int row);
    void refresh_geometry(std::vector<QWidget*>* lines);
    void select_all_pressed(int tab);
    void clear_pressed(int tab);
    void swap_entries(int source_type, mc_selection_entry* e);

signals:
    void changeToPrevPage();

private slots:
    void toggleEnableItemStatus(int row,int column);
    void toggleDeviceTabEnable(int row,int column);
    void on_save_pushButton_clicked();
    void on_restore_pushButton_clicked();
    void on_restoreAll_pushButton_clicked();
    void On_widgetShowed();
    void on_item_toggle(bool toggled);
    void on_checkBox_0_stateChanged(int arg1);
    void on_checkBox_1_stateChanged(int arg1);
    void on_checkBox_2_stateChanged(int arg1);
    void on_checkBox_3_stateChanged(int arg1);
    void on_entry_pressed(int type);
    void on_tab0_selectall_clicked();
    void on_tab0_clear_clicked();
    void on_tab1_selectall_clicked();
    void on_tab1_clear_clicked();
    void on_tab2_selectall_clicked();
    void on_tab2_clear_clicked();
    void on_tab3_selectall_clicked();
    void on_tab3_clear_clicked();

protected:
    void showEvent(QShowEvent *event) override;
};

#endif // TAB_OBSERVATIONS_METRICITEMSDISPLAYCONFIGPAGE_WIDGET_H
