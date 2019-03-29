#-------------------------------------------------
#
# Project created by QtCreator 2016-09-28T00:52:41
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = Experiment
TEMPLATE = app
CONFIG += c++14
QMAKE_CXXFLAGS  += /FS
QMAKE_CXXFLAGS  += /openmp
QMAKE_CXXFLAGS  += /Og /Ot

SOURCES += ui/main.cpp\
        camera/OrbitingCamera.cpp \
        camera/CamtransCamera.cpp \
        ui/abstractbehavior.cpp \
        ui/setupwindow.cpp \
        ui/prequeswindow.cpp \
        ui/overviewwindow.cpp \
        ui/taskintrowindow.cpp \
        ui/sessionintrowindow.cpp \
        ui/viswindow.cpp \
        ui/viswidget.cpp \
        ui/openquestionswindow.cpp \
        ui/breakwindow.cpp \
        ui/postqueswindow.cpp \
        ui/endwindow.cpp \
        utilities/modelloader.cpp\
        scene/scene.cpp \
        utilities/helpers.cpp \
        utilities/settings.cpp



HEADERS  += thirdparty/openvr/headers/openvr.h \
        thirdparty/openvr/headers/openvr_mingw.hpp \
        camera/OrbitingCamera.h \
        camera/CamtransCamera.h \
        ui/abstractbehavior.h \
        ui/setupwindow.h \
        ui/prequeswindow.h \
        ui/overviewwindow.h \
        ui/taskintrowindow.h \
        ui/sessionintrowindow.h \
        ui/viswindow.h \
        camera/Camera.h \
        ui/viswidget.h \
        ui/openquestionswindow.h \
        ui/breakwindow.h \
        ui/postqueswindow.h \
        ui/endwindow.h \
        utilities/modelloader.h\
        scene/scene.h \
        utilities/settings.h \
        utilities/helpers.h



FORMS    += \
    ui/setupwindow.ui \
    ui/prequeswindow.ui \
    ui/overviewwindow.ui \
    ui/taskintrowindow.ui \
    ui/sessionintrowindow.ui \
    ui/viswindow.ui \
    ui/openquestionswindow.ui \
    ui/breakwindow.ui \
    ui/postqueswindow.ui \
    ui/endwindow.ui

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

INCLUDEPATH += thirdparty thirdparty/glew/include thirdparty/openvr/headers
INCLUDEPATH += utilities textures camera ui scene img resources datasets thirdparty/include
DEPENDPATH +=  thirdparty thirdparty/openvr/headersthirdparty/glew/include
DEPENDPATH += utilities textures camera ui scene img resources thirdparty/include
DEFINES += _USE_MATH_DEFINES
DEFINES += TIXML_USE_STL
DEFINES += GLM_SWIZZLE GLM_FORCE_RADIANS

# from http://stackoverflow.com/a/10058744
# Copies the given files to the destination directory
defineTest(copyToDestdir) {
    files = $$1
    message("copy files")
    for(FILE, files) {
        CONFIG(debug, debug|release ) {
            DDIR = $${OUT_PWD}/debug
        } else {
            DDIR = $${OUT_PWD}/release
        }

        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        message($$DDIR)
        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

win32 {
#    RC_FILE = win32.rc
    DEFINES += GLEW_STATIC

    contains(QT_ARCH, i386) {
    message("win 32 bit build")
        LIBS += -L$$PWD/thirdparty/openvr/lib/win32/ \
                -lopenvr_api -lopengl32 -lglu32
        copyToDestdir($${PWD}/thirdparty/openvr/bin/win32/openvr_api.dll)
    } else {

    message("win 64 bit build")
        LIBS += -L$$PWD/thirdparty/openvr/lib/win64/ \
                -lopenvr_api -lopengl32 -lglu32
        copyToDestdir($${PWD}/thirdparty/openvr/bin/win64/openvr_api.dll)
    }
}

macx {
    QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
    QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
    CONFIG += c++11
}

DISTFILES += \
    img/scatterplots.png \
    datasets/participants.json \ \
    spaceholder

RESOURCES += \
    $$PWD/resources.qrc
