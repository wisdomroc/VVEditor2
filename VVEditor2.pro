#-------------------------------------------------
#
# Project created by QtCreator 2022-03-21T11:02:26
#
#-------------------------------------------------

QT       += core gui xml

TRANSLATIONS += VVEditor_zh.ts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VVEditor
TEMPLATE = app
CONFIG += resources_big

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.

#这里可以切换是否使用_fixed文件或者是SQLITE：USE_FIXED_FILE、USE_SQLITE
DEFINES += USE_FIXED_FILE
DEFINES += USE_SQLITE

include(../mainprogram/WRP_GROUP.pri)
include(Condition/condition.pri)

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        vv_tree_item.cpp \
        vv_tree_model.cpp \
        beat_arrow.cpp \
        add_beat_dlg.cpp \
        configuration_dlg.cpp \
        select_signal_dlg.cpp \
        icd_data.cpp \
        graphics_scene.cpp \
        graphics_view.cpp \
        data_factory.cpp \
        command_item.cpp \
        WRP_Project/SingletonProject.cpp \
        WRP_Project/WRP_Project.cpp \
        tooltip.cpp \
        formula_config_dlg.cpp \
        add_command_dlg.cpp \
        treeview.cpp \
        button_delegate.cpp \
        treeviewr.cpp \
        w_headerview.cpp

HEADERS += \
        mainwindow.h \
        vv_typedef.h \
        vv_tree_item.h \
        vv_tree_model.h \
        beat_arrow.h \
        add_beat_dlg.h \
        add_command_dlg.h \
        configuration_dlg.h \
        select_signal_dlg.h \
        icd_data.h \
        graphics_scene.h \
        graphics_view.h \
        data_factory.h \
        vv_common.h \
        command_item.h \
        WRP_Project/SingletonProject.h \
        WRP_Project/WRP_Project.h \
        tooltip.h \
        formula_config_dlg.h \
        treeview.h \
        right_delegate.h \
        button_delegate.h \
        treeviewr.h \
        w_headerview.h

FORMS += \
        mainwindow.ui \
        add_beat_dlg.ui \
        add_command_dlg.ui \
        configuration_dlg.ui \
        select_signal_dlg.ui \
        formula_config_dlg.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    #src.qrc \
    #font.qrc \
    #src.qrc
    font.qrc


LIBS += -lICDController
#LIBS += -lconfiguration
#LIBS += -lDataVisualizationAnalysisLib
LIBS += -lTimeLib

#win32:CONFIG(debug,debug|release){
#LIBS += -lPublicLibraryd
#LIBS += -lQt5Xlsxd
#}else{
#LIBS += -lPublicLibrary
#LIBS += -lQt5Xlsx
#}

#whp add, only for convenient test, none copy
#DESTDIR = ../Dependent/bin_debug
#whp add end
QMAKE_POST_LINK += xcopy /y/F \"$$SOURCE_CP_DIR/./*.exe\" \"$$DEST_CP_DIR\\\" $$escape_expand(\\n\\t)

RC_FILE = logo.rc

DISTFILES += \
    logo.rc
