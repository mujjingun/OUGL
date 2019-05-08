#include "parameters.h"

#define LOD_DEBUG 0

namespace ou {

Parameters::Parameters()
    : maxLods(30)
    , gridSize(64)
    , snapSize(4)
#if LOD_DEBUG
    , zoomFactor(2)
#else
    , zoomFactor(-1)
#endif
    , smoothingFactor(15)
#if LOD_DEBUG
    , renderWireframe(true)
#else
    , renderWireframe(false)
#endif
    , anglePerPixel(0.5)
    , terrainTextureSize(1024)
    , playerHeight(1000)
    , maxRenderLods(15)
    , msaaSamples(1)
    , numPbos(4)
    , terrainTextureCount(maxLods + 6)
    , rUnit(6371000000000)
    , numLats(10)
    , numLons(10)
{
}

}
