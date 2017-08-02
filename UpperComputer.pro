#-------------------------------------------------
#
# Project created by QtCreator 2017-07-21T08:59:44
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += gui

TARGET = UpperComputer
TEMPLATE = app


SOURCES += main.cpp\
        uppercomputer.cpp \
    qtsqlite.cpp \
    qextserialport.cpp \
    qextserialbase.cpp \
    win_qextserialport.cpp \
    signup.cpp \
    recharge.cpp

HEADERS  += uppercomputer.h \
    qtsqlite.h \
    qextserialbase.h \
    win_qextserialport.h \
    qextserialport.h \
    signup.h \
    recharge.h

FORMS    += uppercomputer.ui \
    signup.ui \
    recharge.ui

RESOURCES += \
    src.qrc

RC_ICONS = park.ico
RC_FILE= park.rc
