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

//#include "Miniball_dynamic_d.h"


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

    //qDebug("%f", max);
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

Triangulation::Triangulation(): done_(false), distance_(1), prob_(1), mode_(alpha_shapes), homo_count_(skeleton, 0)
{
}

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

        QStringList  fields = line.right(line.length() - 2).split(QString(", "));
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

    triangles_.clear();
    lines_.clear();
    points_.clear();
    std::fill(homo_count_.begin(), homo_count_.end(), 0);
    done_ = false;

    switch(mode_)
    {
        case alpha_shapes: return calc_alphashapes_();
        case rips:         return calc_rips_();
        //case cech:         return calc_cech_();
        default:           return false;
    }
    done_ = true;
}

/* * /
typedef std::vector<PointMB> PointContainerMB;
typedef unsigned int PointIndex;
typedef Simplex<PointIndex, double> SmplxCh;
typedef Filtration<SmplxCh> CechFiltration;

void add_simplices(CechFiltration& sv, int d, const PointContainerMB& points)
{
    PointIndex indices[d+1];
    for (int i = 0; i < d+1; ++i)
        indices[i] = d - i;

    while(indices[d] < points.size() - d)
    {
        // Add simplex
        Miniball mb(points[indices[0]].dim());
        SmplxCh s;
        for (int i = 0; i < d+1; ++i)
        {
            s.add(indices[i]);
            mb.check_in(points[indices[i]]);
        }
        mb.build();
        s.data() = mb.squared_radius();
        sv.push_back(s);


        // Advance indices
        for (int i = 0; i < d+1; ++i)
        {
            ++indices[i];
            if (indices[i] < points.size() - i)
            {
                for (int j = i-1; j >= 0; --j)
                    indices[j] = indices[j+1] + 1;
                break;
            }
        }
    }
}

bool Triangulation::calc_cech_()
{
    PointContainerMB points;
    for (auto iter = pts_.begin(); iter != pts_.end(); iter++) {
        TPoint tp = *iter;
        PointMB p(3);
        for (int i = 0; i < 3; ++i) {
            p[i] = tp.at(i);
        }
        points.push_back(p);
    }

    int homology_dim = skeleton-1;
    // Compute simplices with their Cech values
//    int num_simplices = 0;
//    for (int i = 0; i <= homology_dim + 1; ++i)
//        num_simplices += choose(pts_.size(), i+1);
    //rInfo("Reserved SimplexVector of size: %d", num_simplices);

    CechFiltration cf;
    for (int i = 0; i <= homology_dim + 1; ++i) {
        add_simplices(cf, i, points);
    }
    rInfo("Size of SimplexVector: %d", cf.size());

    // Sort the filtration
    cf.sort(DataDimensionComparison<SmplxCh>());
    rInfo("Filtration initialized");

    // Compute persistence
    Persistence p(cf);
    rInfo("Persistence initialized");
    p.pair_simplices();
    rInfo("Simplices paired");

    Persistence::SimplexMap<CechFiltration> m = p.make_simplex_map(cf);
    std::map<Dimension, PDgm> dgms;
    init_diagrams(dgms, p.begin(), p.end(),
                  evaluate_through_map(m, SmplxCh::DataEvaluator()),
                  evaluate_through_map(m,  SmplxCh::DimensionExtractor()));

    for(auto iter = p.begin(); iter != p.end(); iter++) {

        if(m[iter].dimension() < 3 )
            continue;

        const std::vector<unsigned int> vtxs = m[iter].vertices();
        if(vtxs.size() == 3) {
            TTriangle to_add = { pts_[vtxs[0]], pts_[vtxs[1]], pts_[vtxs[2]] };
            add_triangle(to_add);
        }
        else if(vtxs.size() == 4) {
            TTriangle to_add1 = { pts_[vtxs[0]], pts_[vtxs[1]], pts_[vtxs[2]] };
            TTriangle to_add2 = { pts_[vtxs[1]], pts_[vtxs[2]], pts_[vtxs[3]] };
            TTriangle to_add3 = { pts_[vtxs[0]], pts_[vtxs[1]], pts_[vtxs[3]] };
            add_triangle(to_add1);
            add_triangle(to_add2);
            add_triangle(to_add3);
        }
    }

//    for (int i = 0; i <= homology_dim; ++i) {
//        std::cout << i << std::endl << dgms[i] << std::endl;
//    }

    QVector<QVector3D> homology;
    for (int i = 0; i <= homology_dim; ++i) {
        for (auto iter = dgms[i].begin(); iter != dgms[i].end(); iter++) {
            //std::cout << (*iter).x() << " " << (*iter).y() << std::endl;
            homology.append({i, (*iter).x(), (*iter).y()});
        }
    }

    // calculate homology
    Q_FOREACH (auto h, homology) homo_count_[h[0]] += h[1] <= distance_ && distance_ <= h[2] ? 1 : 0;

    qDebug() << "Cech homology:" << homo_count_;

    return true;
}
/**/

int Triangulation::calc_euler()
{
    int euler = 0;
    for (int i = 0; i < homo_count_.size(); i++) {
        euler += (pow(-1,i)*homo_count_[i]);
    }
    return euler;
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

    // TODO FIXME
    /*
    QVector<QVector3D> homology;
    for (auto cur = p.begin(); cur != p.end(); ++cur) {
        if (!cur->sign()) {

            const AlphaSimplex3D& b = m[cur];

            if (cur->unpaired()) {
                homology.append({b.dimension(), sqrt(b.value()), std::numeric_limits<double>::max()});
                continue;
            }

            const AlphaSimplex3D& d = m[cur->pair];
            homology.append({b.dimension(), sqrt(b.value()), sqrt(d.value())});
            //homology.append({b.dimension(), sqrt(b.data()[0]), sqrt(d.data()[0]));
        }
    }
    */

    QVector<QVector3D> homology;

    std::map<Dimension, PDgm> dgms;
    init_diagrams(dgms, p.begin(), p.end(),
                  evaluate_through_map(m, AlphaSimplex3D::AlphaValueEvaluator()),
                  evaluate_through_map(m, AlphaSimplex3D::DimensionExtractor()));
    /*
    std::cout << 0 << std::endl << dgms[0] << std::endl;
    std::cout << 1 << std::endl << dgms[1] << std::endl;
    std::cout << 2 << std::endl << dgms[2] << std::endl;
    */

    for (int i = 0; i < skeleton; ++i) {
        //std::cout << i << std::endl;
        for (auto iter = dgms[i].begin(); iter != dgms[i].end(); iter++) {
            //std::cout << (*iter).x() << " " << (*iter).y() << std::endl;

            // TODO wut kr arbitrarno *10?
            homology.append({i, (*iter).x()*10, (*iter).y()*10});
        }
    }

    // calculate homology
    Q_FOREACH (auto h, homology) homo_count_[h[0]] += h[1] <= distance_ && distance_ <= h[2] ? 1 : 0;

    qDebug() << "Alpha shapes finished!";
    return true;
}

typedef         PairwiseDistances<PointContainer, L2Distance>           PairDistances;
typedef         PairDistances::DistanceType                             DistanceType;
typedef         PairDistances::IndexType                                Vertex;

typedef         Rips<PairDistances>                                     Generator;
typedef         Generator::Simplex                                      Smplx;
typedef         Filtration<Smplx>                                       Fltr;
typedef         DynamicPersistenceChains<>                              DynamicPersistence;
typedef         PersistenceDiagram<>                                    PDgm;

bool Triangulation::calc_rips_()
{
    DistanceType            max_distance;

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

    PairDistances           distances(points);
    Generator               rips(distances);
    Generator::Evaluator    size(distances);
    Fltr                    f;

    max_distance = distance_;

    // Generate n-skeleton of the Rips complex
    rips.generate(skeleton, max_distance, make_push_back_functor(f));
    std::cout << "# Generated complex of size: " << f.size() << std::endl;

    // Generate filtration with respect to distance and compute its persistence
    f.sort(Generator::Comparison(distances));

    DynamicPersistence p(f);
    p.pair_simplices();

    // Output cycles
    DynamicPersistence::SimplexMap<Fltr> m = p.make_simplex_map(f);

    for(auto iter = p.begin(); iter != p.end(); iter++) {

        if(m[iter].dimension() == 0 /*|| simplex_width(m[iter]) > distance_ */) continue;

        const std::vector<unsigned int> vtxs = m[iter].vertices();

        if(vtxs.size() == 1)
        {
            TPoint p = pts_[vtxs[0]];
            points_.push_back(QVector3D(p[0], p[1], p[2]));
        }
        else if(vtxs.size() == 2)
        {
            TPoint p1 = pts_[vtxs[0]];
            TPoint p2 = pts_[vtxs[1]];
            lines_.push_back(QVector3D(p1[0], p1[1], p1[2]));
            lines_.push_back(QVector3D(p2[0], p2[1], p2[2]));
        }
        else if(vtxs.size() == 3) {
            TTriangle to_add = { pts_[vtxs[0]], pts_[vtxs[1]], pts_[vtxs[2]] };
            add_triangle(to_add);
        }
        else if(vtxs.size() == 4) {
            TTriangle to_add1 = { pts_[vtxs[0]], pts_[vtxs[1]], pts_[vtxs[2]] };
            TTriangle to_add2 = { pts_[vtxs[1]], pts_[vtxs[2]], pts_[vtxs[3]] };
            TTriangle to_add3 = { pts_[vtxs[0]], pts_[vtxs[1]], pts_[vtxs[3]] };
            add_triangle(to_add1);
            add_triangle(to_add2);
            add_triangle(to_add3);
        }
    }

    QVector<QVector3D> homology;
    for (DynamicPersistence::iterator cur = p.begin(); cur != p.end(); ++cur)
    {
        if (!cur->sign())        // only negative simplices have non-empty cycles
        {
            DynamicPersistence::OrderIndex birth = cur->pair;      // the cycle that cur killed was born when we added birth (another simplex)

            const Smplx& b = m[birth];
            const Smplx& d = m[cur];

            if (b.dimension() >= skeleton) continue;
            homology.append({b.dimension(), size(b), size(d)});
        }
        else if (cur->unpaired())    // positive could be unpaired
        {
            const Smplx& b = m[cur];
            if (b.dimension() >= skeleton) continue;
            homology.append({b.dimension(), size(b), std::numeric_limits<double>::max()});
        }
    }

    // calculate homology
    Q_FOREACH (auto h, homology) homo_count_[h[0]] += h[1] <= distance_ && distance_ <= h[2] ? 1 : 0;

    qDebug() << triangles_.size() << "triangles," << lines_.size() << "lines," << points_.size() << "points";
    qDebug() << "Vietoris-Rips finished!";

    return true;
}

