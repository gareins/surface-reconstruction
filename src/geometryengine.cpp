#include "geometryengine.h"

#include <QVector2D>
#include <QVector3D>
#include <iostream>

#define DEBUG 0
#define TWO_SIDED 1
#define ENABLE_TRANSPARENT 1

GeometryEngine::GeometryEngine()
    : indexBuf(QOpenGLBuffer::IndexBuffer)
{
    initGeometry();
}

GeometryEngine::GeometryEngine(TriangleList triangles, QVector<QVector3D> lines, QVector<QVector3D> points)
    : indexBuf(QOpenGLBuffer::IndexBuffer)
{
    initGeometry(triangles, lines, points);
}

GeometryEngine::~GeometryEngine()
{
    arrayBuf.destroy();
    indexBuf.destroy();
}

void GeometryEngine::initGeometry(TriangleList triangles, QVector<QVector3D> lines, QVector<QVector3D> points)
{
    isTransparent = false;

    initializeOpenGLFunctions();

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Generate 2 VBOs
    arrayBuf.create();
    indexBuf.create();
    arrayBufLines.create();
    arrayBufPoints.create();

    std::vector<VertexData> vertices;
    std::vector<GLuint> indices;
    std::vector<GLuint> tmp(3);

    // iterate over all triangles
    for (uint i = 0; i < triangles.size(); i++) {
        // iterate over all vertices in triangle
        for (int j = 0; j < 3; j++) {
            // create vertex with texture coords
            VertexData vertex = {
                QVector3D(triangles[i][j][0], triangles[i][j][1], triangles[i][j][2]),
                QVector2D(abs(((j+1)%2)-1), abs(((j+1)%3)-1)) // tex coords (0,0),(1,1),(0,1)
            };

            // is vertex already in indices?
            GLuint idx = indexOf(vertices, vertex);
            if (idx >= vertices.size()) {
                // no, add it to the end of the list
                //idx = vertices.size();
                vertices.push_back(vertex);
            }
            // prime the index of current vertex for insertion
            tmp[j] = idx;
        }
        // insert vertex indices of current triangle
        indices.insert(indices.end(), tmp.begin(), tmp.end());

        // render both sides of triangles, for now as a #define option
#if TWO_SIDED
        std::reverse(tmp.begin(), tmp.end());
        indices.insert(indices.end(), tmp.begin(), tmp.end());
#endif
    }

#if DEBUG
    std::cout << "Vertices:" << std::endl << vertices.size() << std::endl;
    for (auto i = vertices.begin(); i != vertices.end(); ++i)
        std::cout << "(" << (*i).position[0] <<"," << (*i).position[1] <<"," << (*i).position[2] <<")" << ' ';
    std::cout << std::endl;

    std::cout << "Indices:" << std::endl << indices.size() << std::endl;
    for (auto i = indices.begin(); i != indices.end(); ++i)
        std::cout << *i << ' ';
    std::cout << std::endl;
#endif

    arrayBuf.bind();
    arrayBuf.allocate(&vertices[0], vertices.size() * sizeof(VertexData));

    indexBuf.bind();
    indexBuf.allocate(&indices[0], indices.size() * sizeof(GLuint));

    idxLen = indices.size();

    if (!lines.empty())
    {
        arrayBufLines.bind();
        arrayBufLines.allocate(&lines[0], lines.size() * sizeof(QVector3D));
    }

    if (!points.empty())
    {
        arrayBufPoints.bind();
        arrayBufPoints.allocate(&points[0], points.size() * sizeof(QVector3D));
    }
}

void GeometryEngine::drawGeometry(QOpenGLShaderProgram *program)
{
    // Tell OpenGL which VBOs to use
    arrayBuf.bind();
    indexBuf.bind();

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    if (isTransparent) {
        glEnable (GL_BLEND);
    }
    else {
        glDisable(GL_BLEND);
    }

    // Draw geometry using indices from VBO 1
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, idxLen, GL_UNSIGNED_INT, 0);
}

void GeometryEngine::drawLineGeometry(QOpenGLShaderProgram *program)
{
    arrayBufLines.bind();

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    /*if (isTransparent) {
        glEnable (GL_BLEND);
    }
    else {
        glDisable(GL_BLEND);
    }*/

    // lines and line color
    program->setUniformValue("wireframe", true);
    program->setUniformValue("wire_color", QVector4D(0,1,0,1));

    // draw lines
    int count = (int)arrayBufLines.size() / sizeof(QVector3D);
    for (int i = 0; i < count; i += 2)
        glDrawArrays(GL_LINES, i, 2);

    // turn off the custom coloring
    program->setUniformValue("wireframe", false);
}

void GeometryEngine::drawPointGeometry(QOpenGLShaderProgram *program)
{
    // points and point color
    program->setUniformValue("wireframe", true);
    program->setUniformValue("wire_color", QVector4D(0,1,0,1));

    // draw points
    // TODO

    // turn off the custom coloring
    program->setUniformValue("wireframe", false);
}

GLuint GeometryEngine::indexOf(std::vector<VertexData> verts, VertexData v) {
    for (uint i = 0; i < verts.size(); i++){
        // TODO? possible to do fuzzy comparison for floating points
        if (verts[i].position == v.position) {
        //if (qFuzzyCompare(verts[i].position, v.position)) {
            return i;
        }
    }
    return verts.size();
}
