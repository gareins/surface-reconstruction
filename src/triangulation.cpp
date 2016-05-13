#include "../../Dionysus/examples/alphashapes/alphashapes3d.h"

#include "triangulation.h"
#include <topology/simplex.h>
#include <utilities/types.h>

#include <topology/rips.h>
#include <topology/filtration.h>
#include <topology/static-persistence.h>
#include <topology/dynamic-persistence.h>
#include <topology/persistence-diagram.h>
#include <iostream>

#include <geometry/l2distance.h>
#include <geometry/distances.h>

#include <qdebug.h>
#include <qtextstream.h>
#include <qfile.h>

#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/map.hpp>

#include <QVector3D>

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

double simplex_width(const AlphaSimplex3D simplex)
{
    double max = 0.0;
    const uint dim = simplex.dimension();

    for(uint i = 0; i <= dim; i++)
        for(uint j = i + 1; j <= dim; j++)
        {
            double x1 = CGAL::to_double((*simplex.vertices()[i]).point().x());
            double y1 = CGAL::to_double((*simplex.vertices()[i]).point().y());
            double z1 = CGAL::to_double((*simplex.vertices()[i]).point().z());
            double x2 = CGAL::to_double((*simplex.vertices()[j]).point().x());
            double y2 = CGAL::to_double((*simplex.vertices()[j]).point().y());
            double z2 = CGAL::to_double((*simplex.vertices()[j]).point().z());

            double d = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2);
            max = d > max ? d : max;
        }

    qDebug("%f", max);
    return max;
}

void add_trig(Triangulation* t, CGAL::Point_3<CGAL::Epeck> pt1, CGAL::Point_3<CGAL::Epeck> pt2, CGAL::Point_3<CGAL::Epeck> pt3)
{
    TPoint p1 = { CGAL::to_double(pt1.x()), CGAL::to_double(pt1.y()), CGAL::to_double(pt1.z()) };
    TPoint p2 = { CGAL::to_double(pt2.x()), CGAL::to_double(pt2.y()), CGAL::to_double(pt2.z()) };
    TPoint p3 = { CGAL::to_double(pt3.x()), CGAL::to_double(pt3.y()), CGAL::to_double(pt3.z()) };
    TTriangle to_add = { p1, p2, p3 };
    t->add_triangle(to_add);
}

// ---------------------------------------------------

typedef Filtration<AlphaSimplex3D>              AlphaFiltration;
typedef StaticPersistence<>                     Persistence;
typedef PersistenceDiagram<>                    PDgm;

bool Triangulation::set_in_file(QString infile)
{
    QFile file(infile);

    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug("Bad file!");
        return false;
    }

    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString line = in.readLine();
        if(!(line[0] == 'v' && line[1] == ' '))
            continue;

        QStringList  fields = line.left(line.length() - 2).split(",");
        double x = fields[0].toDouble();
        double y = fields[1].toDouble();
        double z = fields[2].toDouble();

        TPoint pt = {x, y, z};
        orig_pts_.push_back(pt);
    }

    qDebug("Good file :)");
    return true;
}

bool Triangulation::calculate()
{
    std::random_shuffle(orig_pts_.begin(), orig_pts_.end());

    assert(prob_ <= 1 && prob_ > 0);
    pts_ = PointList(orig_pts_.begin(), orig_pts_.begin() + int(orig_pts_.size() * prob_));

    done_ = false;
    switch(mode_)
    {
    case alpha_shapes: return calc_alphashapes_();
    case rips:         return calc_rips_();
    case cech:         return calc_cech_();
    default:           return false;
    }
    done_ = true;
}

bool Triangulation::calc_cech_() { return false; }

int Triangulation::calc_euler()
{

}

int Triangulation::calc_homology()
{

}

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
        if(m[iter].dimension() < 3 || simplex_width(m[iter]) > distance_ )
            continue;

        const AlphaSimplex3D::VertexContainer& vertices = m[iter].vertices();

        if(vertices.size() == 3)
        {
            add_trig(this, (*vertices[0]).point(), (*vertices[1]).point(), (*vertices[2]).point());
        }

        else if(vertices.size() == 4)
        {
            add_trig(this, (*vertices[0]).point(), (*vertices[1]).point(), (*vertices[2]).point());
            add_trig(this, (*vertices[0]).point(), (*vertices[1]).point(), (*vertices[3]).point());
            add_trig(this, (*vertices[0]).point(), (*vertices[2]).point(), (*vertices[3]).point());
            add_trig(this, (*vertices[1]).point(), (*vertices[2]).point(), (*vertices[3]).point());
        }
    }
    qDebug() << "Alpha shapes finished!";
    return true;
}

typedef         PairwiseDistances<PointContainer, L2Distance>           PairDistances;
typedef         PairDistances::DistanceType                             DistanceType;
typedef         PairDistances::IndexType                                Vertex;

typedef         Rips<PairDistances>                                     Generator;
typedef         Generator::Simplex                                      Smplx;
typedef         Filtration<Smplx>                                       Fltr;
// typedef         StaticPersistence<>                                     Persistence;
typedef         DynamicPersistenceChains<>                              DynamicPersistence;
typedef         PersistenceDiagram<>                                    PDgm;

bool Triangulation::calc_rips_()
{
    Dimension               skeleton;
    DistanceType            max_distance;
    std::string             infilename, diagram_name;
    diagram_name = "vietoris_diagramus";

    //program_options(argc, argv, infilename, skeleton, max_distance, diagram_name);
    std::ofstream           diagram_out(diagram_name.c_str());
    QVector<QVector3D> homology;
    std::cout << "Diagram:         " << diagram_name << std::endl;

    PointContainer          points;

    for(auto iter = pts_.begin(); iter != pts_.end(); iter++)
    {
        points.push_back(Point());

        TPoint tp = *iter;
        //x = tp.at(0); y = tp.at(1); z = tp.at(2);

        points.back().push_back(tp.at(0));
        points.back().push_back(tp.at(1));
        points.back().push_back(tp.at(2));
    }

    //infilename = "./dataset/sphere_100.obj";
    //read_points_beta(infilename, points);

    PairDistances           distances(points);
    Generator               rips(distances);
    Generator::Evaluator    size(distances);
    Fltr                    f;

    //Dimension(&skeleton)->default_value(2);   //"Dimension of the Rips complex we want to compute")
    skeleton = 3;
    //max_distance = Infinity;
    max_distance = distance_;

    //DistanceType>(&max_distance)->default_value(Infinity);    //"Maximum value for the Rips complex construction")
    //std::string>(&diagram_name);                              //"Filename where to output the persistence diagram");

    //qDebug() << "do sm smo prsli";

    // Generate 2-skeleton of the Rips complex for epsilon = 50
    rips.generate(skeleton, max_distance, make_push_back_functor(f));
    std::cout << "# Generated complex of size: " << f.size() << std::endl;

    //qDebug() << "do sm smo prsli 2";

    // Generate filtration with respect to distance and compute its persistence
    f.sort(Generator::Comparison(distances));

    //Timer persistence_timer; persistence_timer.start();
    DynamicPersistence p(f);
    p.pair_simplices();
    //persistence_timer.stop();

#if 1
    // Output cycles
    DynamicPersistence::SimplexMap<Fltr>   m = p.make_simplex_map(f);

/*
    //Persistence::SimplexMap<AlphaFiltration> m = p.make_simplex_map(af);
    for(auto iter = p.begin(); iter != p.end(); iter++)
    {
        if(m[iter].dimension() < 3 || simplex_width(m[iter]) > distance_ )
            continue;

        const AlphaSimplex3D::VertexContainer& vertices = m[iter].vertices();

        if(vertices.size() == 3)
        {
            add_trig(this, (*vertices[0]).point(), (*vertices[1]).point(), (*vertices[2]).point());
        }

        else if(vertices.size() == 4)
        {
            add_trig(this, (*vertices[0]).point(), (*vertices[1]).point(), (*vertices[2]).point());
            add_trig(this, (*vertices[0]).point(), (*vertices[1]).point(), (*vertices[3]).point());
            add_trig(this, (*vertices[0]).point(), (*vertices[2]).point(), (*vertices[3]).point());
            add_trig(this, (*vertices[1]).point(), (*vertices[2]).point(), (*vertices[3]).point());
        }
    }
*/


    for (DynamicPersistence::iterator cur = p.begin(); cur != p.end(); ++cur)
    {
        const DynamicPersistence::Cycle& cycle = cur->cycle;

        if (!cur->sign())        // only negative simplices have non-empty cycles
        {
            DynamicPersistence::OrderIndex birth = cur->pair;      // the cycle that cur killed was born when we added birth (another simplex)

            const Smplx& b = m[birth];
            const Smplx& d = m[cur];

            // if (b.dimension() != 1) continue;
            // std::cout << "Pair: (" << size(b) << ", " << size(d) << ")" << std::endl;
            if (b.dimension() >= skeleton) continue;
            diagram_out << b.dimension() << " " << size(b) << " " << size(d) << std::endl;
            homology.append({b.dimension(), size(b), size(d)});
        }
        else if (cur->unpaired())    // positive could be unpaired
        {
            const Smplx& b = m[cur];
            // if (b.dimension() != 1) continue;

            // std::cout << "Unpaired birth: " << size(b) << std::endl;
            // cycle = cur->chain;      // TODO
            if (b.dimension() >= skeleton) continue;
            diagram_out << b.dimension() << " " << size(b) << " inf" << std::endl;
            homology.append({b.dimension(), size(b), std::numeric_limits<double>::max()});
        }

        // Iterate over the cycle
        // for (Persistence::Cycle::const_iterator si =  cycle.begin();
        //                                                          si != cycle.end();     ++si)
        // {
        //     const Smplx& s = m[*si];
        //     //std::cout << s.dimension() << std::endl;
        //     const Smplx::VertexContainer& vertices = s.vertices();          // std::vector<Vertex> where Vertex = Distances::IndexType
        //     AssertMsg(vertices.size() == s.dimension() + 1, "dimension of a simplex is one less than the number of its vertices");
        //     std::cout << vertices[0] << " " << vertices[1] << std::endl;
        // }
    }
#endif

    //persistence_timer.check("# Persistence timer");

    QVector<double> homo_count(skeleton, 0);

    // calculate homology
    Q_FOREACH (auto h, homology) homo_count[h[0]] += h[1] <= distance_ && distance_ <= h[2] ? 1 : 0;

    qDebug() << "Rips homology:" << homo_count;
    return true;
}

