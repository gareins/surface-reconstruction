#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include <array>
#include <qstring.h>

typedef std::array<double, 3> TPoint;
typedef std::array<TPoint, 3> TTriangle;

typedef std::vector<TPoint > PointList;
typedef std::vector<TTriangle > TriangleList;

// Wrapper...
class Triangulation
{
public:
    enum Mode { alpha_shapes, rips, cech };
    Triangulation(): done_(false), distance_(1), prob_(1), mode_(alpha_shapes) {}

    bool set_in_file(QString pts);
    void set_mode(Mode mode)                      { mode_ = mode; }
    void set_distance(double distance)            { distance_ = distance; }
    void set_point_filtration(double probability) { prob_ = probability; }

    const TriangleList& get_triangles()           { return triangles_; }
    const PointList& get_chosen_pts()             { return pts_; }

    bool calculate();

    void add_triangle(TTriangle t) { triangles_.push_back(t); }

private:
    bool calc_alphashapes_();
    bool calc_rips_();
    bool calc_cech_();

    bool done_;
    double distance_;
    double prob_;
    PointList pts_;
    PointList orig_pts_;
    TriangleList triangles_;
    Mode mode_;
};

#endif // TRIANGULATION_H
