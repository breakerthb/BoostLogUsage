QT       -= core gui

TARGET = test
TEMPLATE = app

DESTDIR = ../libs

INCLUDEPATH = ../MyLoger \

LIBS += -L../libs \
        -lMyLogger \
        -lboost_log \
        -lboost_system \


QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
    main.cpp
