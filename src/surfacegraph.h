#ifndef SURFACEGRAPH_H
#define SURFACEGRAPH_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtWidgets/QSlider>

using namespace QtDataVisualization;

class SurfaceGraph : public QObject
{
    Q_OBJECT
public:
    explicit SurfaceGraph(Q3DSurface *surface);
    ~SurfaceGraph();
    void update();

/*
public Q_SLOTS:
    void changeTheme(int theme);
*/

private:
    Q3DSurface *m_graph;
    QSurfaceDataProxy *m_dataProxy;
    QSurface3DSeries *m_dataSeries;

    void fillDataProxy();
};

#endif // SURFACEGRAPH_H
