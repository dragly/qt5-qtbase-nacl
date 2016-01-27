# DEFINES += QT_EMSCRIPTEN_USE_EMSCRIPTEN_FONT_ENGINE
# DEFINES += QT_EMSCRIPTEN_RUN_QT_ON_THREAD
# DEFINES += QT_EMSCRIPTEN_USE_QT_MESSAGE_LOOP

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/qemscriptenintegration.h \
#           $$PWD/qemscriptenbackingstore.h \
#           $$PWD/qemscriptenclipboard.h \
#           $$PWD/qemscriptencompositor.h \
           $$PWD/qemscriptencursor.h \
           $$PWD/qemscripteneventdispatcher.h \
#           $$PWD/qemscripteneventtranslator.h \
#           $$PWD/qemscriptenfontdatabase.h \
#           $$PWD/qemscriptenfontengine.h \
#           $$PWD/qemscriptenglcontext.h \
#           $$PWD/qemscriptenhelpers.h \
#           $$PWD/qemscriptenhelpers.h \
           $$PWD/qemscripteninstance_p.h \
#           $$PWD/qemscriptenintegration.h \
#           $$PWD/qemscriptenmodule_p.h \
#           $$PWD/qemscriptenwindow.h \
           $$PWD/qemscriptenscreen.h \
#           $$PWD/qemscriptenservices.h \
           $$PWD/qemscriptentheme.h \

SOURCES += \
           $$PWD/qemscriptenpluginmain.cpp \
#           $$PWD/qemscriptenbackingstore.cpp \
#           $$PWD/qemscriptenclipboard.cpp \
#           $$PWD/qemscriptencompositor.cpp \
           $$PWD/qemscriptencursor.cpp \
           $$PWD/qemscripteneventdispatcher.cpp \
#           $$PWD/qemscripteneventtranslator.cpp \
#           $$PWD/qemscriptenfontdatabase.cpp \
#           $$PWD/qemscriptenfontengine.cpp \
#           $$PWD/qemscriptenglcontext.cpp \
#           $$PWD/qemscriptenhelpers.cpp \
           $$PWD/qemscripteninstance_p.cpp \
           $$PWD/qemscriptenintegration.cpp \
#           $$PWD/qemscriptenmodule_p.cpp \
#           $$PWD/qemscriptenpluginmain.cpp \
           $$PWD/qemscriptenscreen.cpp \
#           $$PWD/qemscriptenservices.cpp \
           $$PWD/qemscriptentheme.cpp \
#           $$PWD/qemscriptenwindow.cpp \

RESOURCES += $$PWD/../../../../lib/fonts/naclfonts.qrc

OTHER_FILES += \
#               $$PWD/../../../../tools/nacldemoserver/check_browser.js \
#               $$PWD/../../../../tools/nacldemoserver/qtnaclloader.js \
#               $$PWD/qemscriptenfileaccess.js \
#               $$PWD/qemscriptenhelpers.js \

# Normally we would do this:
# QT += platformsupport-private
# However this file (emscripten.pri) is included in the QtGui build,
# which craetes a dependency conflict: QtGui needs platformsupport
# but platformsupport is normally built after QtGui.
# Include the files we need directly:
include($$PWD/../../../platformsupport/eventdispatchers/eventdispatchers.pri)
include($$PWD/../../../platformsupport/fontdatabases/basic/basic.pri)
INCLUDEPATH += $$PWD/../../../platformsupport/eventdispatchers

# Emscripten dependenices
emscripten {
#    include(emscripten.js.pri)
} else {
#    LIBS += -lppapi -lppapi_cpp -lppapi_gles2
#    !pnacl:nacl-newlib: LIBS += -lerror_handling
}
