QT += gui
QT += xml

CONFIG += c++1Z console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS += -msse2
#QMAKE_CXXFLAGS += -fopenmp
#LIBS += -fopenmp

SOURCES += src/main.cpp \
src/guide.cpp \
    src/gedge.cpp \
    src/gmask.cpp \
    src/gpos.cpp \
    src/gtemp.cpp

HEADERS += src/guide.h \
    src/gedge.h \
    src/gmask.h \
    src/gpos.h \
    src/gtemp.h

INCLUDEPATH += lib/
macx {
    INCLUDEPATH += /usr/local/include/opencv4
    LIBS += -L/usr/local/lib/\
     -lopencv_calib3d.4.3.0\
     -lopencv_core.4.3.0\
     -lopencv_features2d.4.3.0 \
     -lopencv_flann.4.3.0\
     -lopencv_highgui.4.3.0\
     -lopencv_imgproc.4.3.0\
     -lopencv_ml.4.3.0\
     -lopencv_objdetect.4.3.0\
     -lopencv_photo.4.3.0\
     -lopencv_stitching.4.3.0\
     -lopencv_superres.4.3.0\
     -lopencv_video.4.3.0\
     -lopencv_videostab.4.3.0

    DEPENDPATH += /usr/local/include/opencv4
}
