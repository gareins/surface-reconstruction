#include "triangulation.h"

#include <topology/filtration.h>
#include <topology/static-persistence.h>
#include <topology/persistence-diagram.h>
#include <iostream>

#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/map.hpp>

typedef Filtration<AlphaSimplex3D>              AlphaFiltration;
typedef StaticPersistence<>                     Persistence;
typedef PersistenceDiagram<>                    PDgm;

Triangulation::Triangulation()
{

}

Triangulation Triangulation::AlphaShape(std::vector<std::tuple<double, double, double> > pts)
{
    // Read in the point set and compute its Delaunay triangulation
    double x,y,z;
    Delaunay3D Dt;

    for(auto iter = pts.begin(); iter != pts.end(); iter++)
    {
        std::tie(x, y, z) < *iter;

        Delaunay3D::Point p(x,y,z);
        Dt.insert(p);
    }
    qDebug("Delaunay triangulation computed");

    AlphaFiltration  af;
    fill_complex(Dt, af);
    rInfo("Simplices: %d", af.size());

    // Create the alpha-shape filtration
    af.sort(AlphaSimplex3D::AlphaOrder());
    rInfo("Filtration initialized");

    Persistence p(af);
    rInfo("Persistence initializaed");

    p.pair_simplices();

    Persistence::SimplexMap<AlphaFiltration>    m       = p.make_simplex_map(af);
    std::map<Dimension, PDgm> dgms;
    init_diagrams(dgms, p.begin(), p.end(),
                  evaluate_through_map(m, AlphaSimplex3D::AlphaValueEvaluator()),
                  evaluate_through_map(m, AlphaSimplex3D::DimensionExtractor()));
}
