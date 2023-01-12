#ifndef CARD_MDSCONNECTIVITY_H
#define CARD_MDSCONNECTIVITY_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QMouseEvent>

class Card_MDSConnectivity : public QWidget
{
    Q_OBJECT
public:
    explicit Card_MDSConnectivity(QWidget *parent = nullptr);
    void set_is_male(uint8_t is_male);
    uint8_t is_male();
    void set_id (std::string id);
    std::string get_id();
    void set_bed(std::string bed);
    std::string get_bed();
    void set_selected(uint8_t selected);
    uint8_t is_selected();
    void set_vmd(std::string vmd);
    std::string get_vmd();
    void set_RR(std::string RR);
    void set_IE(std::string IE);
    void set_MV(std::string MV);
    void set_VT(std::string VT);
    std::string get_RR();
    std::string get_IE();
    std::string get_MV();
    std::string get_VT();
    void perform_release(std::string employeeID ,std::string local_bed ,std::string local_vmd);
    void update_but();
    QLabel *title;
    QLabel *circle;
    bool eventFilter(QObject *watched, QEvent *event);
    std::map<std::string, int> patient_alarm;
    void set_alarm(std::map<std::string, int> patient_alarm);

private slots:
    void on_releasebtn_clicked();

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
    std::string vmd;
    std::string id;
    std::string bed;
    std::string RR;
    std::string IE;
    std::string MV;
    std::string VT;
    bool is_patient_alarm;
    bool is_Equipment_alarm;
    bool has_checked=0;

};


class CustomButton : public QPushButton
{
    Q_OBJECT

public:
    CustomButton(QWidget* parent = NULL);
    void set_is_male(uint8_t is_male);
    uint8_t is_male();
private:

    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void signalButtonClicked();


private:

    QList<QBrush> m_colorList;
    int m_pressIndex, m_enterIndex;
    uint8_t male;


};
#endif // CARD_MDSCONNECTIVITY_H
