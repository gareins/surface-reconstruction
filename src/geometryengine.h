#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include "triangulation.h"

class GeometryEngine : protected QOpenGLFunctions
{
public:
    GeometryEngine();
    GeometryEngine(TriangleList triangles);
    virtual ~GeometryEngine();

    void drawGeometry(QOpenGLShaderProgram *program);

private:
    void initGeometry(TriangleList triangles = {});

    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
};

#endif // GEOMETRYENGINE_H
