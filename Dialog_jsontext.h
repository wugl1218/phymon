#ifndef DIALOG_JSONTEXT_H
#define DIALOG_JSONTEXT_H

#include <QDialog>
#include <QWidget>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

namespace Ui {
class Dialog_jsontext;
}

class Dialog_jsontext : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_jsontext(QWidget *parent = nullptr);
    ~Dialog_jsontext();
    void getText(QString jsonfile);
    void clearText();


private slots:
    void on_pushButton_clicked();

private:
    Ui::Dialog_jsontext *ui;
protected:
    void mousePressEvent(QMouseEvent *event) override;

};

#endif // DIALOG_JSONTEXT_H
