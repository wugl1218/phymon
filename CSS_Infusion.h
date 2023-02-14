#ifndef CSS_INFUSION_H
#define CSS_INFUSION_H

#include "qcolor.h"
#include <QString>
class CSS
{
    public:
    //被禁用的按鈕 用於Utilities
    QString unEnabled_ButtonStyle         ="QPushButton {font-style:\"Arial\";color:rgb(255, 255, 255);background-color:rgb(49, 87, 120);border-style: none;border-width:1px;border-radius:20px;border-color: rgb(49, 87, 120);padding:8px 12px;border: none;}";
    QString Enabled_ButtonStyle           ="QPushButton {font-style:\"Arial\";color:rgb(154, 153, 150);background-color:rgb(49, 87, 120);border-style: none;border-width:1px;border-radius:20px;border-color: rgb(49, 87, 120);padding:8px 12px;border: none;}";
    //Device的按鈕ㄉ
    QString Device_ButtonStyle            ="QPushButton {font-style:\"Arial\";color:rgb(255, 255, 255);font-size:26px;background-color:rgb(8, 98, 202);border-style: solid;border-width:1px;border-radius:20px;border-color:  rgb(11, 42, 78);border:2px groove gray;padding:8px 12px;border: none;}QPushButton:checked{background-color: rgb(11, 42, 78);};";
    QString Select_Device_ButtonStyle     ="QPushButton {font-style:\"Arial\";color:rgb(255, 255, 255);font-size:26px;background-color:rgb(11, 42, 78);border-style: solid;border-width:1px;border-radius:20px;border-color:  rgb(11, 42, 78);border:2px groove gray;padding:8px 12px;border: none;}QPushButton:checked{background-color: rgb(11, 42, 78);};";
    //常用四態按鈕 用於Observations
    QString Enabled_Checked_ButtonStyle   ="QPushButton {font-style:\"Arial\";color:rgb(61, 56, 70);background-color:rgb(11, 42, 78);border-style: solid;border-width:1px;border-radius:20px;border-color:  rgb(11, 42, 78);border:2px groove gray;padding:20px 17px;border: none;}QPushButton:checked{background-color: rgb(11, 42, 78);};";
    QString Enabled_unChecked_ButtonStyle ="QPushButton {font-style:\"Arial\";color:rgb(61, 56, 70);background-color:rgb(8, 98, 202);border-style: solid;border-width:1px;border-radius:20px;border-color:  rgb(11, 42, 78);border:2px groove gray;padding:20px 17px;border: none;}QPushButton:checked{background-color: rgb(11, 42, 78);};";
    QString Checked_ButtonStyle           ="QPushButton {font-style:\"Arial\";color:rgb(255, 255, 255);background-color:rgb(11, 42, 78);border-style: solid;border-width:1px;border-radius:20px;border-color:  rgb(11, 42, 78);border:2px groove gray;padding:20px 17px;border: none;}QPushButton:checked{background-color: rgb(11, 42, 78);};";
    QString unChecked_ButtonStyle         ="QPushButton {font-style:\"Arial\";color:rgb(255, 255, 255);background-color:rgb(8, 98, 202);border-style: solid;border-width:1px;border-radius:20px;border-color:  rgb(11, 42, 78);border:2px groove gray;padding:20px 17px;border: none;}QPushButton:checked{background-color: rgb(11, 42, 78);};";
    QColor alarm_yellow                   =QColor(196, 160, 0);
    QColor alarm_red                      =QColor(164,0,0);
    QColor alarm_green                    =QColor(141, 195, 151);
    //topalarm背景色
    QString topalarm_mute_background     ="color:rgb(248, 208, 4);font-style:\"Arial\";background-color:rgb(44, 55,100);";
    QString topalarm_background          ="color:rgb(248, 208, 4);font-style:\"Arial\";background-color:rgb(7, 22, 40);";

};

#endif // CSS_INFUSION_H
