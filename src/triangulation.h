#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include <tuple>

#include "qdebug.h"

#include <topology/simplex.h>
#include <utilities/types.h>

// Alpha shapes...

#include "Dionysus/examples/alphashapes/alphashapes3d.h"

typedef std::vector<std::tuple<double, double, double> > PointList;
typedef std::vector<std::tuple<int, int, int> > TriangleList;

// Wrapper...
class Triangulation
{
public:
    enum Mode { alpha_shapes, viterbi, cech };
    Triangulation(): done_(false), mode_(alpha_shapes) {}

    void set_points(PointList pts);
    void set_mode(Mode mode);

    bool calculate();

private:
    bool calc_alphashapes_();
    bool calc_viterbi_();
    bool calc_cech_();

    bool done_;
    PointList pts_;
    std::vector<std::tuple<int, int, int> > triangles_;
    Mode mode_;
};

#endif // TRIANGULATION_H
