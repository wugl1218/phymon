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
class jsontext;
}

class jsontext : public QDialog
{
    Q_OBJECT

public:
    explicit jsontext(QWidget *parent = nullptr);
    ~jsontext();
    void getText(QString jsonfile);
    void clearText();


private slots:
    void on_pushButton_clicked();

private:
    Ui::jsontext *ui;
protected:
    void mousePressEvent(QMouseEvent *event) override;

};

#endif // DIALOG_JSONTEXT_H
