include(openglwindow.pri)

SOURCES += \
    main.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/gui/openglwindow
INSTALLS += target

nacl {
#QMAKE_LFLAGS += -s FULL_ES2=1
}
