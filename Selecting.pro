#-------------------------------------------------
#
# Project created by QtCreator 2017-11-03T20:02:51
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

TARGET = Selecting
TEMPLATE = app

VERSION = 1.0.0.1
QMAKE_TARGET_PRODUCT = Selecting Objects
QMAKE_TARGET_COPYRIGHT = Egor Ivanov
QMAKE_TARGET_DESCRIPTION = For UIR_2017

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    mgraphics.cpp \
    core.cpp

HEADERS += \
        mainwindow.h \
    single_area.h \
    mgraphics.h \
    core.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    resource.qrc

RC_ICONS = Icon.ico
