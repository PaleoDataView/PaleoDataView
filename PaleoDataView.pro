#-------------------------------------------------
#
# Project created by QtCreator 2016-01-14T14:58:38
#
#-------------------------------------------------
# Version: 0.9.3.2

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PaleoDataView
TEMPLATE = app
RC_FILE = "PaleoDataView.rc"

include("QtXlsx/src/xlsx/qtxlsx.pri")
DEFINES += XLSX_NO_LIB

LIBS += -L"$$_PRO_FILE_PWD_/netCDF/bin" -lnetcdf -lhdf5 -lzlib1 -llibcurl

INCLUDEPATH += "netCDF/include"

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

FORMS += \
    Bacon/showbacon.ui \
    Dialog/busybacon.ui \
    Dialog/credits.ui \
    DTW/dtw.ui \
    Editor/editgraph.ui \
    Import/importall.ui \
    Dialog/about.ui \
    COR/amedit.ui \
    AMS/ams.ui \
    AMS/amsall.ui \
    AMS/amsedit.ui \
    Dialog/attdialog.ui \
    Bacon/bacon.ui \
    Calibration/calib.ui \
    Compare/compare.ui \
    Convert/convert.ui \
    COR/correlate.ui \
    ISO/dataview.ui \
    Export/export.ui \
    Extract/extract.ui \
    Hydro/hydro.ui \
    Import/import.ui \
    Dialog/inputdialog.ui \
    MainWindow/mainwindow.ui \
    Import/importpangaea.ui \
    Convert/convert_ra.ui \
    Extract/extract_single.ui \
    Dialog/busy.ui \
    COR/targetselect.ui \
    COR/targetimport.ui \
    COR/importstack.ui

HEADERS += \
    AMS/ams.h \
    AMS/amsall.h \
    AMS/amsdata.h \
    AMS/amsedit.h \
    Bacon/bacon.h \
    Bacon/showbacon.h \
    Calibration/calib.h \
    Compare/compare.h \
    Convert/convert.h \
    Convert/convert_ra.h \
    COR/amdata.h \
    COR/amedit.h \
    COR/correlate.h \
    COR/targetselect.h \
    COR/targetimport.h \
    COR/importstack.h \
    Dialog/about.h \
    Dialog/attdialog.h \
    Dialog/busybacon.h \
    Dialog/credits.h \
    Dialog/inputdialog.h \
    Dialog/busy.h \
    DTW/dtw.h \
    Editor/editgraph.h \
    Editor/editgraph_graph.h \
    Editor/graph.h \
    Editor/graphdata.h \
    Editor/graphgrid.h \
    Editor/graphobject.h \
    Export/export.h \
    Extract/extract.h \
    Extract/extract_single.h \
    General/inventory.h \
    General/resources.h \
    Hydro/hydro.h \
    Hydro/hydrodatabase.h \
    Import/import.h \
    Import/importall.h \
    Import/importpangaea.h \
    ISO/dataview.h \
    MainWindow/mainwindow.h \
    General/colorscheme.h

SOURCES += \
    AMS/ams.cpp \
    AMS/amsall.cpp \
    AMS/amsdata.cpp \
    AMS/amsedit.cpp \
    Bacon/bacon.cpp \
    Bacon/showbacon.cpp \
    Calibration/calib.cpp \
    Compare/compare.cpp \
    Convert/convert.cpp \
    Convert/convert_ra.cpp \
    COR/amdata.cpp \
    COR/amedit.cpp \
    COR/targetselect.cpp \
    COR/targetimport.cpp \
    COR/importstack.cpp \
    COR/correlate.cpp \
    Dialog/about.cpp \
    Dialog/attdialog.cpp \
    Dialog/busybacon.cpp \
    Dialog/credits.cpp \
    Dialog/inputdialog.cpp \
    Dialog/busy.cpp \
    DTW/dtw.cpp \
    Editor/editgraph.cpp \
    Editor/editgraph_graph.cpp \
    Editor/graph.cpp \
    Editor/graphdata.cpp \
    Editor/graphgrid.cpp \
    Editor/graphobject.cpp \
    Export/export.cpp \
    Extract/extract.cpp \
    Extract/extract_single.cpp \
    General/inventory.cpp \
    General/resources.cpp \
    Hydro/hydro.cpp \
    Hydro/hydrodatabase.cpp \
    Import/import.cpp \
    Import/importall.cpp \
    Import/importpangaea.cpp \
    ISO/dataview.cpp \
    MainWindow/mainwindow.cpp \
    main.cpp \
    General/colorscheme.cpp
