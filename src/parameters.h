#ifndef PARAMETERS_H
#define PARAMETERS_H

namespace ou {

struct Parameters {
    Parameters();

    int maxLods;
    int gridSize;
    int snapSize;
    double smoothingFactor;
    bool renderWireframe;
    double anglePerPixel;
    int terrainTextureSize;
    int terrainTextureCount;
    int playerHeight;
};
}

#endif // PARAMETERS_H
