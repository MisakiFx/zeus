QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    causecheck.cpp \
    causecheckhistory.cpp \
    causegrade.cpp \
    errorclass.cpp \
    evalute.cpp \
    leaveaskcontrol.cpp \
    leavingaskhistory.cpp \
    loginwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    model.cpp \
    reginstdialog.cpp \
    toolclass.cpp \
    zeusdao.cpp

HEADERS += \
    causecheck.h \
    causecheckhistory.h \
    causegrade.h \
    errorclass.h \
    evalute.h \
    leaveaskcontrol.h \
    leavingaskhistory.h \
    loginwindow.h \
    macro.h \
    mainwindow.h \
    model.h \
    readonlydelegate.h \
    reginstdialog.h \
    toolclass.h \
    zeusdao.h

FORMS += \
    causecheck.ui \
    causecheckhistory.ui \
    causegrade.ui \
    evalute.ui \
    leaveaskcontrol.ui \
    leavingaskhistory.ui \
    loginwindow.ui \
    mainwindow.ui \
    reginstdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
