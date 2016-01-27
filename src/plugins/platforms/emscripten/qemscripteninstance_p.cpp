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

#include "qemscripteninstance_p.h"

#include "qemscriptenhelpers.h"
#include "qemscripteninstance.h"
#include "qemscriptenintegration.h"
#include "qemscriptenmodule_p.h"
#include "qemscriptenwindow.h"

#include <qpa/qwindowsysteminterface.h>

//#include <ppapi/cpp/completion_callback.h>
//#include <ppapi/cpp/instance.h>
//#include <ppapi/cpp/var.h>
//#include <ppapi/utility/threading/simple_thread.h>

//#ifdef Q_OS_NACL_NEWLIB
//#include <error_handling/error_handling.h>
//#endif

QObject *qtScriptableObject;

Q_LOGGING_CATEGORY(QT_PLATFORM_EMSCRIPTEN_INSTANCE, "qt.platform.emscripten.instance")

QEmscriptenInstancePrivate *qtCreateEmscriptenInstancePrivate(QEmscriptenInstance *instance)
{
    qDebug() << __PRETTY_FUNCTION__;
    return new QEmscriptenInstancePrivate(instance);
}

static QEmscriptenInstancePrivate *g_emscriptenInstancePrivate = 0;
extern void *qtEmscriptenInstance; // QtCore

extern bool qGuiHaveBlockingMain();
extern int qGuiCallBlockingMain();
extern bool qGuiStartup();

QEmscriptenInstancePrivate::QEmscriptenInstancePrivate(QEmscriptenInstance *instance)
//    : m_qtThread(instance)
{
    qDebug() << __PRETTY_FUNCTION__;
    q = instance;

    qtEmscriptenInstance = instance;
    g_emscriptenInstancePrivate = this;

    m_qtStarted = false;
    m_runQtOnThread = false;
    m_useQtMessageLoop = false;

#ifdef QT_EMSCRIPTEN_RUN_QT_ON_THREAD
    m_runQtOnThread = true;
#else
    if (qGuiHaveBlockingMain() || !qgetenv("QT_EMSCRIPTEN_RUN_QT_ON_THREAD").isEmpty())
        m_runQtOnThread = true;
#endif

#ifdef QT_EMSCRIPTEN_USE_QT_MESSAGE_LOOP
    m_useQtMessageLoop = true;
#else
    if (!qgetenv("QT_EMSCRIPTEN_USE_QT_MESSAGE_LOOP").isEmpty())
        m_useQtMessageLoop = true;
#endif

//    m_currentGeometry = pp::Rect();
//    m_callbackFactory.Initialize(this);

    if (m_runQtOnThread) {
        if (m_useQtMessageLoop) {
            void *threadFunction(void *context);
            pthread_create(&m_qtThread_pthread, NULL, threadFunction, this);
        } else {
//            m_qtThread.Start();
//            m_qtMessageLoop = m_qtThread.message_loop();
        }
    } else {
//        m_qtMessageLoop = pp::MessageLoop::GetForMainThread();
    }
}

QEmscriptenInstancePrivate::~QEmscriptenInstancePrivate()
{
    qDebug() << __PRETTY_FUNCTION__;
    if (m_useQtMessageLoop) {
        if (m_useQtMessageLoop) {
            pthread_join(m_qtThread_pthread, NULL);
        } else {
//            m_qtMessageLoop.PostQuit(true); // quit and detach from thread
//            m_qtThread.Join();
        }
    }
}

QEmscriptenInstancePrivate *QEmscriptenInstancePrivate::get() {
    qDebug() << __PRETTY_FUNCTION__;
    return g_emscriptenInstancePrivate;
}

//pp::Instance *QEmscriptenInstancePrivate::getPPInstance() { return g_emscriptenInstancePrivate->q; }

//void QEmscriptenInstancePrivate::processCall(pp::CompletionCallback call)
//{
//    // Run call according to threading mode:
//    if (m_useQtMessageLoop)
//        m_qtMessageLoop_pthread.enqueue(call);
//    else
//        m_qtMessageLoop.PostWork(call);
//}

//void QEmscriptenInstancePrivate::processMessageLoopMessage()
//{
//    pp::CompletionCallback message
//        = m_qtMessageLoop_pthread.dequeue(); // waits for the next message
//    if (message.pp_completion_callback().user_data != 0) {
//        int resultNotUsed = 0;
//        message.Run(resultNotUsed);
//    }
//}

void *threadFunction(void *context)
{
    qDebug() << __PRETTY_FUNCTION__;
    reinterpret_cast<QEmscriptenInstancePrivate *>(context)->qtMessageLoop();
    return 0;
}

void QEmscriptenInstancePrivate::qtMessageLoop()
{
    qDebug() << __PRETTY_FUNCTION__;
    // Attempt PPAPI compatibility: We can attach a message loop,
    // but not run it (we are running the loop below instead).
    // According to the pp::MessageLoop documentation this will
    // allow making blocking PPAPI calls on the Qt main thread
    // (this thread).
//    m_qtMessageLoop.AttachToCurrentThread();

    while (!m_qtMessageLoop_pthread.testQuit())
        processMessageLoopMessage();
}

#ifdef Q_OS_NACL_NEWLIB
void qtExceptionHandler(const char *json)
{
    qDebug() << __PRETTY_FUNCTION__;
    qDebug() << "CRASH";
    qDebug() << json;
}
#endif

// Message handler for redirecting Qt debug output to the web page.
void qtMessageHandler(QtMsgType, const QMessageLogContext &context, const QString &message)
{
    qDebug() << __PRETTY_FUNCTION__;
//    QByteArray bytes = context.category + message.toLatin1() + "\n";
//    QEmscriptenInstancePrivate::getPPInstance()->PostMessage(pp::Var(bytes.constData()));
}

// There is one global app pp::Instance. It corresponds to the
// html div tag that contains the app.
bool QEmscriptenInstancePrivate::init(int32_t result, uint32_t argc, const QVector<QByteArray> &vargn,
                                  const QVector<QByteArray> &vargv)
{
    qDebug() << __PRETTY_FUNCTION__;
    Q_UNUSED(result);

    qCDebug(QT_PLATFORM_EMSCRIPTEN_INSTANCE) << "Init argc:" << argc;

#ifdef Q_OS_NACL_NEWLIB
// Comment in next line to enable the exception/crash handler.
// EHRequestExceptionsJson(qtExceptionHandler);
#endif

    if (false) {
        // Optinally redirect debug and logging output to the web page following the conventions
        // used in the nacl_sdk examples: presence of 'ps_stdout="/dev/tty"' attributes
        // signales that the web page can handle debug output. This has a couple of drawbacks:
        // 1) debug utput sent before installing the message handler still goes to Chrome
        //    standard output.
        // 2) It reserves the postMessage functionality for debug output, this may conflict
        //    with other use cases.
        // 3) It's really slow.
        for (unsigned int i = 0; i < argc; ++i) {
            if (qstrcmp(vargn[i], "ps_stdout") && qstrcmp(vargv[i], "/dev/tty"))
                qInstallMessageHandler(qtMessageHandler);
        }
    }

    // Place argument key/value pairs in the process environment. (NaCl/emscripten does
    // not support environment variables, Qt emulates via qputenv and getenv.)
    for (unsigned int i = 0; i < argc; ++i) {
        QByteArray name = QByteArray(vargn[i]).toUpper(); // html forces lowercase.
        QByteArray value = vargv[i];
        qputenv(name.constData(), value);
        qCDebug(QT_PLATFORM_EMSCRIPTEN_INSTANCE) << "setting environment variable" << name << "="
                                             << value;
    }

    // Enable input event types Qt is interested in. Filtering input events
    // are special: Qt can reject those and the event will bubble to the rest
    // of the page.
//    q->RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE | PP_INPUTEVENT_CLASS_WHEEL);
//    q->RequestFilteringInputEvents(PP_INPUTEVENT_CLASS_KEYBOARD);

    return true;
}

// DidchangeView is called on div tag geometry or configuration change, for
// example if the pixel scaling changes. This is also where Qt is started.
//void QEmscriptenInstancePrivate::didChangeView(int32_t result, const pp::View &view)
//{
//    Q_UNUSED(result);
//    pp::Rect geometry = view.GetRect();
//    // Compute the effective devicePixelRatio. DeviceScale is related
//    // to the hardware and is often an integer. CSSScale is the is the
//    // zoom factor and is often a real number.
//    // NOTE: This leads to non-integer devicePixelRatios. We might want
//    // add an option to not factor in CSSScale.
//    qreal deviceScale = view.GetDeviceScale();
//    qreal cssScale = view.GetCSSScale();
//    qreal devicePixelRatio = deviceScale * cssScale;

//    qCDebug(QT_PLATFORM_EMSCRIPTEN_INSTANCE) << "DidChangeView" << toQRect(geometry) << "DeviceScale"
//                                         << deviceScale << "CSSScale" << cssScale
//                                         << "devicePixelRatio" << devicePixelRatio;

//    if (geometry.size() == m_currentGeometry.size()
//        && devicePixelRatio == m_currentDevicePixelRatio)
//        return;

//    m_currentGeometry = geometry;
//    m_currentDeviceScale = deviceScale;
//    m_currentCssScale = cssScale;
//    m_currentDevicePixelRatio = devicePixelRatio;

//    // Start Qt on the first DidChangeView. This means we have a "screen" while
//    // the application creates the root window etc, which makes life easier.
//    if (!m_qtStarted) {
//        m_qtStarted = true;
//        startQt();
//    } else {
//        if (QEmscriptenIntegration *emscriptenIntegraion = QEmscriptenIntegration::get())
//            emscriptenIntegraion->resizeScreen(this->geometry().size(), m_currentDevicePixelRatio);
//    }

//    if (QEmscriptenIntegration *emscriptenIntegraion = QEmscriptenIntegration::get())
//        emscriptenIntegraion->processEvents();
//}

//void QEmscriptenInstancePrivate::didChangeFocus(int32_t result, bool hasFucus)
//{
//    Q_UNUSED(result);
//    qCDebug(QT_PLATFORM_EMSCRIPTEN_INSTANCE) << "DidChangeFocus" << hasFucus;

//    QEmscriptenIntegration *emscriptenIntegration = QEmscriptenIntegration::get();
//    if (!emscriptenIntegration)
//        return;

//    QWindow *fucusWindow = hasFucus ? emscriptenIntegration->topLevelWindow()->window() : 0;
//    QWindowSystemInterface::handleWindowActivated(fucusWindow, Qt::ActiveWindowFocusReason);
//}

//bool QEmscriptenInstancePrivate::handleInputEvent(int32_t result, const pp::InputEvent &event)
//{
//    Q_UNUSED(result);
//    // this one is spammy (mouse moves)
//    // qCDebug(QT_PLATFORM_EMSCRIPTEN_INSTANCE) << "HandleInputEvent";

//    QEmscriptenIntegration *emscriptenIntegration = QEmscriptenIntegration::get();
//    if (!emscriptenIntegration)
//        return false;

//    bool ret = emscriptenIntegration->emscriptenEventTranslator()->processEvent(event);
//    emscriptenIntegration->processEvents();
//    return ret;
//}

//bool QEmscriptenInstancePrivate::handleDocumentLoad(int32_t result, const pp::URLLoader &url_loader)
//{
//    Q_UNUSED(result);
//    Q_UNUSED(url_loader);
//    return false;
//}

//void QEmscriptenInstancePrivate::handleMessage(int32_t result, const pp::Var &var_message)
//{
//    Q_UNUSED(result);

//    // Expect messages formatted like "tag:message". Dispatch the
//    // message to the handler registered for "tag".
//    QByteArray q_message = toQByteArray(var_message);
//    int split = q_message.indexOf(':');
//    if (split == -1)
//        return;
//    QByteArray tag = q_message.mid(0, split);
//    QByteArray message = q_message.mid(split + 1);
//    if (m_messageHandlers.contains(tag)) {
//        QPair<QPointer<QObject>, const char *> handler = m_messageHandlers[tag];
//        if (!handler.first.isNull())
//            QMetaObject::invokeMethod(handler.first.data(), handler.second,
//                                      Q_ARG(QByteArray, message));
//    }
//}

void QEmscriptenInstancePrivate::registerMessageHandler(const QByteArray &messageTag, QObject *obj,
                                                    const char *slot)
{
    qDebug() << __PRETTY_FUNCTION__;
    m_messageHandlers[messageTag] = qMakePair(QPointer<QObject>(obj), slot);
}

QRect QEmscriptenInstancePrivate::geometry()
{
    qDebug() << __PRETTY_FUNCTION__;
//    QRect r = toQRect(m_currentGeometry);
//    return QRect(r.topLeft(), r.size() / m_currentCssScale);
}

QRect QEmscriptenInstancePrivate::deviceGeometry()
{
    qDebug() << __PRETTY_FUNCTION__;
//    QRect r = toQRect(m_currentGeometry);
//    return QRect(r.topLeft(), r.size() * m_currentDeviceScale);
}

qreal QEmscriptenInstancePrivate::devicePixelRatio() {
    qDebug() << __PRETTY_FUNCTION__; return m_currentDevicePixelRatio; }

qreal QEmscriptenInstancePrivate::cssScale() {
    qDebug() << __PRETTY_FUNCTION__; return m_currentCssScale; }

// From time to time it's useful to flush and empty the posted
// window system events queue. However, Qt and Qt apps does not
// expect this type of event processing happening at arbitarty times
// (for example in the middle of setGeometry call). This function
// schedules a flushWindowSystemEvents() call for when Qt returns
// to the Emscripten event loop.
void QEmscriptenInstancePrivate::scheduleWindowSystemEventsFlush()
{
    qDebug() << __PRETTY_FUNCTION__;
//    m_qtMessageLoop.PostWork(
//        m_callbackFactory.NewCallback(&QEmscriptenInstancePrivate::windowSystemEventsFlushCallback));
}

void QEmscriptenInstancePrivate::windowSystemEventsFlushCallback(int32_t)
{
    qDebug() << __PRETTY_FUNCTION__;
    QWindowSystemInterface::flushWindowSystemEvents();
}

void QEmscriptenInstancePrivate::postMessage(const QByteArray &message)
{
    qDebug() << __PRETTY_FUNCTION__;
    qCDebug(QT_PLATFORM_EMSCRIPTEN_INSTANCE) << "postMessage" << message;
//    q->PostMessage(toPPVar(message));
}

// Runs the given script on the containing web page, using
// the standard Qt message handler provided by nacldeployqt.
// This function is designed to run "safe" javascript only:
// running javascript witch contains for example user-provided
// strings may open up for javascript injection attacks.
void QEmscriptenInstancePrivate::runJavascript(const QByteArray &script)
{
    qDebug() << __PRETTY_FUNCTION__;
    qCDebug(QT_PLATFORM_EMSCRIPTEN_INSTANCE) << "runJavascript" << script;
    postMessage("qtEval:" + script);
}

// Start Qt. Handle the different Qt startup patterns:
// - Q_GUI_MAIN
// - Q_GUI_BLOCKING_MAIN
// - QEmscriptenInstance subclass
//
void QEmscriptenInstancePrivate::startQt()
{
    qDebug() << __PRETTY_FUNCTION__;
    qDebug() << __PRETTY_FUNCTION__;
#ifdef Q_OS_NACL_NEWLIB
    // Make sure the fonts resource is included for static builds.
    Q_INIT_RESOURCE(naclfonts);
#endif

    // There's a "blocking main" registered, call it. The function
    // will then create the QGuiApplicaiton object, run the application
    // and return at app exit.
    if (qGuiHaveBlockingMain()) {
        qGuiCallBlockingMain();
        return;
    }

    // Call qGuiStartup which will create the QGuiApplicaiton object and
    // the platform plugin.
    qGuiStartup();

    // Call application init code. The default QEmscriptenInstance
    // implementation calls the app init function registered
    // with QT_GUI_MAIN. QEmscriptenInstance sublcasses may override
    // this with custom app initialization code.
//    q->applicationInit();
}

void QEmscriptenInstancePrivate::flushCompletedCallback(int32_t) {
    qDebug() << __PRETTY_FUNCTION__;}
