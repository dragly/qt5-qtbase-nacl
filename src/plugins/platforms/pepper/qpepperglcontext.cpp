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
#include <ppapi/gles2/gl2ext_ppapi.h>

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

    glSetCurrentContextPPAPI(m_context.pp_resource());
    return true;
}

void QPepperGLContext::doneCurrent()
{
    qCDebug(QT_PLATFORM_PEPPER_GLCONTEXT) << "doneCurrent";
    glSetCurrentContextPPAPI(0);
}

QFunctionPointer QPepperGLContext::getProcAddress(const QByteArray &procName)
{
    qCDebug(QT_PLATFORM_PEPPER_GLCONTEXT) << "getProcAddress" << procName;

    const PPB_OpenGLES2* functionPointers = glGetInterfacePPAPI();
    if (procName == QByteArrayLiteral("glActiveTexture")) {
    return reinterpret_cast<QFunctionPointer>(functionPointers->ActiveTexture);
    }

    if (procName == QByteArrayLiteral("glAttachShader")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->AttachShader);
    }

    if (procName == QByteArrayLiteral("glBindAttribLocation")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->BindAttribLocation);
    }

    if (procName == QByteArrayLiteral("glBindBuffer")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->BindBuffer);
    }

    if (procName == QByteArrayLiteral("glBindFramebuffer")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->BindFramebuffer);
    }

    if (procName == QByteArrayLiteral("glBindRenderbuffer")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->BindRenderbuffer);
    }

    if (procName == QByteArrayLiteral("glBindTexture")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->BindTexture);
    }

    if (procName == QByteArrayLiteral("glBlendColor")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->BlendColor);
    }

    if (procName == QByteArrayLiteral("glBlendEquation")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->BlendEquation);
    }

    if (procName == QByteArrayLiteral("glBlendEquationSeparate")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->BlendEquationSeparate);
    }

    if (procName == QByteArrayLiteral("glBlendFunc")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->BlendFunc);
    }

    if (procName == QByteArrayLiteral("glBlendFuncSeparate")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->BlendFuncSeparate);
    }

    if (procName == QByteArrayLiteral("glBufferData")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->BufferData);
    }

    if (procName == QByteArrayLiteral("glBufferSubData")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->BufferSubData);
    }

    if (procName == QByteArrayLiteral("glCheckFramebufferStatus")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->CheckFramebufferStatus);
    }

    if (procName == QByteArrayLiteral("glClear")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Clear);
    }

    if (procName == QByteArrayLiteral("glClearColor")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->ClearColor);
    }

    if (procName == QByteArrayLiteral("glClearDepthf")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->ClearDepthf);
    }

    if (procName == QByteArrayLiteral("glClearStencil")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->ClearStencil);
    }

    if (procName == QByteArrayLiteral("glColorMask")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->ColorMask);
    }

    if (procName == QByteArrayLiteral("glCompileShader")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->CompileShader);
    }

    if (procName == QByteArrayLiteral("glCompressedTexImage2D")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->CompressedTexImage2D);
    }

    if (procName == QByteArrayLiteral("glCompressedTexSubImage2D")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->CompressedTexSubImage2D);
    }

    if (procName == QByteArrayLiteral("glCopyTexImage2D")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->CopyTexImage2D);
    }

    if (procName == QByteArrayLiteral("glCopyTexSubImage2D")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->CopyTexSubImage2D);
    }

    if (procName == QByteArrayLiteral("glCreateProgram")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->CreateProgram);
    }

    if (procName == QByteArrayLiteral("glCreateShader")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->CreateShader);
    }

    if (procName == QByteArrayLiteral("glCullFace")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->CullFace);
    }

    if (procName == QByteArrayLiteral("glDeleteBuffers")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->DeleteBuffers);
    }

    if (procName == QByteArrayLiteral("glDeleteFramebuffers")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->DeleteFramebuffers);
    }

    if (procName == QByteArrayLiteral("glDeleteProgram")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->DeleteProgram);
    }

    if (procName == QByteArrayLiteral("glDeleteRenderbuffers")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->DeleteRenderbuffers);
    }

    if (procName == QByteArrayLiteral("glDeleteShader")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->DeleteShader);
    }

    if (procName == QByteArrayLiteral("glDeleteTextures")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->DeleteTextures);
    }

    if (procName == QByteArrayLiteral("glDepthFunc")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->DepthFunc);
    }

    if (procName == QByteArrayLiteral("glDepthMask")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->DepthMask);
    }

    if (procName == QByteArrayLiteral("glDepthRangef")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->DepthRangef);
    }

    if (procName == QByteArrayLiteral("glDetachShader")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->DetachShader);
    }

    if (procName == QByteArrayLiteral("glDisable")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Disable);
    }

    if (procName == QByteArrayLiteral("glDisableVertexAttribArray")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->DisableVertexAttribArray);
    }

    if (procName == QByteArrayLiteral("glDrawArrays")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->DrawArrays);
    }

    if (procName == QByteArrayLiteral("glDrawElements")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->DrawElements);
    }

    if (procName == QByteArrayLiteral("glEnable")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Enable);
    }

    if (procName == QByteArrayLiteral("glEnableVertexAttribArray")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->EnableVertexAttribArray);
    }

    if (procName == QByteArrayLiteral("glFinish")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Finish);
    }

    if (procName == QByteArrayLiteral("glFlush")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Flush);
    }

    if (procName == QByteArrayLiteral("glFramebufferRenderbuffer")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->FramebufferRenderbuffer);
    }

    if (procName == QByteArrayLiteral("glFramebufferTexture2D")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->FramebufferTexture2D);
    }

    if (procName == QByteArrayLiteral("glFrontFace")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->FrontFace);
    }

    if (procName == QByteArrayLiteral("glGenBuffers")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GenBuffers);
    }

    if (procName == QByteArrayLiteral("glGenerateMipmap")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GenerateMipmap);
    }

    if (procName == QByteArrayLiteral("glGenFramebuffers")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GenFramebuffers);
    }

    if (procName == QByteArrayLiteral("glGenRenderbuffers")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GenRenderbuffers);
    }

    if (procName == QByteArrayLiteral("glGenTextures")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GenTextures);
    }

    if (procName == QByteArrayLiteral("glGetActiveAttrib")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetActiveAttrib);
    }

    if (procName == QByteArrayLiteral("glGetActiveUniform")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetActiveUniform);
    }

    if (procName == QByteArrayLiteral("glGetAttachedShaders")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetAttachedShaders);
    }

    if (procName == QByteArrayLiteral("glGetAttribLocation")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetAttribLocation);
    }

    if (procName == QByteArrayLiteral("glGetBooleanv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetBooleanv);
    }

    if (procName == QByteArrayLiteral("glGetBufferParameteriv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetBufferParameteriv);
    }

    if (procName == QByteArrayLiteral("glGetError")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetError);
    }

    if (procName == QByteArrayLiteral("glGetFloatv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetFloatv);
    }

    if (procName == QByteArrayLiteral("glGetFramebufferAttachmentParameteriv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetFramebufferAttachmentParameteriv);
    }

    if (procName == QByteArrayLiteral("glGetIntegerv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetIntegerv);
    }

    if (procName == QByteArrayLiteral("glGetProgramiv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetProgramiv);
    }

    if (procName == QByteArrayLiteral("glGetProgramInfoLog")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetProgramInfoLog);
    }

    if (procName == QByteArrayLiteral("glGetRenderbufferParameteriv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetRenderbufferParameteriv);
    }

    if (procName == QByteArrayLiteral("glGetShaderiv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetShaderiv);
    }

    if (procName == QByteArrayLiteral("glGetShaderInfoLog")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetShaderInfoLog);
    }

    if (procName == QByteArrayLiteral("glGetShaderPrecisionFormat")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetShaderPrecisionFormat);
    }

    if (procName == QByteArrayLiteral("glGetShaderSource")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetShaderSource);
    }

    if (procName == QByteArrayLiteral("glGetString")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetString);
    }

    if (procName == QByteArrayLiteral("glGetTexParameterfv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetTexParameterfv);
    }

    if (procName == QByteArrayLiteral("glGetTexParameteriv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetTexParameteriv);
    }

    if (procName == QByteArrayLiteral("glGetUniformfv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetUniformfv);
    }

    if (procName == QByteArrayLiteral("glGetUniformiv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetUniformiv);
    }

    if (procName == QByteArrayLiteral("glGetUniformLocation")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetUniformLocation);
    }

    if (procName == QByteArrayLiteral("glGetVertexAttribfv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetVertexAttribfv);
    }

    if (procName == QByteArrayLiteral("glGetVertexAttribiv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetVertexAttribiv);
    }

    if (procName == QByteArrayLiteral("glGetVertexAttribPointerv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->GetVertexAttribPointerv);
    }

    if (procName == QByteArrayLiteral("glHint")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Hint);
    }

    if (procName == QByteArrayLiteral("glIsBuffer")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->IsBuffer);
    }

    if (procName == QByteArrayLiteral("glIsEnabled")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->IsEnabled);
    }

    if (procName == QByteArrayLiteral("glIsFramebuffer")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->IsFramebuffer);
    }

    if (procName == QByteArrayLiteral("glIsProgram")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->IsProgram);
    }

    if (procName == QByteArrayLiteral("glIsRenderbuffer")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->IsRenderbuffer);
    }

    if (procName == QByteArrayLiteral("glIsShader")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->IsShader);
    }

    if (procName == QByteArrayLiteral("glIsTexture")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->IsTexture);
    }

    if (procName == QByteArrayLiteral("glLineWidth")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->LineWidth);
    }

    if (procName == QByteArrayLiteral("glLinkProgram")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->LinkProgram);
    }

    if (procName == QByteArrayLiteral("glPixelStorei")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->PixelStorei);
    }

    if (procName == QByteArrayLiteral("glPolygonOffset")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->PolygonOffset);
    }

    if (procName == QByteArrayLiteral("glReadPixels")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->ReadPixels);
    }

    if (procName == QByteArrayLiteral("glReleaseShaderCompiler")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->ReleaseShaderCompiler);
    }

    if (procName == QByteArrayLiteral("glRenderbufferStorage")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->RenderbufferStorage);
    }

    if (procName == QByteArrayLiteral("glSampleCoverage")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->SampleCoverage);
    }

    if (procName == QByteArrayLiteral("glScissor")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Scissor);
    }

    if (procName == QByteArrayLiteral("glShaderBinary")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->ShaderBinary);
    }

    if (procName == QByteArrayLiteral("glShaderSource")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->ShaderSource);
    }

    if (procName == QByteArrayLiteral("glStencilFunc")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->StencilFunc);
    }

    if (procName == QByteArrayLiteral("glStencilFuncSeparate")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->StencilFuncSeparate);
    }

    if (procName == QByteArrayLiteral("glStencilMask")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->StencilMask);
    }

    if (procName == QByteArrayLiteral("glStencilMaskSeparate")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->StencilMaskSeparate);
    }

    if (procName == QByteArrayLiteral("glStencilOp")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->StencilOp);
    }

    if (procName == QByteArrayLiteral("glStencilOpSeparate")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->StencilOpSeparate);
    }

    if (procName == QByteArrayLiteral("glTexImage2D")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->TexImage2D);
    }

    if (procName == QByteArrayLiteral("glTexParameterf")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->TexParameterf);
    }

    if (procName == QByteArrayLiteral("glTexParameterfv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->TexParameterfv);
    }

    if (procName == QByteArrayLiteral("glTexParameteri")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->TexParameteri);
    }

    if (procName == QByteArrayLiteral("glTexParameteriv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->TexParameteriv);
    }

    if (procName == QByteArrayLiteral("glTexSubImage2D")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->TexSubImage2D);
    }

    if (procName == QByteArrayLiteral("glUniform1f")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Uniform1f);
    }

    if (procName == QByteArrayLiteral("glUniform1fv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Uniform1fv);
    }

    if (procName == QByteArrayLiteral("glUniform1i")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Uniform1i);
    }

    if (procName == QByteArrayLiteral("glUniform1iv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Uniform1iv);
    }

    if (procName == QByteArrayLiteral("glUniform2f")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Uniform2f);
    }

    if (procName == QByteArrayLiteral("glUniform2fv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Uniform2fv);
    }

    if (procName == QByteArrayLiteral("glUniform2i")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Uniform2i);
    }

    if (procName == QByteArrayLiteral("glUniform2iv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Uniform2iv);
    }

    if (procName == QByteArrayLiteral("glUniform3f")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Uniform3f);
    }

    if (procName == QByteArrayLiteral("glUniform3fv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Uniform3fv);
    }

    if (procName == QByteArrayLiteral("glUniform3i")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Uniform3i);
    }

    if (procName == QByteArrayLiteral("glUniform3iv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Uniform3iv);
    }

    if (procName == QByteArrayLiteral("glUniform4f")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Uniform4f);
    }

    if (procName == QByteArrayLiteral("glUniform4fv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Uniform4fv);
    }

    if (procName == QByteArrayLiteral("glUniform4i")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Uniform4i);
    }

    if (procName == QByteArrayLiteral("glUniform4iv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Uniform4iv);
    }

    if (procName == QByteArrayLiteral("glUniformMatrix2fv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->UniformMatrix2fv);
    }

    if (procName == QByteArrayLiteral("glUniformMatrix3fv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->UniformMatrix3fv);
    }

    if (procName == QByteArrayLiteral("glUniformMatrix4fv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->UniformMatrix4fv);
    }

    if (procName == QByteArrayLiteral("glUseProgram")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->UseProgram);
    }

    if (procName == QByteArrayLiteral("glValidateProgram")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->ValidateProgram);
    }

    if (procName == QByteArrayLiteral("glVertexAttrib1f")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->VertexAttrib1f);
    }

    if (procName == QByteArrayLiteral("glVertexAttrib1fv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->VertexAttrib1fv);
    }

    if (procName == QByteArrayLiteral("glVertexAttrib2f")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->VertexAttrib2f);
    }

    if (procName == QByteArrayLiteral("glVertexAttrib2fv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->VertexAttrib2fv);
    }

    if (procName == QByteArrayLiteral("glVertexAttrib3f")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->VertexAttrib3f);
    }

    if (procName == QByteArrayLiteral("glVertexAttrib3fv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->VertexAttrib3fv);
    }

    if (procName == QByteArrayLiteral("glVertexAttrib4f")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->VertexAttrib4f);
    }

    if (procName == QByteArrayLiteral("glVertexAttrib4fv")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->VertexAttrib4fv);
    }

    if (procName == QByteArrayLiteral("glVertexAttribPointer")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->VertexAttribPointer);
    }

    if (procName == QByteArrayLiteral("glViewport")) {
        return reinterpret_cast<QFunctionPointer>(functionPointers->Viewport);
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
    if (!glInitializePPAPI(pp::Module::Get()->get_browser_interface())) {
        qWarning("Unable to initialize GL PPAPI!\n");
        return false;
    }
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
        glSetCurrentContextPPAPI(0);
        return false;
    }

    return true;
}

#endif
