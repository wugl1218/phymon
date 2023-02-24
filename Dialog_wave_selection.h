#ifndef DIALOG_WAVE_SELECTION_H
#define DIALOG_WAVE_SELECTION_H

#include <QDialog>
#include <vector>
#include <string>
#include <stdint.h>
#include "Common.h"

namespace Ui {
class Dialog_wave_selection;
}

class Dialog_wave_selection : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_wave_selection(QWidget *parent = nullptr);
    ~Dialog_wave_selection();
    void clear_tabs();
    void add_tab(const char* tab_name, std::vector<stDisplayItems>* items, uint8_t has_loop);

    std::string selected_tab_name;
    std::string selected_item;
    int m_selected_index;
private slots:
    void clicked();

private:
    Ui::Dialog_wave_selection *ui;

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // DIALOG_WAVE_SELECTION_H
