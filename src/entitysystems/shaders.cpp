#include "shaders.h"

namespace ou {
const char* hdrVertShaderSrc =
#include "shaders/hdr.vert.glsl"
    ;
const char* hdrFragShaderSrc =
#include "shaders/hdr.frag.glsl"
    ;
const char* planetVertShaderSrc =
#include "shaders/planet.vert.glsl"
    ;
const char* planetFragShaderSrc =
#include "shaders/planet.frag.glsl"
    ;
const char* terrainShaderSrc =
#include "shaders/terrain.comp.glsl"
    ;
const char* terrain2ShaderSrc =
#include "shaders/terrain2.comp.glsl"
    ;
}
