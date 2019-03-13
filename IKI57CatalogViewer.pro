#-------------------------------------------------
#
# Project created by QtCreator 2016-05-28T14:20:48
#
#-------------------------------------------------

QT       += core gui sql
QMAKE_LFLAGS += -Wl,--large-address-aware
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = IKI57CatalogViewer
TEMPLATE = app
CONFIG += c++14

SOURCES += main.cpp\
        mainwindow.cpp \
    customplot/qcustomplot.cpp \
    mathfunc.cpp \
    surroundcatalog.cpp \
    customplot/cxyplotter.cpp \
    customplot/cplotter.cpp

HEADERS  += mainwindow.h \
    customplot/qcustomplot.h \
    mathfunc.h \
    surroundcatalog.h \
    customplot/cxyplotter.h \
    customplot/cplotter.h \
    goodcolors.h \
    sqlitedb/sqlite3.h

FORMS    += mainwindow.ui
RC_FILE = viewerdisc.rc
RESOURCES += \
    viewer_rec.qrc

win32: LIBS += -L$$PWD/sqlitedb/ -lsqlite3

INCLUDEPATH += $$PWD/sqlitedb
DEPENDPATH += $$PWD/sqlitedb
