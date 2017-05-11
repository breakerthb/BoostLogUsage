#-------------------------------------------------
#
# Project created by QtCreator 2017-05-08T10:48:53
#
#-------------------------------------------------

QT       -= core gui

TARGET = MyLogger
TEMPLATE = lib

DESTDIR = ../libs

DEFINES += BOOST_LOG_DYN_LINK

QMAKE_CXXFLAGS += -std=c++11

DEFINES +=

SOURCES += \
    logger.cpp

HEADERS +=\
    logger.h


