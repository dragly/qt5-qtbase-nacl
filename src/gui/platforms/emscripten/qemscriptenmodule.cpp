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

#include "qemscriptenmodule.h"
#include "qemscriptenmodule_p.h"
#include "qemscripteninstance.h"

extern QEmscriptenModulePrivate *qtCreateEmscriptenModulePrivate(QEmscriptenModule *module);

QEmscriptenModule::QEmscriptenModule()
:d(qtCreateEmscriptenModulePrivate(this))
{
    
}

QEmscriptenModule::~QEmscriptenModule()
{
    delete d;
}

//bool QEmscriptenModule::Init()
//{
//    return d->init();
//}

// pp::Instance* QEmscriptenModule::CreateInstance(PP_Instance ppInstance)
// {
//     return d->createInstance(ppInstance);
// }

// Helper function for Q_GUI_MAIN: allows creating a QEmscriptenModule
// without pulling in the ppapi headers.
// pp::Module *qtGuiCreateEmscriptenModule()
// {
//     return new QEmscriptenModule();
// }
