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

void Triangulation::set_in_file(QString infile)
{
    pts_ = pts;
}

void Triangulation::set_mode(Mode mode)
{
    mode_ = mode;
}

bool Triangulation::calculate()
{
    switch(mode_)
    {
    case alpha_shapes: return calc_alphashapes_();
    case viterbi:      return calc_viterbi_();
    case cech:         return calc_cech_();
    default:           return false;
    }
}

bool Triangulation::calc_viterbi_() { return false; }
bool Triangulation::calc_cech_() { return false; }

bool Triangulation::calc_alphashapes_()
{
    // Read in the point set and compute its Delaunay triangulation
    double x,y,z;
    Delaunay3D Dt;

    for(auto iter = pts_.begin(); iter != pts_.end(); iter++)
    {
        std::tie(x, y, z) < *iter;

        Delaunay3D::Point p(x,y,z);
        Dt.insert(p);
    }
    qDebug("Delaunay triangulation computed");

    AlphaFiltration  af;
    fill_complex(Dt, af);

    // Create the alpha-shape filtration
    af.sort(AlphaSimplex3D::AlphaOrder());
    qDebug("Filtration initialized");

    Persistence p(af);
    qDebug("Persistence initializaed");

    p.pair_simplices();
    return true;
}
