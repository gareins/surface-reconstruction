#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include <tuple>

#include "qdebug.h"

#include <topology/simplex.h>
#include <utilities/types.h>

// Alpha shapes...

#include "Dionysus/examples/alphashapes/alphashapes3d.h"

// Wrapper...
class Triangulation
{
public:
    Triangulation();

    static Triangulation AlphaShape(std::vector<std::tuple<double, double, double> > pts);

};

#endif // TRIANGULATION_H
