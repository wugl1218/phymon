#ifndef TAB_MAPPING_WIDGET_H
#define TAB_MAPPING_WIDGET_H

#include <QWidget>
#include <QDate>
#include "Card_Patient.h"
#include "Dialog_forcerelease.h"
#include "qtimer.h"
#include "Dialog_release.h"
#include <vector>


namespace Ui {
class Tab_Mapping_Widget;
}

class Tab_Mapping_Widget : public QWidget
{
    Q_OBJECT

public:
    Ui::Tab_Mapping_Widget *ui;
    explicit Tab_Mapping_Widget(QWidget *parent = nullptr);
    ~Tab_Mapping_Widget();
    void update_patients();
    void select_patient(int i);
    void update_devices();
    void force_release();
    void force_return();
    void perform_release(std::string employeeID);
    void mapping_UI_reset();
    void hide_windows();
    Dialog_forcerelease frd;


private slots:
    void on_releasebtn_clicked();
    void on_returnbtn_clicked();

    void worker();
    void patient_clicked();

signals:
    void monitorsignal();
    void bindingChanged();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QPointF last_pos;
    QTimer worker_timer;
    double velocity;
    double last_velocity;
    double friction;
    uint8_t press_is_stationary;
    QPointF press_pos;
    std::vector<Card_Patient*> cards;
    int selected_patient;
    void on_itemSelectionChanged();
};

#endif // TAB_MAPPING_WIDGET_H
