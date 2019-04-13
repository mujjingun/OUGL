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
    int playerHeight;
    int maxRenderLods;
    int msaaSamples;
    int numPbos;
    int terrainTextureCount;
};
}

#endif // PARAMETERS_H
