#include "parameters.h"

namespace ou {

Parameters::Parameters()
    : maxLods(30)
    , gridSize(128)
    , snapSize(4)
    , smoothingFactor(15)
    , renderWireframe(true)
    , anglePerPixel(0.5)
    , terrainTextureSize(1024)
    , terrainTextureCount(maxLods + 6)
    , playerHeight(1000)
{
}
}
