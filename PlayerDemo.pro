QT += core gui opengl
QT += openglwidgets multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += -I /usr/local/Cellar/ffmpeg/4.4.1_5/include

LIBS += -L/usr/local/Cellar/ffmpeg/4.4.1_5/lib -lavutil -lavcodec -lavformat -lswscale -lswresample

SOURCES += \
    audioplayer.cpp \
    decoder.cpp \
    demux.cpp \
    main.cpp \
    mainwindow.cpp \
    resampler.cpp \
    videowidget.cpp

HEADERS += \
    audioplayer.h \
    decoder.h \
    demux.h \
    mainwindow.h \
    resampler.h \
    videowidget.h

FORMS += \
    mainwindow.ui
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
