#ifndef OBSERVATIONSTAB_HISTORY_WIDGET_H
#define OBSERVATIONSTAB_HISTORY_WIDGET_H

#include <QWidget>

namespace Ui {
class ObservationsTab_history_Widget;
}

class ObservationsTab_history_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit ObservationsTab_history_Widget(QWidget *parent = nullptr);
    ~ObservationsTab_history_Widget();

private:
    Ui::ObservationsTab_history_Widget *ui;

signals:
    void changeToRealtimePage();

private slots:
    void on_return_pushButton_clicked();
};

#endif // OBSERVATIONSTAB_HISTORY_WIDGET_H
