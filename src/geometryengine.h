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
    GeometryEngine(TriangleList triangles, QVector<QVector3D> lines, QVector<QVector3D> points);
    virtual ~GeometryEngine();

    void drawGeometry(QOpenGLShaderProgram *program);
    void drawLineGeometry(QOpenGLShaderProgram *program);
    void drawPointGeometry(QOpenGLShaderProgram *program);
    bool isTransparent;

private:
    void initGeometry(TriangleList triangles = {}, QVector<QVector3D> lines = {}, QVector<QVector3D> points = {});
    GLuint indexOf(std::vector<VertexData>, VertexData);

    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
    QOpenGLBuffer arrayBufLines;
    QOpenGLBuffer indexBufLines;
    QOpenGLBuffer arrayBufPoints;
    QOpenGLBuffer indexBufPoints;
    uint idxLen;
    uint idxLenLines;
    uint idxLenPoints;
};

#endif // GEOMETRYENGINE_H
