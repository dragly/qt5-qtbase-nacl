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

#ifndef QEMSCRIPTENEVENTDISPATCHER_H
#define QEMSCRIPTENEVENTDISPATCHER_H

#include <QtCore/QHash>
#include <QtCore/QLoggingCategory>
#include "../platformsupport/eventdispatchers/qunixeventdispatcher_qpa_p.h"

//#include <ppapi/cpp/message_loop.h>
//#include <ppapi/utility/completion_callback_factory.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_PLATFORM_EMSCRIPTEN_EVENTDISPATHCER)

class QEmscriptenEventDispatcher : public QUnixEventDispatcherQPA
{
public:
    explicit QEmscriptenEventDispatcher(QObject *parent = 0);
    ~QEmscriptenEventDispatcher();

    bool processEvents(QEventLoop::ProcessEventsFlags flags
                       = QEventLoop::EventLoopExec) Q_DECL_OVERRIDE;
    bool hasPendingEvents() Q_DECL_OVERRIDE;

    void registerTimer(int timerId, int interval, Qt::TimerType timerType,
                       QObject *object) Q_DECL_OVERRIDE;
    bool unregisterTimer(int timerId) Q_DECL_OVERRIDE;
    bool unregisterTimers(QObject *object) Q_DECL_OVERRIDE;

    void flush() Q_DECL_OVERRIDE;
    void wakeUp() Q_DECL_OVERRIDE;

private:
    struct EmscriptenTimerInfo {

        EmscriptenTimerInfo(){}
        EmscriptenTimerInfo(int timerId, int interval, Qt::TimerType timerType, QObject *object)
            : timerId(timerId)
            , interval(interval)
            , timerType(timerType)
            , object(object)
        {
        }
        int timerId;
        int interval;
        Qt::TimerType timerType;
        QObject *object;
    };

    void startTimer(EmscriptenTimerInfo info);
    void timerCallback(int32_t result, int32_t timerSerial);
    void scheduleProcessEvents();
    void processEventsCallback(int32_t status);

    int m_currentTimerSerial;
    QHash<int, int> m_activeTimerIds;                // timer serial -> Qt timer id
    QHash<int, int> m_activeTimerSerials;            // Qt timer id -> timer serial
    QMultiHash<QObject *, int> m_activeObjectTimers; // QObject * -> Qt timer id
    QHash<int, EmscriptenTimerInfo> m_timerDetails;
//    pp::MessageLoop m_messageLoop;
//    pp::CompletionCallbackFactory<QEmscriptenEventDispatcher> m_completionCallbackFactory;
};

QT_END_NAMESPACE

#endif
