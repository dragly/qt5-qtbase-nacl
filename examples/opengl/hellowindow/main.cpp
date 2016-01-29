/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "hellowindow.h"

#include <qpa/qplatformintegration.h>

#include <QGuiApplication>
#include <QScreen>
#include <QThread>
#include <QtGui>

void app_init(int argc, char *argv[])
{
//    QGuiApplication app(argc, argv);
    // Some platforms can only have one window per screen. Therefore we need to differentiate.
    const bool multipleWindows = QGuiApplication::arguments().contains(QStringLiteral("--multiple"));
    const bool multipleScreens = QGuiApplication::arguments().contains(QStringLiteral("--multiscreen"));

    QScreen *screen = QGuiApplication::primaryScreen();

    QRect screenGeometry = screen->availableGeometry();

    QSurfaceFormat format;
    format.setDepthBufferSize(16);
    if (QGuiApplication::arguments().contains(QStringLiteral("--multisample")))
        format.setSamples(4);

    QPoint center = QPoint(screenGeometry.center().x(), screenGeometry.top() + 80);
    QSize windowSize(400, 320);
    int delta = 40;

    QSharedPointer<Renderer> rendererA(new Renderer(format));

    HelloWindow *windowA = new HelloWindow(rendererA);
    windowA->setGeometry(QRect(center, windowSize).translated(-windowSize.width() - delta / 2, 0));
    windowA->setTitle(QStringLiteral("Thread A - Context A"));
    windowA->setVisible(true);
    windowA->showFullScreen();
}

void app_exit() {}

#ifdef Q_OS_NACL
Q_GUI_MAIN(app_init, app_exit)
#else
int main(int argc, char **argv)
{
    qDebug() << "App";
    QGuiApplication app(argc, argv);
    qDebug() << "Init";
    app_init(argc, argv);
    qDebug() << "exec";
    app.exec();
    qDebug() << "exit";
    app_exit();
    return 0;
}
#endif
