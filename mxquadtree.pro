QT += core gui widgets

CONFIG += c++11
CONFIG += static

SOURCES += \
    frontend/main.cpp \
    frontend/mainwindow.cpp \
    frontend/quadtreeviewer.cpp \
    mxquadtree.cpp

HEADERS += \
    frontend/mainwindow.h \
    frontend/quadtreeviewer.h \
    mxquadtree.h

FORMS += \
    frontend/mainwindow.ui
