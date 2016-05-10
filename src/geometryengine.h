#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include "triangulation.h"

struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
};

class GeometryEngine : protected QOpenGLFunctions
{
public:
    GeometryEngine();
    GeometryEngine(TriangleList triangles);
    virtual ~GeometryEngine();

    void drawGeometry(QOpenGLShaderProgram *program);
    bool isTransparent;

private:
    void initGeometry(TriangleList triangles = {});
    GLuint indexOf(std::vector<VertexData>, VertexData);

    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
    uint idxLen;
};

#endif // GEOMETRYENGINE_H
