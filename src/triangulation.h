#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include <array>

typedef std::array<double, 3> TPoint;
typedef std::array<int, 3> TTriangle;

typedef std::vector<TPoint > PointList;
typedef std::vector<TTriangle > TriangleList;

// Wrapper...
class Triangulation
{
public:
    enum Mode { alpha_shapes, viterbi, cech };
    Triangulation(): done_(false), mode_(alpha_shapes) {}

    bool set_in_file(std::string pts);
    void set_mode(Mode mode);

    bool calculate();

private:
    bool calc_alphashapes_();
    bool calc_viterbi_();
    bool calc_cech_();

    bool done_;
    PointList pts_;
    PointList orig_pts_;
    std::vector<std::tuple<int, int, int> > triangles_;
    Mode mode_;
};

#endif // TRIANGULATION_H
