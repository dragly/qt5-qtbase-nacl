/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the documentation of the Qt Toolkit.
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

#include "openglwindow.h"

#include <QtGui>
#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>

#include <QtCore/qmath.h>

//! [1]
class TriangleWindow : public OpenGLWindow
{
public:
    TriangleWindow();

    void initialize() Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;

private:
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;

    QOpenGLShaderProgram *m_program;
    int m_frame;
    QVector<GLuint> m_vboIds;
};

TriangleWindow::TriangleWindow()
    : m_program(0)
    , m_frame(0)
{
}
//! [1]

TriangleWindow *window;

//! [2]
void app_init(int argc, char **argv)
{
    qDebug() << "Starting app";
//    QGuiApplication app(argc, argv);
    // QLoggingCategory::setFilterRules(QStringLiteral("*.*=true"));

    QSurfaceFormat format;
    format.setSamples(16);
    qDebug() << "Starting window";

    window = new TriangleWindow();
    window->setFormat(format);
    window->resize(640, 480);
    window->show();
    qDebug() << "Start animation";

    window->setAnimating(true);
    qDebug() << "Animation started!";
//    return app.exec();
}
//! [2]
//
void app_exit() {
    delete window;
}

#ifdef Q_OS_NACL
Q_GUI_MAIN(app_init, app_exit)
#else
int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    app_init(argc, argv);
    int retval =  app.exec();
    app_exit();
    return retval;
}
#endif



//! [3]
static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute highp vec4 colAttr;\n"
    "varying highp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position = matrix * posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying lowp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";
//! [3]

//! [4]
void TriangleWindow::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");
    
    int m_numberOfVBOs = 1;
    m_vboIds.resize(m_numberOfVBOs);
    glGenBuffers(m_numberOfVBOs, &m_vboIds.front());
}
//! [4]

struct Vertex {
    QVector2D position;
    QVector3D color;
};

//! [5]
void TriangleWindow::render()
{    
    // qDebug() << "Render!!!!";
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClearColor(0.1, 0.1, 0.15, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();

    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f);
    matrix.translate(0, 0, -2);
    matrix.rotate(100.0f * m_frame / screen()->refreshRate(), 0, 1, 0);

    m_program->setUniformValue(m_matrixUniform, matrix);

//    GLfloat vertices[] = {
//        0.0f, 0.707f,
//        -0.5f, -0.5f,
//        0.5f, -0.5f
//    };

//    GLfloat colors[] = {
//        1.0f, 0.0f, 0.0f,
//        0.0f, 1.0f, 0.0f,
//        0.0f, 0.0f, 1.0f
//    };

    QVector<Vertex> vertices;
    vertices.resize(3);

    vertices[0].position = QVector2D(0.0, 0.707);
    vertices[1].position = QVector2D(-0.5, -0.5);
    vertices[2].position = QVector2D(0.5, -0.5);

    vertices[0].color = QVector3D(1.0, 0.0, 0.0);
    vertices[1].color = QVector3D(0.0, 1.0, 0.0);
    vertices[2].color = QVector3D(0.0, 0.0, 1.0);

    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices.front(), GL_STATIC_DRAW);

    quintptr offset = 0;
    m_program->enableAttributeArray(m_posAttr);
    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, sizeof(QVector2D), (const void *)offset);

    offset += sizeof(QVector3D);
    m_program->enableAttributeArray(m_colAttr);
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (const void *)offset);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    m_program->disableAttributeArray(m_colAttr);
    m_program->disableAttributeArray(m_posAttr);

    m_program->release();

    ++m_frame;
}
//! [5]
