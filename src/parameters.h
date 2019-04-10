#ifndef PARAMETERS_H
#define PARAMETERS_H

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
    int terrainTextureCount;
    int playerHeight;
    int maxRenderLods;
};
}

#endif // PARAMETERS_H
