/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com <http://qt.digia.com/>
**
** This file is part of the Qt Native Client platform plugin.
**
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.digia.com <http://qt.digia.com/>
**
****************************************************************************/

#include "qemscripteninstance.h"
//#include "qemscripteninstance_p.h"

extern QEmscriptenInstancePrivate *qtCreateEmscriptenInstancePrivate(QEmscriptenInstance *instance);

// QEmscriptenInstance::QEmscriptenInstance(PP_Instance instance)
// : pp::Instance(instance)
// , d(qtCreateEmscriptenInstancePrivate(this))
// {
// 
// }

// bool QEmscriptenInstance::Init(uint32_t argc, const char* argn[], const char* argv[])
// {
//     // argn/argv will not be valid by the time the message loop processes
//     // the message posted below. Make a copy.
//     QVector<QByteArray> vargn;
//     QVector<QByteArray> vargv;
//     for (uint32_t i = 0; i < argc; ++i) {
//         vargn.push_back(argn[i]);
//         vargv.push_back(argv[i]);
//     }
// 
//     if (d->m_runQtOnThread) {
//         d->processCall(d->m_callbackFactory.NewCallback(
//                 &QEmscriptenInstancePrivate::init, argc, vargn, vargv));
//     } else {
//         uint32_t unused = 0;
//         d->init(unused, argc, vargn, vargv);
//     }
// 
//     return true;
// }
// 
// void QEmscriptenInstance::DidChangeView(const pp::View &view)
// {
//     if (d->m_runQtOnThread) {
//         d->processCall(d->m_callbackFactory.NewCallback(
//             &QEmscriptenInstancePrivate::didChangeView, view));
//     } else {
//         uint32_t unused = 0;
//         d->didChangeView(unused, view);
//     }
// }
// 
// void QEmscriptenInstance::DidChangeFocus(bool hasFucus)
// {
//     if (d->m_runQtOnThread) {
//         d->processCall(d->m_callbackFactory.NewCallback(
//             &QEmscriptenInstancePrivate::didChangeFocus, hasFucus));
//     } else {
//         uint32_t unused = 0;
//         d->didChangeFocus(unused, hasFucus);
//     }
// }
// 
// bool QEmscriptenInstance::HandleInputEvent(const pp::InputEvent& event)
// {
//     if (d->m_runQtOnThread) {
//         d->processCall(d->m_callbackFactory.NewCallback(
//             &QEmscriptenInstancePrivate::handleInputEvent, event));
//         return true; // FIXME: Get result from async call above.
//     } else {
//         uint32_t unused = 0;
//         return d->handleInputEvent(unused, event);
//     }
// }
// 
// void QEmscriptenInstance::HandleMessage(const pp::Var& message)
// {
//     if (d->m_runQtOnThread) {
//         d->processCall(d->m_callbackFactory.NewCallback(
//             &QEmscriptenInstancePrivate::handleMessage, message));
//     } else {
//         uint32_t unused = 0;
//         d->handleMessage(unused, message);
//     }
// }
// 
// void QEmscriptenInstance::applicationInit()
// {
//     // The default applicationInit() implementation calls the app_init
//     // function registered with QT_GUI_MAIN.
//     extern void qGuiAppInit();
//     qGuiAppInit();
// }
