#include "Dialog_jsontext.h"
#include "ui_Dialog_jsontext.h"
#include "ui_Tab_Utilities_exporterPage_Widget.h"
#include "qevent.h"


jsontext::jsontext(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::jsontext)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);
    connect(ui->returnbtn, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));

}

jsontext::~jsontext()
{
    delete ui;
}
void jsontext::getText(QString jsonfile)
{
    if (jsonfile=="") return;
    QString label ="../output/";
    label += jsonfile;
    ui->fileName_label->setText(jsonfile);
    QFile file(label);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Cannot open file for reading:" << qPrintable(file.errorString());
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readAll();
        line.replace(QString(","), QString(",\n"));
        line.replace(QString("}"), QString("}\n"));
        ui->json_text->setText(line);
        ui->json_text->setFont(QFont("Arial[Mono]",16));
    }

    file.close();
}
void jsontext::clearText()
{
    ui->json_text->clear();
}

void jsontext::on_pushButton_clicked()
{
    close();
}
void jsontext::mousePressEvent(QMouseEvent *event)
{
}

