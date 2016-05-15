#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include <array>
#include <qstring.h>

#include <geometry/l2distance.h>
#include <topology/simplex.h>
#include <topology/filtration.h>

#include <QVector>
#include <QVector3D>

typedef std::array<double, 3> TPoint;
//typedef std::array<TPoint, 2> TPoint;
typedef std::array<TPoint, 3> TTriangle;

typedef std::vector<TPoint> PointList;
typedef std::vector<TTriangle> TriangleList;
typedef std::vector<TPoint> Simplex0List;
typedef std::vector<TPoint> Simplex1List;


// Wrapper...
class Triangulation
{
public:
    enum Mode { alpha_shapes, rips, cech };
    Triangulation();

    bool set_in_file(QString pts);
    void set_mode(Mode mode)                      { mode_ = mode; }
    void set_distance(double distance)            { distance_ = distance; }
    void set_point_filtration(double probability) { prob_ = probability; }

    const TriangleList& get_triangles()           { return triangles_; }
    const QVector<QVector3D> get_lines()          { return lines_; }
    const QVector<QVector3D> get_points()         { return points_; }
    const PointList& get_chosen_pts()             { return pts_; }
    const std::vector<double>& get_homology()     { return homo_count_; }
    int calc_euler();

    bool calculate();

    void add_triangle(TTriangle t) { triangles_.push_back(t); }

private:
    bool calc_alphashapes_();
    bool calc_rips_();
    //bool calc_cech_();

    bool done_;
    double distance_;
    double prob_;
    PointList pts_;
    PointList orig_pts_;
    TriangleList triangles_;
    QVector<QVector3D> lines_;
    QVector<QVector3D> points_;
    Mode mode_;
    Dimension skeleton = 3;
    std::vector<double> homo_count_;
};

#endif // TRIANGULATION_H
