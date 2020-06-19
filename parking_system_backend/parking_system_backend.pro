QT       += core gui
QT       += serialport
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += .\lpr\include \
               .\opencv3410\include

LIBS += D:\works\parking_system\parking_system_backend\opencv3410\x64\mingw\lib\libopencv_*.a

HEADERS += \
    db_wrapper.h \
    lpr_wrapper.h \
    mainwindow.h \
    parking_system_superio_protocol.h \
    serial_service.h

SOURCES += \
    db_wrapper.cpp \
    lpr/src/CNNRecognizer.cpp \
    lpr/src/FastDeskew.cpp \
    lpr/src/FineMapping.cpp \
    lpr/src/Pipeline.cpp \
    lpr/src/PlateDetection.cpp \
    lpr/src/PlateSegmentation.cpp \
    lpr/src/Recognizer.cpp \
    lpr/src/SegmentationFreeRecognizer.cpp \
    lpr_wrapper.cpp \
    main.cpp \
    mainwindow.cpp

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = resource\main.ico
