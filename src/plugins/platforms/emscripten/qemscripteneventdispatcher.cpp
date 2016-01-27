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

#include "qemscripteneventdispatcher.h"

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

Q_LOGGING_CATEGORY(QT_PLATFORM_EMSCRIPTEN_EVENTDISPATHCER, "qt.platform.emscripten.eventdispatcher")

QEmscriptenEventDispatcher::QEmscriptenEventDispatcher(QObject *parent)
    : QUnixEventDispatcherQPA(parent)
    , m_currentTimerSerial(0)
//    , m_messageLoop(pp::MessageLoop::GetCurrent())
//    , m_completionCallbackFactory(this)
{
    qDebug() << __PRETTY_FUNCTION__;
    qCDebug(QT_PLATFORM_EMSCRIPTEN_EVENTDISPATHCER) << "QEmscriptenEventDispatcher()";
}

QEmscriptenEventDispatcher::~QEmscriptenEventDispatcher() {
    qDebug() << __PRETTY_FUNCTION__;
}

bool QEmscriptenEventDispatcher::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    qDebug() << __PRETTY_FUNCTION__;
    do {
        QUnixEventDispatcherQPA::processEvents(flags);
    } while (hasPendingEvents());
    return true;
}

bool QEmscriptenEventDispatcher::hasPendingEvents()
{
    qDebug() << __PRETTY_FUNCTION__;
    return QUnixEventDispatcherQPA::hasPendingEvents();
}

void QEmscriptenEventDispatcher::registerTimer(int timerId, int interval, Qt::TimerType timerType,
                                           QObject *object)
{
    qDebug() << __PRETTY_FUNCTION__;
    qCDebug(QT_PLATFORM_EMSCRIPTEN_EVENTDISPATHCER) << "QEmscriptenEventDispatcher::registerTimer"
                                                << timerId << interval << object;

    // Maintain QObject *-> Qt timer id mapping.
    m_activeObjectTimers.insertMulti(object, timerId);

    // Capture timer detail for later use
    EmscriptenTimerInfo timerInfo(timerId, interval, timerType, object);
    m_timerDetails.insert(timerId, timerInfo);

    startTimer(timerInfo);
}

bool QEmscriptenEventDispatcher::unregisterTimer(int timerId)
{
    qDebug() << __PRETTY_FUNCTION__;
    qCDebug(QT_PLATFORM_EMSCRIPTEN_EVENTDISPATHCER) << "QEmscriptenEventDispatcher::unregisterTimer"
                                                << timerId;

    // Remove QObject -> Qt timer id mapping.
    QObject *timerObject = m_timerDetails.value(timerId).object;
    m_activeObjectTimers.remove(timerObject, timerId);

    m_timerDetails.remove(timerId);

    // Remove the timerId and the currently active serial.
    int timerSerial = m_activeTimerSerials.value(timerId);
    m_activeTimerSerials.remove(timerId);
    m_activeTimerIds.remove(timerSerial);

    return true;
}

bool QEmscriptenEventDispatcher::unregisterTimers(QObject *object)
{
    qDebug() << __PRETTY_FUNCTION__;
    qCDebug(QT_PLATFORM_EMSCRIPTEN_EVENTDISPATHCER) << "QEmscriptenEventDispatcher::unregisterTimers"
                                                << object;

    // Find all active Qt timer ids for the given object and copy them to a list.
    // (we will update the QObject * -> timerId hash while iterating the list later on)
    QList<int> timerIds;
    QMultiHash<QObject *, int>::iterator it = m_activeObjectTimers.find(object);
    while (it != m_activeObjectTimers.end() && it.key() == object)
        timerIds.append(it.value());

    // Unregister each timer.
    foreach (int timerId, timerIds)
        unregisterTimer(timerId);

    return true;
}

void QEmscriptenEventDispatcher::flush()
{
    qDebug() << __PRETTY_FUNCTION__;
    qCDebug(QT_PLATFORM_EMSCRIPTEN_EVENTDISPATHCER) << "QEmscriptenEventDispatcher::flush";

    QUnixEventDispatcherQPA::flush();
}

void QEmscriptenEventDispatcher::wakeUp()
{
    qDebug() << __PRETTY_FUNCTION__;
    qCDebug(QT_PLATFORM_EMSCRIPTEN_EVENTDISPATHCER) << "QEmscriptenEventDispatcher::wakeup";
    scheduleProcessEvents();
}

void QEmscriptenEventDispatcher::startTimer(EmscriptenTimerInfo info)
{
    qDebug() << __PRETTY_FUNCTION__;
    qCDebug(QT_PLATFORM_EMSCRIPTEN_EVENTDISPATHCER) << "startTimer" << info.timerId;

    // Allocate a timer serial value for this startTimer call.
    //
    // Qt reuses timer id's. This means there is time for Qt to unregister a
    // timer and register a new timer with the same id before our timer
    // callback is called.
    //
    // This is compounded by single-shot timer behavior: We are not informed
    // that a given timer is of the single-shot variant. Instead Qt will
    // unregister it during the timer fire event.
    //
    // This is further compounded by the fact that the Emscripten timer API is of
    // the single-shot variant. After sending the timer event to Qt a decision
    // has to be made if the timer should be rescheduled or not.
    //
    // In summary: To avoid confusing separate uses of the same timer id we
    // give each call to CallOnMainThread a serial number which we can
    // independently cancel.
    //
    ++m_currentTimerSerial;
    m_activeTimerIds[m_currentTimerSerial] = info.timerId;
    m_activeTimerSerials[info.timerId] = m_currentTimerSerial;

//    m_messageLoop.PostWork(m_completionCallbackFactory.NewCallback(
//                               &QEmscriptenEventDispatcher::timerCallback, m_currentTimerSerial),
//                           info.interval);
}

void QEmscriptenEventDispatcher::timerCallback(int32_t result, int32_t timerSerial)
{
    qDebug() << __PRETTY_FUNCTION__;
    Q_UNUSED(result);
    qCDebug(QT_PLATFORM_EMSCRIPTEN_EVENTDISPATHCER) << "timerCallback" << timerSerial;

    // The timer might have been unregistered. In that case don't fire.
    if (!m_activeTimerIds.contains(timerSerial))
        return;

    // Get the timer info for the timerSerial/timerID.
    int timerId = m_activeTimerIds.value(timerSerial);
    const EmscriptenTimerInfo &info = m_timerDetails.value(timerId);

    // Send the timer event
    QTimerEvent e(info.timerId);
    QCoreApplication::sendEvent(info.object, &e);
    processEvents();

    // After running Qt and application code the timer may have been unregistered,
    // and the timer id may have been reused. The timerSerial will hower not
    // be reused; use that to determine if the timer is active.
    if (m_activeTimerIds.contains(timerSerial))
        startTimer(info);

    // one serial number per callback, we are done with this one.
    m_activeTimerIds.remove(timerSerial);
}

void QEmscriptenEventDispatcher::scheduleProcessEvents()
{
    qDebug() << __PRETTY_FUNCTION__;
    qCDebug(QT_PLATFORM_EMSCRIPTEN_EVENTDISPATHCER) << "scheduleProcessEvents";
//    pp::CompletionCallback processEvents
//        = m_completionCallbackFactory.NewCallback(&QEmscriptenEventDispatcher::processEventsCallback);
//    int32_t result = m_messageLoop.PostWork(processEvents);
//    if (result != PP_OK)
//        qCDebug(QT_PLATFORM_EMSCRIPTEN_EVENTDISPATHCER) << "scheduleProcessEvents PostWork error"
//                                                    << result;
}

void QEmscriptenEventDispatcher::processEventsCallback(int32_t status)
{
    qDebug() << __PRETTY_FUNCTION__;
    Q_UNUSED(status);
    qCDebug(QT_PLATFORM_EMSCRIPTEN_EVENTDISPATHCER) << "processEvents";

    processEvents();
}
