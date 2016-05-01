#-------------------------------------------------
#
# Project created by QtCreator 2012-05-18T02:36:49
#
#-------------------------------------------------

QT       += core gui network widgets websockets

CONFIG   += c++11
win32:CONFIG -= embed_manifest_dll
win32:CONFIG += embed_manifest_exe

TARGET = server

SOURCES += main.cpp\
        mainwindow.cpp \
    world.cpp \
    block.cpp \
    player.cpp \
    incoming.cpp \
    logger.cpp \
    position.cpp

HEADERS  += mainwindow.h \
    world.h \
    block.h \
    player.h \
    incoming.h \
    logger.h \
    position.h \
    SendPlayerData.h

QMAKE_INFO_PLIST = Info.plist

OTHER_FILES += ../client/* Info.plist

FORMS    += mainwindow.ui
