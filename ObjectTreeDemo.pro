QT += widgets
CONFIG += c++17 console
CONFIG -= app_bundle
TARGET = ObjectTreeDemo
TEMPLATE = app
SOURCES += main.cpp mainwindow.cpp
HEADERS += mainwindow.h widgets.h
msvc:QMAKE_CXXFLAGS += /utf-8
