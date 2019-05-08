#include "shaders.h"

namespace ou {
const char* const quadVertShaderSrc =
#include "shaders/quad.vert.glsl"
    ;
const char* const hdrFragShaderSrc =
#include "shaders/hdr.frag.glsl"
    ;
const char* const skyFromSpaceVertShaderSrc =
#include "shaders/skyfromspace.vert.glsl"
    ;
const char* const skyFromSpaceFragShaderSrc =
#include "shaders/skyfromspace.frag.glsl"
    ;
const char* const planetVertShaderSrc =
#include "shaders/planet.vert.glsl"
    ;
const char* const planetFragShaderSrc =
#include "shaders/planet.frag.glsl"
    ;
const char* const terrainShaderSrc =
#include "shaders/terrain.comp.glsl"
    ;
const char* const terrain2ShaderSrc =
#include "shaders/terrain2.comp.glsl"
    ;
}
