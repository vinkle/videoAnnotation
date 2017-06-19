#-------------------------------------------------
#
# Project created by QtCreator 2017-05-31T01:40:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = surgicalAnnotation
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
INCLUDEPATH += /usr/local/opencv2/

LIBS += -L/usr/local/lib
LIBS     += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video

SOURCES += main.cpp\
        interface.cpp \
    objectsAnnotation.cpp \
    objectdescription.cpp \
    mainGraphicsView.cpp \
    qtopencvviewergl.cpp \
    cornergrabber.cpp \
    polygonDrawing.cpp \
    graphicsscene.cpp \
    rectangleDrawing.cpp \
    tracker/BB.cpp \
    tracker/BBPredict.cpp \
    tracker/FBTrack.cpp \
    tracker/Lk.cpp \
    tracker/Median.cpp \
    tracker/MedianFlowTracker.cpp

HEADERS  += interface.h \
    objectsAnnotation.h \
    objectdescription.h \
    mainGraphicsView.h \
    qtopencvviewergl.h \
    cornergrabber.h \
    polygonDrawing.h \
    graphicsscene.h \
    rectangleDrawing.h \
    tracker/BB.h \
    tracker/BBPredict.h \
    tracker/FBTrack.h \
    tracker/Lk.h \
    tracker/Median.h \
    tracker/MedianFlowTracker.h

FORMS    += interface.ui \
    objectdescription.ui
