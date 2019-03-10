#ifndef PARAMETERS_H
#define PARAMETERS_H

namespace ou {

struct Parameters {
    Parameters();

    int maxLods;
    int gridSize;
    double smoothingFactor;
    bool renderWireframe;
    double anglePerPixel;
};
}

#endif // PARAMETERS_H
