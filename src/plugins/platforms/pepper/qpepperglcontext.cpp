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

#include <qglobal.h>

#ifndef QT_NO_OPENGL
#include "qpepperglcontext.h"

#include "qpepperinstance_p.h"

#include <QtCore/QDebug>

#include <ppapi/cpp/graphics_3d.h>
#include <ppapi/cpp/graphics_3d_client.h>

#ifdef Q_OS_NACL_EMSCRIPTEN
// Emscriptens GLES2 API is more complete than pepper.js
// In the future, this might change. We should then include <GLES2/gl2.h> then.
#include "3rdparty/emscripten/GLES2/gl2.h"
#include "3rdparty/emscripten/GLES2/gl2ext.h"

#else

#include <ppapi/gles2/gl2ext_ppapi.h>

#endif


#include <QtCore/QCoreApplication>
#include <QtCore/QThread>

Q_LOGGING_CATEGORY(QT_PLATFORM_PEPPER_GLCONTEXT, "qt.platform.pepper.glcontext")

QPepperGLContext::QPepperGLContext()
    : m_pendingFlush(false)
    , m_callbackFactory(this)
{
    qCDebug(QT_PLATFORM_PEPPER_GLCONTEXT) << "QPepperGLContext";
}

QPepperGLContext::~QPepperGLContext() {}

QSurfaceFormat QPepperGLContext::format() const
{
    qCDebug(QT_PLATFORM_PEPPER_GLCONTEXT) << "format";

    QSurfaceFormat format;
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setAlphaBufferSize(8);
    format.setRenderableType(QSurfaceFormat::OpenGLES);
    return format;
}

void QPepperGLContext::swapBuffers(QPlatformSurface *surface)
{
    Q_UNUSED(surface);
    qCDebug(QT_PLATFORM_PEPPER_GLCONTEXT) << "swapBuffers";

    if (m_pendingFlush) {
        qCDebug(QT_PLATFORM_PEPPER_GLCONTEXT) << "swapBuffers overflush";
        return;
    }

    m_pendingFlush = true;
    m_context.SwapBuffers(m_callbackFactory.NewCallback(&QPepperGLContext::flushCallback));
    bool qtOnSecondaryThread = QPepperInstancePrivate::get()->m_runQtOnThread;
    while (qtOnSecondaryThread && m_pendingFlush) {
        QCoreApplication::processEvents();
        QThread::msleep(1);
    }
}

bool QPepperGLContext::makeCurrent(QPlatformSurface *surface)
{
    Q_UNUSED(surface);
    qCDebug(QT_PLATFORM_PEPPER_GLCONTEXT) << "makeCurrent";

    if (m_context.is_null())
        initGl();

    QSize newSize = QPepperInstancePrivate::get()->deviceGeometry().size();
    if (newSize != m_currentSize) {
        int32_t result = m_context.ResizeBuffers(newSize.width(), newSize.height());
        if (result < 0) {
            qWarning() << "Unable to resize buffer to" << newSize;
            return false;
        }
        m_currentSize = newSize;
    }
#ifndef Q_OS_NACL_EMSCRIPTEN
    glSetCurrentContextPPAPI(m_context.pp_resource());
#endif
    return true;
}

void QPepperGLContext::doneCurrent()
{
    qCDebug(QT_PLATFORM_PEPPER_GLCONTEXT) << "doneCurrent";
#ifndef Q_OS_NACL_EMSCRIPTEN    
    glSetCurrentContextPPAPI(0);
#endif
}

QFunctionPointer QPepperGLContext::getProcAddress(const QByteArray &procName)
{
    qCDebug(QT_PLATFORM_PEPPER_GLCONTEXT) << "getProcAddress" << procName;
#ifndef Q_OS_NACL_EMSCRIPTEN
    glSetCurrentContextPPAPI(m_context.pp_resource());
#endif
//    const PPB_OpenGLES2* functionPointers = glGetInterfacePPAPI();
    if (procName == QByteArrayLiteral("glActiveTexture")) {
        return reinterpret_cast<QFunctionPointer>(glActiveTexture);
    }

    if (procName == QByteArrayLiteral("glAttachShader")) {
        return reinterpret_cast<QFunctionPointer>(glAttachShader);
    }

    if (procName == QByteArrayLiteral("glBindAttribLocation")) {
        return reinterpret_cast<QFunctionPointer>(glBindAttribLocation);
    }

    if (procName == QByteArrayLiteral("glBindBuffer")) {
        return reinterpret_cast<QFunctionPointer>(glBindBuffer);
    }

    if (procName == QByteArrayLiteral("glBindFramebuffer")) {
        return reinterpret_cast<QFunctionPointer>(glBindFramebuffer);
    }

    if (procName == QByteArrayLiteral("glBindRenderbuffer")) {
        return reinterpret_cast<QFunctionPointer>(glBindRenderbuffer);
    }

    if (procName == QByteArrayLiteral("glBindTexture")) {
        return reinterpret_cast<QFunctionPointer>(glBindTexture);
    }

    if (procName == QByteArrayLiteral("glBlendColor")) {
        return reinterpret_cast<QFunctionPointer>(glBlendColor);
    }

    if (procName == QByteArrayLiteral("glBlendEquation")) {
        return reinterpret_cast<QFunctionPointer>(glBlendEquation);
    }

    if (procName == QByteArrayLiteral("glBlendEquationSeparate")) {
        return reinterpret_cast<QFunctionPointer>(glBlendEquationSeparate);
    }

    if (procName == QByteArrayLiteral("glBlendFunc")) {
        return reinterpret_cast<QFunctionPointer>(glBlendFunc);
    }

    if (procName == QByteArrayLiteral("glBlendFuncSeparate")) {
        return reinterpret_cast<QFunctionPointer>(glBlendFuncSeparate);
    }

    if (procName == QByteArrayLiteral("glBufferData")) {
        return reinterpret_cast<QFunctionPointer>(glBufferData);
    }

    if (procName == QByteArrayLiteral("glBufferSubData")) {
        return reinterpret_cast<QFunctionPointer>(glBufferSubData);
    }

    if (procName == QByteArrayLiteral("glCheckFramebufferStatus")) {
        return reinterpret_cast<QFunctionPointer>(glCheckFramebufferStatus);
    }

    if (procName == QByteArrayLiteral("glClear")) {
        return reinterpret_cast<QFunctionPointer>(glClear);
    }

    if (procName == QByteArrayLiteral("glClearColor")) {
        return reinterpret_cast<QFunctionPointer>(glClearColor);
    }

    if (procName == QByteArrayLiteral("glClearDepthf")) {
        return reinterpret_cast<QFunctionPointer>(glClearDepthf);
    }

    if (procName == QByteArrayLiteral("glClearStencil")) {
        return reinterpret_cast<QFunctionPointer>(glClearStencil);
    }

    if (procName == QByteArrayLiteral("glColorMask")) {
        return reinterpret_cast<QFunctionPointer>(glColorMask);
    }

    if (procName == QByteArrayLiteral("glCompileShader")) {
        return reinterpret_cast<QFunctionPointer>(glCompileShader);
    }

    if (procName == QByteArrayLiteral("glCompressedTexImage2D")) {
        return reinterpret_cast<QFunctionPointer>(glCompressedTexImage2D);
    }

    if (procName == QByteArrayLiteral("glCompressedTexSubImage2D")) {
        return reinterpret_cast<QFunctionPointer>(glCompressedTexSubImage2D);
    }

    if (procName == QByteArrayLiteral("glCopyTexImage2D")) {
        return reinterpret_cast<QFunctionPointer>(glCopyTexImage2D);
    }

    if (procName == QByteArrayLiteral("glCopyTexSubImage2D")) {
        return reinterpret_cast<QFunctionPointer>(glCopyTexSubImage2D);
    }

    if (procName == QByteArrayLiteral("glCreateProgram")) {
        return reinterpret_cast<QFunctionPointer>(glCreateProgram);
    }

    if (procName == QByteArrayLiteral("glCreateShader")) {
        return reinterpret_cast<QFunctionPointer>(glCreateShader);
    }

    if (procName == QByteArrayLiteral("glCullFace")) {
        return reinterpret_cast<QFunctionPointer>(glCullFace);
    }

    if (procName == QByteArrayLiteral("glDeleteBuffers")) {
        return reinterpret_cast<QFunctionPointer>(glDeleteBuffers);
    }

    if (procName == QByteArrayLiteral("glDeleteFramebuffers")) {
        return reinterpret_cast<QFunctionPointer>(glDeleteFramebuffers);
    }

    if (procName == QByteArrayLiteral("glDeleteProgram")) {
        return reinterpret_cast<QFunctionPointer>(glDeleteProgram);
    }

    if (procName == QByteArrayLiteral("glDeleteRenderbuffers")) {
        return reinterpret_cast<QFunctionPointer>(glDeleteRenderbuffers);
    }

    if (procName == QByteArrayLiteral("glDeleteShader")) {
        return reinterpret_cast<QFunctionPointer>(glDeleteShader);
    }

    if (procName == QByteArrayLiteral("glDeleteTextures")) {
        return reinterpret_cast<QFunctionPointer>(glDeleteTextures);
    }

    if (procName == QByteArrayLiteral("glDepthFunc")) {
        return reinterpret_cast<QFunctionPointer>(glDepthFunc);
    }

    if (procName == QByteArrayLiteral("glDepthMask")) {
        return reinterpret_cast<QFunctionPointer>(glDepthMask);
    }

    if (procName == QByteArrayLiteral("glDepthRangef")) {
        return reinterpret_cast<QFunctionPointer>(glDepthRangef);
    }

    if (procName == QByteArrayLiteral("glDetachShader")) {
        return reinterpret_cast<QFunctionPointer>(glDetachShader);
    }

    if (procName == QByteArrayLiteral("glDisable")) {
        return reinterpret_cast<QFunctionPointer>(glDisable);
    }

    if (procName == QByteArrayLiteral("glDisableVertexAttribArray")) {
        return reinterpret_cast<QFunctionPointer>(glDisableVertexAttribArray);
    }

    if (procName == QByteArrayLiteral("glDrawArrays")) {
        return reinterpret_cast<QFunctionPointer>(glDrawArrays);
    }

    if (procName == QByteArrayLiteral("glDrawElements")) {
        return reinterpret_cast<QFunctionPointer>(glDrawElements);
    }

    if (procName == QByteArrayLiteral("glEnable")) {
        return reinterpret_cast<QFunctionPointer>(glEnable);
    }

    if (procName == QByteArrayLiteral("glEnableVertexAttribArray")) {
        return reinterpret_cast<QFunctionPointer>(glEnableVertexAttribArray);
    }

    if (procName == QByteArrayLiteral("glFinish")) {
        return reinterpret_cast<QFunctionPointer>(glFinish);
    }

    if (procName == QByteArrayLiteral("glFlush")) {
        return reinterpret_cast<QFunctionPointer>(glFlush);
    }

    if (procName == QByteArrayLiteral("glFramebufferRenderbuffer")) {
        return reinterpret_cast<QFunctionPointer>(glFramebufferRenderbuffer);
    }

    if (procName == QByteArrayLiteral("glFramebufferTexture2D")) {
        return reinterpret_cast<QFunctionPointer>(glFramebufferTexture2D);
    }

    if (procName == QByteArrayLiteral("glFrontFace")) {
        return reinterpret_cast<QFunctionPointer>(glFrontFace);
    }

    if (procName == QByteArrayLiteral("glGenBuffers")) {
        return reinterpret_cast<QFunctionPointer>(glGenBuffers);
    }

    if (procName == QByteArrayLiteral("glGenerateMipmap")) {
        return reinterpret_cast<QFunctionPointer>(glGenerateMipmap);
    }

    if (procName == QByteArrayLiteral("glGenFramebuffers")) {
        return reinterpret_cast<QFunctionPointer>(glGenFramebuffers);
    }

    if (procName == QByteArrayLiteral("glGenRenderbuffers")) {
        return reinterpret_cast<QFunctionPointer>(glGenRenderbuffers);
    }

    if (procName == QByteArrayLiteral("glGenTextures")) {
        return reinterpret_cast<QFunctionPointer>(glGenTextures);
    }

    if (procName == QByteArrayLiteral("glGetActiveAttrib")) {
        return reinterpret_cast<QFunctionPointer>(glGetActiveAttrib);
    }

    if (procName == QByteArrayLiteral("glGetActiveUniform")) {
        return reinterpret_cast<QFunctionPointer>(glGetActiveUniform);
    }

    if (procName == QByteArrayLiteral("glGetAttachedShaders")) {
        return reinterpret_cast<QFunctionPointer>(glGetAttachedShaders);
    }

    if (procName == QByteArrayLiteral("glGetAttribLocation")) {
        return reinterpret_cast<QFunctionPointer>(glGetAttribLocation);
    }

    if (procName == QByteArrayLiteral("glGetBooleanv")) {
        return reinterpret_cast<QFunctionPointer>(glGetBooleanv);
    }

    if (procName == QByteArrayLiteral("glGetBufferParameteriv")) {
        return reinterpret_cast<QFunctionPointer>(glGetBufferParameteriv);
    }

    if (procName == QByteArrayLiteral("glGetError")) {
        return reinterpret_cast<QFunctionPointer>(glGetError);
    }

    if (procName == QByteArrayLiteral("glGetFloatv")) {
        return reinterpret_cast<QFunctionPointer>(glGetFloatv);
    }

    if (procName == QByteArrayLiteral("glGetFramebufferAttachmentParameteriv")) {
        return reinterpret_cast<QFunctionPointer>(glGetFramebufferAttachmentParameteriv);
    }

    if (procName == QByteArrayLiteral("glGetIntegerv")) {
        return reinterpret_cast<QFunctionPointer>(glGetIntegerv);
    }

    if (procName == QByteArrayLiteral("glGetProgramiv")) {
        return reinterpret_cast<QFunctionPointer>(glGetProgramiv);
    }

    if (procName == QByteArrayLiteral("glGetProgramInfoLog")) {
        return reinterpret_cast<QFunctionPointer>(glGetProgramInfoLog);
    }

    if (procName == QByteArrayLiteral("glGetRenderbufferParameteriv")) {
        return reinterpret_cast<QFunctionPointer>(glGetRenderbufferParameteriv);
    }

    if (procName == QByteArrayLiteral("glGetShaderiv")) {
        return reinterpret_cast<QFunctionPointer>(glGetShaderiv);
    }

    if (procName == QByteArrayLiteral("glGetShaderInfoLog")) {
        return reinterpret_cast<QFunctionPointer>(glGetShaderInfoLog);
    }

    if (procName == QByteArrayLiteral("glGetShaderPrecisionFormat")) {
        return reinterpret_cast<QFunctionPointer>(glGetShaderPrecisionFormat);
    }

    if (procName == QByteArrayLiteral("glGetShaderSource")) {
        return reinterpret_cast<QFunctionPointer>(glGetShaderSource);
    }

    if (procName == QByteArrayLiteral("glGetString")) {
        return reinterpret_cast<QFunctionPointer>(glGetString);
    }

    if (procName == QByteArrayLiteral("glGetTexParameterfv")) {
        return reinterpret_cast<QFunctionPointer>(glGetTexParameterfv);
    }

    if (procName == QByteArrayLiteral("glGetTexParameteriv")) {
        return reinterpret_cast<QFunctionPointer>(glGetTexParameteriv);
    }

    if (procName == QByteArrayLiteral("glGetUniformfv")) {
        return reinterpret_cast<QFunctionPointer>(glGetUniformfv);
    }

    if (procName == QByteArrayLiteral("glGetUniformiv")) {
        return reinterpret_cast<QFunctionPointer>(glGetUniformiv);
    }

    if (procName == QByteArrayLiteral("glGetUniformLocation")) {
        return reinterpret_cast<QFunctionPointer>(glGetUniformLocation);
    }

    if (procName == QByteArrayLiteral("glGetVertexAttribfv")) {
        return reinterpret_cast<QFunctionPointer>(glGetVertexAttribfv);
    }

    if (procName == QByteArrayLiteral("glGetVertexAttribiv")) {
        return reinterpret_cast<QFunctionPointer>(glGetVertexAttribiv);
    }

    if (procName == QByteArrayLiteral("glGetVertexAttribPointerv")) {
        return reinterpret_cast<QFunctionPointer>(glGetVertexAttribPointerv);
    }

    if (procName == QByteArrayLiteral("glHint")) {
        return reinterpret_cast<QFunctionPointer>(glHint);
    }

    if (procName == QByteArrayLiteral("glIsBuffer")) {
        return reinterpret_cast<QFunctionPointer>(glIsBuffer);
    }

    if (procName == QByteArrayLiteral("glIsEnabled")) {
        return reinterpret_cast<QFunctionPointer>(glIsEnabled);
    }

    if (procName == QByteArrayLiteral("glIsFramebuffer")) {
        return reinterpret_cast<QFunctionPointer>(glIsFramebuffer);
    }

    if (procName == QByteArrayLiteral("glIsProgram")) {
        return reinterpret_cast<QFunctionPointer>(glIsProgram);
    }

    if (procName == QByteArrayLiteral("glIsRenderbuffer")) {
        return reinterpret_cast<QFunctionPointer>(glIsRenderbuffer);
    }

    if (procName == QByteArrayLiteral("glIsShader")) {
        return reinterpret_cast<QFunctionPointer>(glIsShader);
    }

    if (procName == QByteArrayLiteral("glIsTexture")) {
        return reinterpret_cast<QFunctionPointer>(glIsTexture);
    }

    if (procName == QByteArrayLiteral("glLineWidth")) {
        return reinterpret_cast<QFunctionPointer>(glLineWidth);
    }

    if (procName == QByteArrayLiteral("glLinkProgram")) {
        return reinterpret_cast<QFunctionPointer>(glLinkProgram);
    }

    if (procName == QByteArrayLiteral("glPixelStorei")) {
        return reinterpret_cast<QFunctionPointer>(glPixelStorei);
    }

    if (procName == QByteArrayLiteral("glPolygonOffset")) {
        return reinterpret_cast<QFunctionPointer>(glPolygonOffset);
    }

    if (procName == QByteArrayLiteral("glReadPixels")) {
        return reinterpret_cast<QFunctionPointer>(glReadPixels);
    }

    if (procName == QByteArrayLiteral("glReleaseShaderCompiler")) {
        return reinterpret_cast<QFunctionPointer>(glReleaseShaderCompiler);
    }

    if (procName == QByteArrayLiteral("glRenderbufferStorage")) {
        return reinterpret_cast<QFunctionPointer>(glRenderbufferStorage);
    }

    if (procName == QByteArrayLiteral("glSampleCoverage")) {
        return reinterpret_cast<QFunctionPointer>(glSampleCoverage);
    }

    if (procName == QByteArrayLiteral("glScissor")) {
        return reinterpret_cast<QFunctionPointer>(glScissor);
    }

    if (procName == QByteArrayLiteral("glShaderBinary")) {
        return reinterpret_cast<QFunctionPointer>(glShaderBinary);
    }

    if (procName == QByteArrayLiteral("glShaderSource")) {
        return reinterpret_cast<QFunctionPointer>(glShaderSource);
    }

    if (procName == QByteArrayLiteral("glStencilFunc")) {
        return reinterpret_cast<QFunctionPointer>(glStencilFunc);
    }

    if (procName == QByteArrayLiteral("glStencilFuncSeparate")) {
        return reinterpret_cast<QFunctionPointer>(glStencilFuncSeparate);
    }

    if (procName == QByteArrayLiteral("glStencilMask")) {
        return reinterpret_cast<QFunctionPointer>(glStencilMask);
    }

    if (procName == QByteArrayLiteral("glStencilMaskSeparate")) {
        return reinterpret_cast<QFunctionPointer>(glStencilMaskSeparate);
    }

    if (procName == QByteArrayLiteral("glStencilOp")) {
        return reinterpret_cast<QFunctionPointer>(glStencilOp);
    }

    if (procName == QByteArrayLiteral("glStencilOpSeparate")) {
        return reinterpret_cast<QFunctionPointer>(glStencilOpSeparate);
    }

    if (procName == QByteArrayLiteral("glTexImage2D")) {
        return reinterpret_cast<QFunctionPointer>(glTexImage2D);
    }

    if (procName == QByteArrayLiteral("glTexParameterf")) {
        return reinterpret_cast<QFunctionPointer>(glTexParameterf);
    }

    if (procName == QByteArrayLiteral("glTexParameterfv")) {
        return reinterpret_cast<QFunctionPointer>(glTexParameterfv);
    }

    if (procName == QByteArrayLiteral("glTexParameteri")) {
        return reinterpret_cast<QFunctionPointer>(glTexParameteri);
    }

    if (procName == QByteArrayLiteral("glTexParameteriv")) {
        return reinterpret_cast<QFunctionPointer>(glTexParameteriv);
    }

    if (procName == QByteArrayLiteral("glTexSubImage2D")) {
        return reinterpret_cast<QFunctionPointer>(glTexSubImage2D);
    }

    if (procName == QByteArrayLiteral("glUniform1f")) {
        return reinterpret_cast<QFunctionPointer>(glUniform1f);
    }

    if (procName == QByteArrayLiteral("glUniform1fv")) {
        return reinterpret_cast<QFunctionPointer>(glUniform1fv);
    }

    if (procName == QByteArrayLiteral("glUniform1i")) {
        return reinterpret_cast<QFunctionPointer>(glUniform1i);
    }

    if (procName == QByteArrayLiteral("glUniform1iv")) {
        return reinterpret_cast<QFunctionPointer>(glUniform1iv);
    }

    if (procName == QByteArrayLiteral("glUniform2f")) {
        return reinterpret_cast<QFunctionPointer>(glUniform2f);
    }

    if (procName == QByteArrayLiteral("glUniform2fv")) {
        return reinterpret_cast<QFunctionPointer>(glUniform2fv);
    }

    if (procName == QByteArrayLiteral("glUniform2i")) {
        return reinterpret_cast<QFunctionPointer>(glUniform2i);
    }

    if (procName == QByteArrayLiteral("glUniform2iv")) {
        return reinterpret_cast<QFunctionPointer>(glUniform2iv);
    }

    if (procName == QByteArrayLiteral("glUniform3f")) {
        return reinterpret_cast<QFunctionPointer>(glUniform3f);
    }

    if (procName == QByteArrayLiteral("glUniform3fv")) {
        return reinterpret_cast<QFunctionPointer>(glUniform3fv);
    }

    if (procName == QByteArrayLiteral("glUniform3i")) {
        return reinterpret_cast<QFunctionPointer>(glUniform3i);
    }

    if (procName == QByteArrayLiteral("glUniform3iv")) {
        return reinterpret_cast<QFunctionPointer>(glUniform3iv);
    }

    if (procName == QByteArrayLiteral("glUniform4f")) {
        return reinterpret_cast<QFunctionPointer>(glUniform4f);
    }

    if (procName == QByteArrayLiteral("glUniform4fv")) {
        return reinterpret_cast<QFunctionPointer>(glUniform4fv);
    }

    if (procName == QByteArrayLiteral("glUniform4i")) {
        return reinterpret_cast<QFunctionPointer>(glUniform4i);
    }

    if (procName == QByteArrayLiteral("glUniform4iv")) {
        return reinterpret_cast<QFunctionPointer>(glUniform4iv);
    }

    if (procName == QByteArrayLiteral("glUniformMatrix2fv")) {
        return reinterpret_cast<QFunctionPointer>(glUniformMatrix2fv);
    }

    if (procName == QByteArrayLiteral("glUniformMatrix3fv")) {
        return reinterpret_cast<QFunctionPointer>(glUniformMatrix3fv);
    }

    if (procName == QByteArrayLiteral("glUniformMatrix4fv")) {
        return reinterpret_cast<QFunctionPointer>(glUniformMatrix4fv);
    }

    if (procName == QByteArrayLiteral("glUseProgram")) {
        return reinterpret_cast<QFunctionPointer>(glUseProgram);
    }

    if (procName == QByteArrayLiteral("glValidateProgram")) {
        return reinterpret_cast<QFunctionPointer>(glValidateProgram);
    }

    if (procName == QByteArrayLiteral("glVertexAttrib1f")) {
        return reinterpret_cast<QFunctionPointer>(glVertexAttrib1f);
    }

    if (procName == QByteArrayLiteral("glVertexAttrib1fv")) {
        return reinterpret_cast<QFunctionPointer>(glVertexAttrib1fv);
    }

    if (procName == QByteArrayLiteral("glVertexAttrib2f")) {
        return reinterpret_cast<QFunctionPointer>(glVertexAttrib2f);
    }

    if (procName == QByteArrayLiteral("glVertexAttrib2fv")) {
        return reinterpret_cast<QFunctionPointer>(glVertexAttrib2fv);
    }

    if (procName == QByteArrayLiteral("glVertexAttrib3f")) {
        return reinterpret_cast<QFunctionPointer>(glVertexAttrib3f);
    }

    if (procName == QByteArrayLiteral("glVertexAttrib3fv")) {
        return reinterpret_cast<QFunctionPointer>(glVertexAttrib3fv);
    }

    if (procName == QByteArrayLiteral("glVertexAttrib4f")) {
        return reinterpret_cast<QFunctionPointer>(glVertexAttrib4f);
    }

    if (procName == QByteArrayLiteral("glVertexAttrib4fv")) {
        return reinterpret_cast<QFunctionPointer>(glVertexAttrib4fv);
    }

    if (procName == QByteArrayLiteral("glVertexAttribPointer")) {
        return reinterpret_cast<QFunctionPointer>(glVertexAttribPointer);
    }

    if (procName == QByteArrayLiteral("glViewport")) {
        return reinterpret_cast<QFunctionPointer>(glViewport);
    }

    // TODO Consider adding glBlitFramebufferEXT glRenderbufferStorageMultisampleEXT etc. from ppapi/lib/gl/gles2/gles2.c
    return 0;
}

void QPepperGLContext::flushCallback(int32_t)
{
    qCDebug(QT_PLATFORM_PEPPER_GLCONTEXT) << "swapBuffers callback";
    m_pendingFlush = false;
}

bool QPepperGLContext::initGl()
{
    qCDebug(QT_PLATFORM_PEPPER_GLCONTEXT) << "initGl";
#ifndef Q_OS_NACL_EMSCRIPTEN
    if (!glInitializePPAPI(pp::Module::Get()->get_browser_interface())) {
        qWarning("Unable to initialize GL PPAPI!\n");
        return false;
    }
#endif
    m_currentSize = QPepperInstancePrivate::get()->geometry().size();
    QSurfaceFormat f = format();

    const int32_t attrib_list[] = {
      PP_GRAPHICS3DATTRIB_ALPHA_SIZE, f.alphaBufferSize(),
      PP_GRAPHICS3DATTRIB_DEPTH_SIZE, f.depthBufferSize(),
      PP_GRAPHICS3DATTRIB_STENCIL_SIZE, f.stencilBufferSize(),
      PP_GRAPHICS3DATTRIB_WIDTH, m_currentSize.width(),
      PP_GRAPHICS3DATTRIB_HEIGHT, m_currentSize.height(),
      PP_GRAPHICS3DATTRIB_NONE
    };

    pp::Instance *instance = QPepperInstancePrivate::getPPInstance();
    m_context = pp::Graphics3D(instance, attrib_list);
    if (!instance->BindGraphics(m_context)) {
        qWarning("Unable to bind 3d context!\n");
        m_context = pp::Graphics3D();
#ifndef Q_OS_NACL_EMSCRIPTEN
        glSetCurrentContextPPAPI(0);
#endif
        return false;
    }

    return true;
}

#endif
