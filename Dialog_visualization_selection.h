#ifndef DIALOG_VISUALIZATION_SELECTION_H
#define DIALOG_VISUALIZATION_SELECTION_H

#include <QDialog>

namespace Ui {
class Dialog_visualization_selection;
}

class Dialog_visualization_selection : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_visualization_selection(QWidget *parent = nullptr);
    ~Dialog_visualization_selection();

private:
    Ui::Dialog_visualization_selection *ui;
};

#endif // DIALOG_VISUALIZATION_SELECTION_H
