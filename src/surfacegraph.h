#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "geometryengine.h"
#include "triangulation.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class GeometryEngine;

class SurfaceGraph : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit SurfaceGraph(QWidget *parent = 0);
    ~SurfaceGraph();
    void redraw(TriangleList triangles);
    void toggleTransparency(bool isTransparent);

protected:
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void setAxisAndSpeed(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e) Q_DECL_OVERRIDE;
    void timerEvent(QTimerEvent *e) Q_DECL_OVERRIDE;

    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

    void initShaders();
    void initTextures();

private:
    QBasicTimer timer;
    QOpenGLShaderProgram program;
    GeometryEngine *geometries;

    QOpenGLTexture *texture;

    QMatrix4x4 projection;

    QVector2D mousePressPosition;
    QVector3D rotationAxis;
    qreal angularSpeed;
    QQuaternion rotation;

    float zoom;
    bool isDragging;
};

#endif // MAINWIDGET_H
