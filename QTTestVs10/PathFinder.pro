# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = app
TARGET = PathFinder
DESTDIR = ../Win32/Debug
QT += core gui network
CONFIG += debug
DEFINES += QT_LARGEFILE_SUPPORT QT_DLL QT_NETWORK_LIB
INCLUDEPATH += ./../GeneralTrackerFunctions \
    ./GeneratedFiles \
    . \
    ./GeneratedFiles/Debug
LIBS += -l/"./../../opencv2.4.1/build/x86/vc10/lib/opencv_core241d/" \
    -l/"./../../opencv2.4.1/build/x86/vc10/lib/opencv_highgui241d/" \
    -l/"./../../opencv2.4.1/build/x86/vc10/lib/opencv_video241d/" \
    -l/"./../../opencv2.4.1/build/x86/vc10/lib/opencv_ml241d/" \
    -l/"./../../opencv2.4.1/build/x86/vc10/lib/opencv_legacy241d/" \
    -l/"./../../opencv2.4.1/build/x86/vc10/lib/opencv_imgproc241d/" \
    -l/"./../../opencv2.4.1/build/x86/vc10/lib/opencv_contrib241d/"
PRECOMPILED_HEADER = StdAfx.h
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/debug
OBJECTS_DIR += debug
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(PathFinder.pri)