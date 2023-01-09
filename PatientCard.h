#ifndef PATIENTCARD_H
#define PATIENTCARD_H

#include <QWidget>

class PatientCard : public QWidget
{
    Q_OBJECT
public:
    explicit PatientCard(QWidget *parent = nullptr);
    void set_is_male(uint8_t is_male);
    uint8_t is_male();
    void set_selected(uint8_t selected);
    uint8_t is_selected();
    void set_bed(std::string bed);
    std::string get_bed();
    void set_mrn(std::string mrn);
    std::string get_mrn();
    void set_name(std::string name);
    std::string get_name();

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    uint8_t male;
    uint8_t selected;
    std::string bed;
    std::string mrn;
    std::string name;
};

#endif // PATIENTCARD_H
