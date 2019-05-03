#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <cstdint>

namespace ou {

struct Parameters {
    Parameters();

    int maxLods;
    int gridSize;
    int snapSize;
    int zoomFactor;
    double smoothingFactor;
    bool renderWireframe;
    double anglePerPixel;
    int terrainTextureSize;
    int playerHeight;
    int maxRenderLods;
    int msaaSamples;
    int numPbos;
    int terrainTextureCount;
    std::int64_t rUnit;
};
}

#endif // PARAMETERS_H
