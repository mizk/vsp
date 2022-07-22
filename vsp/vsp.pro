QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    globaldef.cpp \
    main.cpp \
    mainwindow.cpp \
    moviedialog.cpp \
    optiondialog.cpp \
    processor.cpp \
    processorthread.cpp \
    vlist.cpp \
    vtask.cpp

HEADERS += \
    globaldef.h \
    mainwindow.h \
    moviedialog.h \
    optiondialog.h \
    processor.h \
    processorthread.h \
    vlist.h \
    vtask.h

FORMS += \
    mainwindow.ui \
    moviedialog.ui \
    optiondialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    main.qrc

RC_ICONS = logo.ico

TRANSLATIONS += lang.ts \
