#include "surfacegraph.h"

#include <QMouseEvent>
#include <QWheelEvent>

#include <math.h>
#include <iostream>


SurfaceGraph::SurfaceGraph(QWidget *parent) :
    QOpenGLWidget(parent),
    geometries(0),
    texture(0),
    angularSpeed(0)
{
    zoom = 1.0;
    isDragging = false;
}

SurfaceGraph::~SurfaceGraph()
{
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete texture;
    delete geometries;
    doneCurrent();
}

void SurfaceGraph::mousePressEvent(QMouseEvent *e)
{
    // Save mouse press position
    mousePressPosition = QVector2D(e->localPos());
    isDragging = true;
}

void SurfaceGraph::mouseMoveEvent(QMouseEvent *e) {
    if (isDragging) {
        setAxisAndSpeed(e);
        angularSpeed = 2;
        mousePressPosition = QVector2D(e->localPos());
    }
}

void SurfaceGraph::mouseReleaseEvent(QMouseEvent *e)
{
    setAxisAndSpeed(e);
    isDragging = false;
}

void SurfaceGraph::setAxisAndSpeed(QMouseEvent *e)
{
    // Mouse release position - mouse press position
    QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

    // Rotation axis is perpendicular to the mouse position difference
    // vector
    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

    // Accelerate angular speed relative to the length of the mouse sweep
    qreal acc = diff.length() / 100.0;

    // Calculate new rotation axis as weighted sum
    rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

    // Increase angular speed
    angularSpeed += acc;
}

void SurfaceGraph::wheelEvent(QWheelEvent *e)
{
    if(e->delta() > 0) {
        zoom *= 1.1;
    }
    else if(e->delta() < 0) {
        zoom /= 1.1;
    }

    update();
}

void SurfaceGraph::timerEvent(QTimerEvent *)
{
    // Stop rotation when speed goes below threshold
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
    } else {
        // Update rotation
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;
        update();
    }

    // decrease angular speed (gradually if we flicked the object)
    if (isDragging) {
        angularSpeed = 0;
    }
    else {
        angularSpeed *= 0.99;
    }
}

void SurfaceGraph::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    initShaders();
    initTextures();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    geometries = new GeometryEngine;

    // Use QBasicTimer because its faster than QTimer
    timer.start(12, this);
}

void SurfaceGraph::initShaders()
{
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}

void SurfaceGraph::initTextures()
{
    // Load texture image
    texture = new QOpenGLTexture(QImage(":/cube.png").mirrored());

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);
}

void SurfaceGraph::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}

void SurfaceGraph::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    texture->bind();

    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, -5.0);
    matrix.rotate(rotation);
    matrix.scale(zoom,zoom,zoom);

    // Set modelview-projection matrix
    program.setUniformValue("mvp_matrix", projection * matrix);

    // Use texture unit 0 (which file to use)
    program.setUniformValue("texture", 0);

    // Draw geometry
    geometries->drawGeometry(&program);
}

void SurfaceGraph::redraw(TriangleList triangles)
{
    makeCurrent();
    delete geometries;
    geometries = new GeometryEngine(triangles);
    doneCurrent();

    update();
}

void SurfaceGraph::toggleTransparency(bool isTransparent) {
    geometries->isTransparent = isTransparent;

    update();
}
