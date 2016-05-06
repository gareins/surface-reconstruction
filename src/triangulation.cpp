#include "Dionysus/examples/alphashapes/alphashapes3d.h"

#include "triangulation.h"
#include <topology/simplex.h>
#include <utilities/types.h>

#include <topology/filtration.h>
#include <topology/static-persistence.h>
#include <topology/persistence-diagram.h>
#include <iostream>

#include <qdebug.h>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/map.hpp>

// ---------------------------------------------------
// This could go to some utils file
#include <sstream>
#include <vector>

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}
// ---------------------------------------------------

typedef Filtration<AlphaSimplex3D>              AlphaFiltration;
typedef StaticPersistence<>                     Persistence;
typedef PersistenceDiagram<>                    PDgm;

bool Triangulation::set_in_file(std::string infile)
{
    std::ifstream f (infile);
    std::string line;

    if (!f.is_open())
        return false;

    while ( getline (f, line) )
    {
        if(!(line[0] == 'v' && line[1] == ' '))
            continue;

        std::vector<std::string> tmp = split(line, ' ');
        std::array<double, 3> pt = {std::stod(tmp[1]), std::stod(tmp[2]), std::stod(tmp[3])};
        orig_pts_.push_back(pt);
    }

    return true;
}

bool Triangulation::calculate()
{
    std::random_shuffle(orig_pts_.begin(), orig_pts_.end());

    assert(prob_ <= 1 && prob_ > 0);
    pts_ = PointList(orig_pts_.begin(), orig_pts_.begin() + int(orig_pts_.size() * prob_));

    switch(mode_)
    {
    case alpha_shapes: return calc_alphashapes_();
    case rips:         return calc_rips_();
    case cech:         return calc_cech_();
    default:           return false;
    }
}

bool Triangulation::calc_cech_() { return false; }

bool Triangulation::calc_alphashapes_()
{
    // Read in the point set and compute its Delaunay triangulation
    double x,y,z;
    Delaunay3D Dt;

    for(auto iter = pts_.begin(); iter != pts_.end(); iter++)
    {
        TPoint tp = *iter;
        x = tp.at(0); y = tp.at(1); z = tp.at(2);

        Delaunay3D::Point p(x,y,z);
        Dt.insert(p);
    }

    AlphaFiltration  af;
    fill_complex(Dt, af);

    // Create the alpha-shape filtration
    af.sort(AlphaSimplex3D::AlphaOrder());

    Persistence p(af);
    //TODO: can we actually read these stars to track progress??
    p.pair_simplices();

    triangles_.clear();

    Persistence::SimplexMap<AlphaFiltration> m = p.make_simplex_map(af);
    for(auto iter = p.begin(); iter != p.end(); iter++)
    {
        auto vertices = m[iter].vertices();

        //TODO: handle 4D simplexes!!
        if(vertices.size() != 3)
            continue;

        TTriangle trig;
        uint idx = 0;
        for(auto v = vertices.begin(); v != vertices.end(); v++)
        {
            CGAL::Point_3<CGAL::Epeck> pt = (**v).point();

            trig[idx] = {
                    CGAL::to_double(pt.x()),
                    CGAL::to_double(pt.y()),
                    CGAL::to_double(pt.z())
            };
            idx++;
        }

        triangles_.push_back(trig);
    }

    return true;
}

bool Triangulation::calc_rips_()
{
    return false;
}
