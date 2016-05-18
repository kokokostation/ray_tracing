#-------------------------------------------------
#
# Project created by QtCreator 2016-04-23T16:57:30
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ray_tracing
TEMPLATE = app

SOURCES += main.cpp\
    main_window.cpp \
    geometry.cpp \
    primitive.cpp \
    tracer.cpp \
    picture.cpp \
    light.cpp \
    kd_tree.cpp \
    continuous_performer.cpp \
    parcer.cpp

HEADERS  += \
    main_window.h \
    geometry.h \
    primitive.h \
    tracer.h \
    picture.h \
    light.h \
    kd_tree.h \
    continuous_performer.h \
    parcer.h

FORMS    +=

QMAKE_CXXFLAGS += -std=c++1y -pthread
LIBS += -pthread
