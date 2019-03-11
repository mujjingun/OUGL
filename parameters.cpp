#include "parameters.h"

namespace ou {

Parameters::Parameters()
    : maxLods(30)
    , gridSize(64)
    , smoothingFactor(15)
    , renderWireframe(false)
    , anglePerPixel(0.5)
    , terrainTextureSize(1024)
    , terrainTextureCount(30)
{
}
}
