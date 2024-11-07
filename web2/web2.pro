QT       += core gui
QT += multimedia
QT += concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dirnamewidget.cpp \
    imagegroup.cpp \
    imagewidget.cpp \
    main.cpp \
    mainwindow.cpp \
    mylistwidgetitem.cpp \
    show.cpp

HEADERS += \
    dirnamewidget.h \
    imagegroup.h \
    imagewidget.h \
    mainwindow.h \
    mylistwidgetitem.h \
    path.h \
    show.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
