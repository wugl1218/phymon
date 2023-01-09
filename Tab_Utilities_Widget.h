#ifndef TAB_UTILITIES_WIDGET_H
#define TAB_UTILITIES_WIDGET_H

#include <QWidget>
#include <QAbstractButton>

namespace Ui {
class Tab_Utilities_Widget;
}

class Tab_Utilities_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Tab_Utilities_Widget(QWidget *parent = nullptr);
    ~Tab_Utilities_Widget();
    uint8_t index_u8;//philo
    int addTab(QWidget *page, const QString &label);
    int addTab(QWidget *page, const QIcon &icon, const QString &label);
    int insertTab(int index, QWidget *page, const QString &label);
    int insertTab(int index, QWidget *page,
                              const QIcon &icon, const QString &label);


private:
    Ui::Tab_Utilities_Widget *ui;

private slots:
    void on_UtilitiesTab_tabWidget_currentChanged(int index);
};

#endif // TAB_UTILITIES_WIDGET_H
