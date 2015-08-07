#-------------------------------------------------
#
# Project created by QtCreator 2013-12-04T15:21:31
#
#-------------------------------------------------

QT       += core xml

QT       -= gui

TARGET = RayTracerFull
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH +=C:\QTFiles\include

LIBS += "C:\QTFiles\lib\freeglut.lib" "C:\QTFiles\lib\glew32.lib" "C:\QTFiles\lib\FreeImage.lib"

SOURCES += main.cpp \
    View.cpp \
    Texture.cpp \
    Sphere.cpp \
    Scenegraph.cpp \
    Plane.cpp \
    Material.cpp \
    Light.cpp \
    Cylinder.cpp \
    Cone.cpp \
    Box.cpp

HEADERS += \
    View.h \
    Texture.h \
    Sphere.h \
    Scenegraph.h \
    Ray.h \
    Plane.h \
    ObjectXMLReader.h \
    Object.h \
    Node.h \
    Material.h \
    Light.h \
    HitRecord.h \
    Group.h \
    FreeImage.h \
    Cylinder.h \
    Cone.h \
    Box.h

OTHER_FILES += \
    white.png \
    Thumbs.db \
    road.png \
    RayTracerFull.pro.user \
    object.xml \
    lighting-texturing.vert \
    lighting-texturing.frag \
    input-sphere.txt \
    input.txt \
    FreeImage.dll \
    earthmap.jpg \
    checkerboard.jpg \
    bare.vert \
    bare.frag \
    SimpleScene.txt
