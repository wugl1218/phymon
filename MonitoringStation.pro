QT       += core gui multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += RTI_UNIX \
           RTI_LINUX \
           RTI_64BIT \
#           DDSTOPIC_EMBEDDED \

SOURCES += \
    BarcodeDaemon.cpp \
    Dialog_topalarm_mute.cpp \
    Manager_MDSConnectivity_Card.cpp \
    Manager_Patient_Card.cpp \
    Common.cpp \
    Dialog_forcerelease.cpp \
    Dialog_jsontext.cpp \
    Dialog_loopselection.cpp \
    Dialog_msg.cpp \
    Dialog_network.cpp \
    Dialog_release.cpp \
    Dialog_visualization_config.cpp \
    Dialog_visualization_selection.cpp \
    Dialog_wave_selection.cpp \
    MainDialog.cpp \
    Manager_Device.cpp \
    Manager_MDSConnectivity.cpp \
    Manager_Patient.cpp \
    Manager_Topalarm.cpp \
    NetworkManager.cpp \
    Tab_Devices_Widget.cpp \
    Tab_Domaininput.cpp \
    Tab_Mapping_Widget.cpp \
    Tab_NS_Monitor.cpp \
    Tab_Observations_Widget.cpp \
    Tab_Observations_historyPage_Widget.cpp \
    Tab_Observations_mainPage_Widget.cpp \
    Tab_Observations_metricItemsDisplayConfigPage_Widget.cpp \
    Tab_Utilities_Widget.cpp \
    Tab_Utilities_alarmHistoryPage_Widget.cpp \
    Tab_Utilities_exporterPage_Widget.cpp \
    Thread_network.cpp \
    exporter.cpp \
    log.cpp \
    main.cpp \
    mc_btn_Clickable.cpp \
    mc_btn_topalart.cpp \
    mc_chart.cpp \
    mc_legend.cpp \
    mc_loop.cpp \
    mc_navigation.cpp \
    mc_selection_entry.cpp \
    mc_visualization.cpp \
    mc_visualization_setting.cpp \
    mc_visualization_setting_item.cpp \
    mc_wavepanel.cpp \
    mc_wavepanel_add.cpp \
    mc_wavepanel_item.cpp

HEADERS += \
    BarcodeDaemon.h \
    CSS_Infusion.h \
    Dialog_topalarm_mute.h \
    Manager_MDSConnectivity_Card.h \
    Manager_Patient_Card.h \
    Common.h \
    Dialog_forcerelease.h \
    Dialog_jsontext.h \
    Dialog_loopselection.h \
    Dialog_msg.h \
    Dialog_network.h \
    Dialog_release.h \
    Dialog_visualization_config.h \
    Dialog_visualization_selection.h \
    Dialog_wave_selection.h \
    MainDialog.h \
    Manager_Device.h \
    Manager_MDSConnectivity.h \
    Manager_Patient.h \
    Manager_Topalarm.h \
    NetworkManager.h \
    Tab_Devices_Widget.h \
    Tab_Domaininput.h \
    Tab_Mapping_Widget.h \
    Tab_NS_Monitor.h \
    Tab_Observations_Widget.h \
    Tab_Observations_historyPage_Widget.h \
    Tab_Observations_mainPage_Widget.h \
    Tab_Observations_metricItemsDisplayConfigPage_Widget.h \
    Tab_Utilities_Widget.h \
    Tab_Utilities_alarmHistoryPage_Widget.h \
    Tab_Utilities_exporterPage_Widget.h \
    Thread_network.h \
    exporter.h \
    log.h \
    mc_btn_Clickable.h \
    mc_btn_topalart.h \
    mc_chart.h \
    mc_legend.h \
    mc_loop.h \
    mc_navigation.h \
    mc_selection_entry.h \
    mc_visualization.h \
    mc_visualization_setting.h \
    mc_visualization_setting_item.h \
    mc_wavepanel.h \
    mc_wavepanel_add.h \
    mc_wavepanel_item.h

FORMS += \
    Dialog_forcerelease.ui \
    Dialog_jsontext.ui \
    Dialog_loopselection.ui \
    Dialog_msg.ui \
    Dialog_network.ui \
    Dialog_release.ui \
    Dialog_topalarm_mute.ui \
    Dialog_visualization_config.ui \
    Dialog_visualization_selection.ui \
    Dialog_wave_selection.ui \
    MainDialog.ui \
    Tab_Devices_Widget.ui \
    Tab_Domaininput.ui \
    Tab_Mapping_Widget.ui \
    Tab_NS_Monitor.ui \
    Tab_Observations_Widget.ui \
    Tab_Observations_historyPage_Widget.ui \
    Tab_Observations_mainPage_Widget.ui \
    Tab_Observations_metricItemsDisplayConfigPage_Widget.ui \
    Tab_Utilities_Widget.ui \
    Tab_Utilities_alarmHistoryPage_Widget.ui \
    Tab_Utilities_exporterPage_Widget.ui

DEPENDPATH += /opt/rti_connext_dds-6.1.0/lib/x64Linux4gcc7.3.0 \


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$PWD/rapidjson \
    /home/agooda/rti_connext_dds-6.1.0/include \
    /home/agooda/rti_connext_dds-6.1.0/include/ndds \
    /home/agooda/rti_connext_dds-6.1.0/include/ndds/hpp \
    /home/agooda/couchbase-lite-C/build/output/include \
    $$PWD/../miot/CBLPlugin \
    $$PWD/../miot/DDSPlugin \
    $$PWD/../miot/FHIRPlugin \
    $$PWD/../miot/PluginInterface \
    $$PWD/../miot/PluginManager \
    $$PWD/../miot/UtilsPlugin


LIBS += -L/home/agooda/rti_connext_dds-6.1.0/lib/x64Linux4gcc7.3.0 -lnddscpp2 -lnddsc -lnddscore -ldl -lm -lpthread \
        -L/home/agooda/opt/dds_collector/lib -lpluginmanager \
        -L/home/agooda/couchbase-lite-C/build/output/lib/x86_64-linux-gnu -lcblite

RESOURCES += \
    resource.qrc

DISTFILES +=
