#ifndef PLANETMATH_H
#define PLANETMATH_H

#include <glm/glm.hpp>

namespace ou {

glm::i64vec2 eucmod(glm::i64vec2 a, std::int64_t base);
glm::dvec3 applySide(glm::dvec3 const& cube, int side);

struct CubeCoords {
    glm::dvec2 pos;
    int side;
};
CubeCoords cubizePoint(glm::dvec3 const& pos);

struct FirstOrderDerivatives {
    glm::dvec3 fx, fy;
};
FirstOrderDerivatives derivatives(glm::dvec2 pos, int side);

struct SecondOrderDerivatives {
    glm::dvec3 fxx, fxy, fyy;
};
SecondOrderDerivatives curvature(glm::dvec2 pos, int side);
}

#endif // PLANETMATH_H
