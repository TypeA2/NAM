#-------------------------------------------------
#
# Project created by QtCreator 2017-05-25T00:24:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NAM
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32:RC_ICONS += NieRAutomata_MAINICON.ico

SOURCES += main.cpp\
        nierautomatamediautil.cpp \
    namtreeview.cpp \
    namffplay_usm.cpp \
    namffmpeg_usm.cpp \
    filehandler_usm.cpp \
    filehandler_wem.cpp \
    namffmpeg_wem.cpp \
    sidewidget_wsp.cpp \
    convertdialog.cpp \
    namconversion.cpp \
    sidewidget_empty.cpp

HEADERS  += nierautomatamediautil.h \
    namtreeview.h \
    namfilehandler.h \
    sidewidget_wsp.h \
    convertdialog.h \
    namconversion.h \
    sidewidget_empty.h

DISTFILES +=
