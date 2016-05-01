#-------------------------------------------------
#
# Project created by QtCreator 2012-05-18T02:36:49
#
#-------------------------------------------------

QT       += core gui network
QT       += widgets

TARGET = server
    win32:CONFIG -= embed_manifest_dll
    win32:CONFIG += embed_manifest_exe

SOURCES += main.cpp\
        mainwindow.cpp \
    world.cpp \
    block.cpp \
    player.cpp \
    incoming.cpp \
    logger.cpp \
    ext/qtwebsocket/QtWebSocket/QWsSocket.cpp \
    ext/qtwebsocket/QtWebSocket/QWsServer.cpp \
    position.cpp

HEADERS  += mainwindow.h \
    world.h \
    block.h \
    player.h \
    incoming.h \
    logger.h \
    ext/qtwebsocket/QtWebSocket/QWsSocket.h \
    ext/qtwebsocket/QtWebSocket/QWsServer.h \
    position.h \
    SendPlayerData.h

INCLUDEPATH += C:\Users\fatcomp\Reep\sonicawe\winlib


FORMS    += mainwindow.ui
