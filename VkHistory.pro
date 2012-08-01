#-------------------------------------------------
#
# Project created by QtCreator 2012-07-09T22:21:55
#
#-------------------------------------------------

QT       += gui network webkit

TARGET = VkHistory
TEMPLATE = app

SOURCES += main.cpp\
    vk_auth.cpp \
    mainwindow.cpp \
    vk_api.cpp

HEADERS  += \
    vk_auth.h \
    mainwindow.h \
    vk_api.h

FORMS    += \
    mainwindow.ui

win32: LIBS += -L$$PWD/qjson-qjson/lib/ -lqjson

INCLUDEPATH += $$PWD/qjson-qjson/include
DEPENDPATH += $$PWD/qjson-qjson/include

win32: PRE_TARGETDEPS += $$PWD/qjson-qjson/lib/qjson.lib

OTHER_FILES += \
    HtmlBefore.txt
