/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
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
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPLUGIN_H
#define QPLUGIN_H

#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>
#include <QtCore/qjsonobject.h>

QT_BEGIN_NAMESPACE

#ifndef Q_EXTERN_C
#  ifdef __cplusplus
#    define Q_EXTERN_C extern "C"
#  else
#    define Q_EXTERN_C extern
#  endif
#endif

typedef QObject *(*QtPluginInstanceFunction)();
typedef const char *(*QtPluginMetaDataFunction)();

struct Q_CORE_EXPORT QStaticPlugin
{
    // Note: This struct is initialized using an initializer list.
    // As such, it cannot have any new constructors or variables.
#ifndef Q_QDOC
    QtPluginInstanceFunction instance;
    QtPluginMetaDataFunction rawMetaData;
#else
    // Since qdoc gets confused by the use of function
    // pointers, we add these dummes for it to parse instead:
    QObject *instance();
    const char *rawMetaData();
#endif
    QJsonObject metaData() const;
};
Q_DECLARE_TYPEINFO(QStaticPlugin, Q_PRIMITIVE_TYPE);

void Q_CORE_EXPORT qRegisterStaticPluginFunction(QStaticPlugin staticPlugin);

#if defined(Q_OS_NACL_EMSCRIPTEN)
// It is important that the metadata is aligned in Emscripten because we are
// not allowed to read unaligned data in JS/Emscripten.
// Without this, importing static plugins will randomly fail because reading
// the offset 'size' value in QLibraryPrivate::fromRawMetaData is undefined behavior.
// For this purpose, aligning to 32 bits would suffice.
// Using 64 bits is probably overkill, but should ensure that this problem doesn't
// surface in other parts of the code.
#  define QT_PLUGIN_METADATA_SECTION \
    __attribute__ ((section (".qtmetadata"))) __attribute__((used)) __attribute__((aligned(64)))
#elif defined(Q_OS_PNACL)
// PNaCl does not support "section":
// "Variable _ZL17qt_pluginMetaData has disallowed "section" attribute"
// PNaCl is Q_CC_CLANG. TODO: should it not set Q_OF_ELF?
#  define QT_PLUGIN_VERIFICATION_SECTION
#  define QT_PLUGIN_METADATA_SECTION
#elif (defined(Q_OF_ELF) || defined(Q_OS_WIN)) && (defined (Q_CC_GNU) || defined(Q_CC_CLANG))
#  define QT_PLUGIN_METADATA_SECTION \
    __attribute__ ((section (".qtmetadata"))) __attribute__((used))
#elif defined(Q_OS_MAC)
// TODO: Implement section parsing on Mac
#  define QT_PLUGIN_METADATA_SECTION \
    __attribute__ ((section ("__TEXT,qtmetadata"))) __attribute__((used))
#elif defined(Q_CC_MSVC)
// TODO: Implement section parsing for MSVC
#pragma section(".qtmetadata",read,shared)
#  define QT_PLUGIN_METADATA_SECTION \
    __declspec(allocate(".qtmetadata"))
#else
#  define QT_PLUGIN_VERIFICATION_SECTION
#  define QT_PLUGIN_METADATA_SECTION
#endif


#define Q_IMPORT_PLUGIN(PLUGIN) \
        extern const QT_PREPEND_NAMESPACE(QStaticPlugin) qt_static_plugin_##PLUGIN(); \
        class Static##PLUGIN##PluginInstance{ \
        public: \
                Static##PLUGIN##PluginInstance() { \
                    qRegisterStaticPluginFunction(qt_static_plugin_##PLUGIN()); \
                } \
        }; \
       static Static##PLUGIN##PluginInstance static##PLUGIN##Instance;

#define Q_PLUGIN_INSTANCE(IMPLEMENTATION) \
        { \
            static QT_PREPEND_NAMESPACE(QPointer)<QT_PREPEND_NAMESPACE(QObject)> _instance; \
            if (!_instance)      \
                _instance = new IMPLEMENTATION; \
            return _instance; \
        }

#if defined(QT_STATICPLUGIN)

#  define QT_MOC_EXPORT_PLUGIN(PLUGINCLASS, PLUGINCLASSNAME) \
    static QT_PREPEND_NAMESPACE(QObject) *qt_plugin_instance_##PLUGINCLASSNAME() \
    Q_PLUGIN_INSTANCE(PLUGINCLASS) \
    static const char *qt_plugin_query_metadata_##PLUGINCLASSNAME() { return reinterpret_cast<const char *>(qt_pluginMetaData); } \
    const QT_PREPEND_NAMESPACE(QStaticPlugin) qt_static_plugin_##PLUGINCLASSNAME() { \
        QT_PREPEND_NAMESPACE(QStaticPlugin) plugin = { qt_plugin_instance_##PLUGINCLASSNAME, qt_plugin_query_metadata_##PLUGINCLASSNAME}; \
        return plugin; \
    }

#else

#  define QT_MOC_EXPORT_PLUGIN(PLUGINCLASS, PLUGINCLASSNAME)      \
            Q_EXTERN_C Q_DECL_EXPORT \
            const char *qt_plugin_query_metadata() \
            { return reinterpret_cast<const char *>(qt_pluginMetaData); } \
            Q_EXTERN_C Q_DECL_EXPORT QT_PREPEND_NAMESPACE(QObject) *qt_plugin_instance() \
            Q_PLUGIN_INSTANCE(PLUGINCLASS)

#endif


#define Q_EXPORT_PLUGIN(PLUGIN) \
            Q_EXPORT_PLUGIN2(PLUGIN, PLUGIN)
#  define Q_EXPORT_PLUGIN2(PLUGIN, PLUGINCLASS)      \
    Q_STATIC_ASSERT_X(false, "Old plugin system used")

#  define Q_EXPORT_STATIC_PLUGIN2(PLUGIN, PLUGINCLASS) \
    Q_STATIC_ASSERT_X(false, "Old plugin system used")


QT_END_NAMESPACE

#endif // Q_PLUGIN_H
