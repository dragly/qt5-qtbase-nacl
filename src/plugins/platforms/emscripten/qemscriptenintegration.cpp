/****************************************************************************
**
** Copyright (C) 2015 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt for Native Client platform plugin.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qemscriptenintegration.h"

// #include "qemscriptenbackingstore.h"
// #include "qemscriptenclipboard.h"
// #include "qemscriptencompositor.h"
 #include "qemscripteneventdispatcher.h"
// #include "qemscriptenfontdatabase.h"
// #include "qemscriptenglcontext.h"
 #include "qemscripteninstance_p.h"
 #include "qemscriptenscreen.h"
// #include "qemscriptenservices.h"
 #include "qemscriptentheme.h"
// #include "qemscriptenwindow.h"

#include <QtCore/qdebug.h>
#include <QtGui/QSurface>
#include <qpa/qplatformwindow.h>
#include <qpa/qwindowsysteminterface.h>

void *QEmscriptenPlatformNativeInterface::nativeResourceForIntegration(const QByteArray &resource)
{
    qDebug() << __PRETTY_FUNCTION__;
    // if (resource == "Instance")
        // return QEmscriptenInstancePrivate::getPPInstance();
    return 0;
}

QPlatformIntegration *qt_create_emscripten_integration()
{
    qDebug() << __PRETTY_FUNCTION__;
    return QEmscriptenIntegration::create();
}

QEmscriptenIntegration *QEmscriptenIntegration::create()
{
    qDebug() << __PRETTY_FUNCTION__;
//    if (QEmscriptenInstancePrivate::get() == 0) {
//        qFatal("ERROR: QEmscriptenInstance is not created. Use Q_GUI_MAIN instead of main().");
//        return 0;
//    }
    return new QEmscriptenIntegration();
}

static QEmscriptenIntegration *globalEmscriptenIntegration;
QEmscriptenIntegration *QEmscriptenIntegration::get() { return globalEmscriptenIntegration; }

QEmscriptenIntegration::QEmscriptenIntegration()
    : m_clipboard(0)
    , m_eventDispatcher(0)
    , m_fontDatabase(0)
    , m_services(0)
    , m_topLevelWindow(0)
    , m_compositor(0)
    , m_eventTranslator(0)
    , m_screen(0)
    , m_platformNativeInterface(0)
{
    qDebug() << __PRETTY_FUNCTION__;
    globalEmscriptenIntegration = this;

    m_screen = new QEmscriptenScreen();
    screenAdded(m_screen);

//    m_eventTranslator = new QEmscriptenEventTranslator();
//    QObject::connect(m_eventTranslator, SIGNAL(getWindowAt(QPoint, QWindow **)), this,
//                     SLOT(getWindowAt(QPoint, QWindow **)));
//    QObject::connect(m_eventTranslator, SIGNAL(getKeyWindow(QWindow **)), this,
//                     SLOT(getKeyWindow(QWindow **)));
}

QEmscriptenIntegration::~QEmscriptenIntegration()
{

    qDebug() << __PRETTY_FUNCTION__;
    globalEmscriptenIntegration = 0;
//    delete m_platformNativeInterface;
//    delete m_compositor;
//    delete m_eventTranslator;
//    delete m_fontDatabase;
}

bool QEmscriptenIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    qDebug() << __PRETTY_FUNCTION__;
//    switch (cap) {
//    case ThreadedOpenGL :
//        // Threaded GL is supported when Qt is running on a secondary thread only.
//        return QEmscriptenInstancePrivate::get()->m_runQtOnThread;
//    default:
//        return QPlatformIntegration::hasCapability(cap);
//    }
    return QPlatformIntegration::hasCapability(cap);
}

QPlatformWindow *QEmscriptenIntegration::createPlatformWindow(QWindow *window) const
{
    qDebug() << __PRETTY_FUNCTION__;
//    QEmscriptenWindow *platformWindow = new QEmscriptenWindow(window);
//    if (m_topLevelWindow == 0)
//        m_topLevelWindow = platformWindow;

//    return platformWindow;
    return 0;
}

QPlatformBackingStore *QEmscriptenIntegration::createPlatformBackingStore(QWindow *window) const
{
    qDebug() << __PRETTY_FUNCTION__;
//    QEmscriptenBackingStore *backingStore = new QEmscriptenBackingStore(window);
//    return backingStore;
//    return QPlatformIntegration::createPlatformBackingStore(window);
    return 0;
}

QPlatformOpenGLContext *
QEmscriptenIntegration::createPlatformOpenGLContext(QOpenGLContext *context) const
{
    qDebug() << __PRETTY_FUNCTION__;
//    QEmscriptenGLContext *glContext = new QEmscriptenGLContext();
//    return glContext;
    return QPlatformIntegration::createPlatformOpenGLContext(context);
}

QAbstractEventDispatcher *QEmscriptenIntegration::createEventDispatcher() const
{
    qDebug() << __PRETTY_FUNCTION__;
    m_eventDispatcher = new QEmscriptenEventDispatcher();
    return m_eventDispatcher;
}

QPlatformFontDatabase *QEmscriptenIntegration::fontDatabase() const
{
    qDebug() << __PRETTY_FUNCTION__;
//    if (m_fontDatabase == 0)
//        m_fontDatabase = new QEmscriptenFontDatabase();

//    return m_fontDatabase;
    return QPlatformIntegration::fontDatabase();
}

QPlatformClipboard *QEmscriptenIntegration::clipboard() const
{
    qDebug() << __PRETTY_FUNCTION__;
    //  WIP: disabled.
    //    if (m_clipboard == 0)
    //        m_clipboard = new QEmscriptenClipboard();
    //    return m_clipboard;
    return QPlatformIntegration::clipboard();
}

QPlatformNativeInterface *QEmscriptenIntegration::nativeInterface() const
{
    qDebug() << __PRETTY_FUNCTION__;
//    if (m_platformNativeInterface == 0)
//        m_platformNativeInterface = new QEmscriptenPlatformNativeInterface();

//    return m_platformNativeInterface;
    return QPlatformIntegration::nativeInterface();
}

QPlatformServices *QEmscriptenIntegration::services() const
{
    qDebug() << __PRETTY_FUNCTION__;
//    if (m_services == 0)
//        m_services = new QEmscriptenServices();
//    return m_services;
    return QPlatformIntegration::services();
}

QVariant QEmscriptenIntegration::styleHint(StyleHint hint) const
{
    qDebug() << __PRETTY_FUNCTION__;
    switch (hint) {
    case ShowIsFullScreen:
        return true;
    default:
        return QPlatformIntegration::styleHint(hint);
    }
}

Qt::WindowState QEmscriptenIntegration::defaultWindowState(Qt::WindowFlags) const
{
    qDebug() << __PRETTY_FUNCTION__;
    return Qt::WindowFullScreen;
}

QStringList QEmscriptenIntegration::themeNames() const
{
    qDebug() << __PRETTY_FUNCTION__;
    return QStringList() << QStringLiteral("emscripten");
}

QPlatformTheme *QEmscriptenIntegration::createPlatformTheme(const QString &name) const
{
    qDebug() << __PRETTY_FUNCTION__;
    if (name == QStringLiteral("emscripten"))
        return new QEmscriptenTheme;

    return 0;
}

QEmscriptenCompositor *QEmscriptenIntegration::emscriptenCompositor() const {
    qDebug() << __PRETTY_FUNCTION__;
    return m_compositor;
}

QEmscriptenEventTranslator *QEmscriptenIntegration::emscriptenEventTranslator() const
{
    qDebug() << __PRETTY_FUNCTION__;
    return m_eventTranslator;
}

void QEmscriptenIntegration::processEvents() {
    qDebug() << __PRETTY_FUNCTION__;
//    m_eventDispatcher->processEvents();
}

void QEmscriptenIntegration::resizeScreen(QSize size, qreal devicePixelRatio)
{
    qDebug() << __PRETTY_FUNCTION__;
    // Set the frame buffer on the compositor
//    if (m_compositor)
//        m_compositor->beginResize(size, devicePixelRatio);

//    // Send the screen geometry change to Qt, resize windows.
//    QRect screenRect(QPoint(0, 0), size);
//    m_screen->resizeMaximizedWindows();
//    QWindowSystemInterface::handleScreenGeometryChange(m_screen->screen(),
//                                                       screenRect,  // new geometry
//                                                       screenRect); // new available geometry
//    QWindowSystemInterface::flushWindowSystemEvents();

//    // Let Qt process the resize events;
//    if (m_eventDispatcher)
//        m_eventDispatcher->processEvents();

//    // End resize and composit.
//    if (m_compositor)
//        m_compositor->endResize();
}

QEmscriptenWindow *QEmscriptenIntegration::topLevelWindow() const {
    qDebug() << __PRETTY_FUNCTION__;
    return m_topLevelWindow;
}

void QEmscriptenIntegration::getWindowAt(const QPoint &point, QWindow **window)
{
    qDebug() << __PRETTY_FUNCTION__;
//    if (m_compositor)
//        *window = m_compositor->windowAt(point);
//    else if (m_topLevelWindow)
//        *window = m_topLevelWindow->window();
//    else
//        *window = 0;
}

void QEmscriptenIntegration::getKeyWindow(QWindow **window)
{
    qDebug() << __PRETTY_FUNCTION__;
//    if (m_compositor)
//        *window = m_compositor->keyWindow();
//    else if (m_topLevelWindow)
//        *window = m_topLevelWindow->window();
//    else
//        *window = 0;
}
