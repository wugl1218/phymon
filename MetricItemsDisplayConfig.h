#ifndef METRICITEMSDISPLAYCONFIG_H
#define METRICITEMSDISPLAYCONFIG_H

#include <QWidget>

namespace Ui {
class MetricItemsDisplayConfigForm;
}

class MetricItemsDisplayConfigForm : public QWidget
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
    QString type;
    QString description;
};
//====================================================================
public:
    explicit MetricItemsDisplayConfigForm(QWidget *parent = nullptr);
    ~MetricItemsDisplayConfigForm();

protected:
    void showEvent(QShowEvent *event);

private:
    Ui::MetricItemsDisplayConfigForm *ui;
    //
    void _toggleDeviceTabVisible(int row);

signals:
    void backToMainWidget();

private slots:
    void toggleEnableItemStatus(int row,int column);
    void toggleDeviceTabVisible(int row,int column);
    void on_close_pushButton_clicked();
    void on_restore_pushButton_clicked();
};

#endif // METRICITEMSDISPLAYCONFIG_H
