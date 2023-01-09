#ifndef CSS_INFUSION_H
#define CSS_INFUSION_H

#include <QString>
class CSS
{
    public:
    //被禁用的按鈕 用於Utilities
    QString unEnabled_ButtonStyle         ="QPushButton {font-style:\"Arial\";color:rgb(154, 153, 150);background-color:rgb(49, 87, 120);border-style: none;border-width:1px;border-radius:20px;border-color: rgb(49, 87, 120);padding:8px 12px;border: none;}";
    QString Enabled_ButtonStyle           ="QPushButton {font-style:\"Arial\";color:rgb(255, 255, 255);background-color:rgb(49, 87, 120);border-style: none;border-width:1px;border-radius:20px;border-color: rgb(49, 87, 120);padding:8px 12px;border: none;}";
    //Device的按鈕ㄉ
    QString Device_ButtonStyle            ="QPushButton {font-style:\"Arial\";color:rgb(255, 255, 255);font-size:26px;background-color:rgb(8, 98, 202);border-style: solid;border-width:1px;border-radius:20px;border-color:  rgb(11, 42, 78);border:2px groove gray;padding:8px 12px;border: none;}QPushButton:checked{background-color: rgb(11, 42, 78);};";
    QString Select_Device_ButtonStyle     ="QPushButton {font-style:\"Arial\";color:rgb(255, 255, 255);font-size:26px;background-color:rgb(11, 42, 78);border-style: solid;border-width:1px;border-radius:20px;border-color:  rgb(11, 42, 78);border:2px groove gray;padding:8px 12px;border: none;}QPushButton:checked{background-color: rgb(11, 42, 78);};";
    //常用四態按鈕 用於Observations
    QString Enabled_Checked_ButtonStyle   ="QPushButton {font-style:\"Arial\";color:rgb(61, 56, 70);background-color:rgb(11, 42, 78);border-style: solid;border-width:1px;border-radius:20px;border-color:  rgb(11, 42, 78);border:2px groove gray;padding:20px 17px;border: none;}QPushButton:checked{background-color: rgb(11, 42, 78);};";
    QString Enabled_unChecked_ButtonStyle ="QPushButton {font-style:\"Arial\";color:rgb(61, 56, 70);background-color:rgb(8, 98, 202);border-style: solid;border-width:1px;border-radius:20px;border-color:  rgb(11, 42, 78);border:2px groove gray;padding:20px 17px;border: none;}QPushButton:checked{background-color: rgb(11, 42, 78);};";
    QString Checked_ButtonStyle           ="QPushButton {font-style:\"Arial\";color:rgb(255, 255, 255);background-color:rgb(11, 42, 78);border-style: solid;border-width:1px;border-radius:20px;border-color:  rgb(11, 42, 78);border:2px groove gray;padding:20px 17px;border: none;}QPushButton:checked{background-color: rgb(11, 42, 78);};";;
    QString unChecked_ButtonStyle         ="QPushButton {font-style:\"Arial\";color:rgb(255, 255, 255);background-color:rgb(8, 98, 202);border-style: solid;border-width:1px;border-radius:20px;border-color:  rgb(11, 42, 78);border:2px groove gray;padding:20px 17px;border: none;}QPushButton:checked{background-color: rgb(11, 42, 78);};";;
};

#endif // CSS_INFUSION_H
