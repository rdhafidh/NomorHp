#-------------------------------------------------
#
# Project created by QtCreator 2017-04-28T05:23:59
#
#-------------------------------------------------

QT       += core  sql gui-private sql concurrent
CONFIG += release c++14 console
DEFINES += 
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
win32-g++{
QMAKE_CXXFLAGS -= -O2
QMAKE_CXXFLAGS += -mtune=core2 -O3
}

TARGET = app_entry_nomor
TEMPLATE = app

include(xlsx/qtxlsx.pri)
include(excel/excel.pri)
SOURCES += main.cpp \
        mainwindow.cpp tabwidgetutama.cpp tableview/formtableview.cpp \
        util/sqlfetch.cpp util/utilproduk.cpp util/prosesbar.cpp \
    halamanpage.cpp \
    asklogin.cpp \
    halamanprovider.cpp numberparsertype.cpp \
    importnomorcsv.cpp \
    csvnumberparser.cpp \
    pengaturanhowmany.cpp util/settinghandler.cpp \
    tentangaplikasi.cpp \ 
    resetdatanomor.cpp \
    nomornonduplicate.cpp \
    importnomornonduplicate.cpp \
    numbernonduplicateparser.cpp \
    hapusdatanomordimaster.cpp \
    importhapusnomoronlyparser.cpp \
    hapusbyimportdatanomordimaster.cpp

INCLUDEPATH += util tableview

HEADERS  += mainwindow.h tabwidgetutama.h \
        tableview/formtableview.h \
         numberparsertype.h \
        tableview/sqlmodel.h  \
        tableview/tableaccesslevel.h  \
        tableview/tableviewedit.h \
        tableview/customslottable.h \
        util/sqlfetch.h \
        util/utilproduk.h util/prosesbar.h \
    halamanpage.h \
    asklogin.h \
    halamanprovider.h \ 
    importnomorcsv.h \
    csvnumberparser.h \
    pengaturanhowmany.h \
    tentangaplikasi.h \
    resetdatanomor.h \
    nomornonduplicate.h \
    importnomornonduplicate.h \
    numbernonduplicateparser.h \
    hapusdatanomordimaster.h \
    importhapusnomoronlyparser.h \
    hapusbyimportdatanomordimaster.h

win32*{
INCLUDEPATH += D:\masteraplikasi\transferh11nov\cpp-peglibsvn\4

}

FORMS    += menu.ui tableview/formtableview.ui  util/prosesbar.ui \
    halamanpage.ui \
    asklogin.ui \
    halamanprovider.ui \
    importnomorcsv.ui \
    pengaturanhowmany.ui \
    tentangaplikasi.ui \
    nomornonduplicate.ui \
    importnomornonduplicate.ui \
    hapusdatanomordimaster.ui

DISTFILES += \
    foo \
    csvlib.pri

RESOURCES += \
    resource.qrc

RC_FILE += res.rc

