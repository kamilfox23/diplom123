QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = THE_GAME
TEMPLATE = app


SOURCES += main.cpp \
    scene3d.cpp

HEADERS  += \
    scene3d.h

RESOURCES += \
    Textures.qrc
