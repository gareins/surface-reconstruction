#include "surfacegraph.h"

#include <QtDataVisualization/QValue3DAxis>
#include <QtDataVisualization/Q3DTheme>
#include <QtGui/QImage>
#include <QtCore/qmath.h>

using namespace QtDataVisualization;

SurfaceGraph::SurfaceGraph(Q3DSurface *surface)
    : m_graph(surface)
{
    m_graph->setAxisX(new QValue3DAxis);
    m_graph->setAxisY(new QValue3DAxis);
    m_graph->setAxisZ(new QValue3DAxis);

    m_dataProxy = new QSurfaceDataProxy();
    m_dataSeries = new QSurface3DSeries(m_dataProxy);
    fillDataProxy();
    m_graph->addSeries(m_dataSeries);
}

SurfaceGraph::~SurfaceGraph()
{
    delete m_graph;
}

void SurfaceGraph::fillDataProxy()
{
    QSurfaceDataArray *data = new QSurfaceDataArray;
    QSurfaceDataRow *dataRow1 = new QSurfaceDataRow;
    QSurfaceDataRow *dataRow2 = new QSurfaceDataRow;
    QSurfaceDataRow *dataRow3 = new QSurfaceDataRow;

    *dataRow1 << QVector3D(0.0f, 0.0f, 0.0f) << QVector3D(1.0f, 0.0f, 0.0f) << QVector3D(2.0f, 0.0f, 0.0f);
    *dataRow2 << QVector3D(0.0f, 0.0f, 1.0f) << QVector3D(1.0f, 2.0f, 1.0f) << QVector3D(2.0f, 0.0f, 1.0f);
    *dataRow3 << QVector3D(0.0f, 0.0f, 2.0f) << QVector3D(1.0f, 0.0f, 2.0f) << QVector3D(2.0f, 0.0f, 2.0f);
    *data << dataRow1 << dataRow2 << dataRow3;

    m_dataSeries->dataProxy()->resetArray(data);
}

void SurfaceGraph::update()
{
     printf("UPDATE!\n");
}

/*
void SurfaceGraph::changeTheme(int theme)
{
    m_graph->activeTheme()->setType(Q3DTheme::Theme(theme));
}
*/
