INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
#LIBS += -L$$PWD/lib -ltbb

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp \
    $$PWD/mygl.cpp \
    $$PWD/scene/camera.cpp \
    $$PWD/scene/scene.cpp \
    $$PWD/openGL/drawable.cpp \
    $$PWD/openGL/glwidget277.cpp \
    $$PWD/openGL/shaderprogram.cpp \
    $$PWD/raytracing/intersection.cpp \
    $$PWD/raytracing/ray.cpp \
    $$PWD/scene/transform.cpp \
    $$PWD/tinyobj/tiny_obj_loader.cc \
    $$PWD/cameracontrolshelp.cpp \
    $$PWD/samplers/sampler.cpp \
    $$PWD/scene/geometry/mesh.cpp \
    $$PWD/warpfunctions.cpp \
    $$PWD/scene/bounds.cpp \
    $$PWD/samplers/poissonsampler.cpp \
    $$PWD/samplers/poissonbvh.cpp \
    $$PWD/raytracing/film.cpp

HEADERS += \
    $$PWD/mainwindow.h \
    $$PWD/mygl.h \
    $$PWD/scene/camera.h \
    $$PWD/la.h \
    $$PWD/scene/scene.h \
    $$PWD/openGL/drawable.h \
    $$PWD/openGL/glwidget277.h \
    $$PWD/openGL/shaderprogram.h \
    $$PWD/raytracing/intersection.h \
    $$PWD/raytracing/ray.h \
    $$PWD/scene/transform.h \
    $$PWD/tinyobj/tiny_obj_loader.h \
    $$PWD/cameracontrolshelp.h \
    $$PWD/globals.h \
    $$PWD/samplers/sampler.h \
    $$PWD/scene/geometry/mesh.h \
    $$PWD/scene/bounds.h \
    $$PWD/warpfunctions.h \
    $$PWD/samplers/poissonsampler.h \
    $$PWD/samplers/poissonbvh.h \
    $$PWD/raytracing/film.h
