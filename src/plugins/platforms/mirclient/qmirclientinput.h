/****************************************************************************
**
** Copyright (C) 2014-2015 Canonical, Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QMIRCLIENTINPUT_H
#define QMIRCLIENTINPUT_H

// Qt
#include <qpa/qwindowsysteminterface.h>

#include <mir_toolkit/mir_client_library.h>

class QMirClientClientIntegration;
class QMirClientWindow;

class QMirClientInput : public QObject
{
    Q_OBJECT

public:
    QMirClientInput(QMirClientClientIntegration* integration);
    virtual ~QMirClientInput();

    // QObject methods.
    void customEvent(QEvent* event) override;

    void postEvent(QMirClientWindow* window, const MirEvent *event);
    QMirClientClientIntegration* integration() const { return mIntegration; }

protected:
    void dispatchKeyEvent(QWindow *window, const MirInputEvent *event);
    void dispatchPointerEvent(QWindow *window, const MirInputEvent *event);
    void dispatchTouchEvent(QWindow *window, const MirInputEvent *event);
    void dispatchInputEvent(QWindow *window, const MirInputEvent *event);

    void dispatchOrientationEvent(QWindow* window, const MirOrientationEvent *event);

private:
    QMirClientClientIntegration* mIntegration;
    QTouchDevice* mTouchDevice;
    const QByteArray mEventFilterType;
    const QEvent::Type mEventType;
};

#endif // QMIRCLIENTINPUT_H
