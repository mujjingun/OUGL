#include "planetmath.h"

namespace ou {

glm::i64vec2 eucmod(glm::i64vec2 a, std::int64_t base)
{
    a.x = (a.x < 0 ? ((a.x % base) + base) % base : a.x % base);
    a.y = (a.y < 0 ? ((a.y % base) + base) % base : a.y % base);
    return a;
}

glm::dvec3 applySide(glm::dvec3 const& cube, int side)
{
    switch (side) {
    case 0:
        return { cube.z, cube.x, cube.y };
    case 1:
        return { -cube.z, -cube.x, cube.y };
    case 2:
        return { cube.y, cube.z, cube.x };
    case 3:
        return { cube.y, -cube.z, -cube.x };
    case 4:
        return { cube.x, cube.y, cube.z };
    case 5:
        return { -cube.x, cube.y, -cube.z };
    }

    return {};
}

CubeCoords cubizePoint(glm::dvec3 const& pos)
{
    int side;
    glm::dvec3 absPos = glm::abs(pos);
    glm::dvec2 cube;
    if (absPos.x > absPos.y && absPos.x > absPos.z) {
        if (pos.x > 0) {
            side = 0;
            cube = { pos.y, pos.z };
        } else {
            side = 1;
            cube = { -pos.y, pos.z };
        }
    } else if (absPos.y > absPos.x && absPos.y > absPos.z) {
        if (pos.y > 0) {
            side = 2;
            cube = { pos.z, pos.x };
        } else {
            side = 3;
            cube = { -pos.z, pos.x };
        }
    } else {
        if (pos.z > 0) {
            side = 4;
            cube = { pos.x, pos.y };
        } else {
            side = 5;
            cube = { -pos.x, pos.y };
        }
    }

    const auto sq = cube * cube;
    const double t0 = 2.0 * sq.y - 2.0 * sq.x - 3.0;
    const double u0 = std::sqrt(glm::max(t0 * t0 - 24.0 * sq.x, 0.0));
    const double v0 = 2.0 * sq.x - 2.0 * sq.y;

    const double t1 = 2.0 * sq.x - 2.0 * sq.y - 3.0;
    const double u1 = std::sqrt(glm::max(t1 * t1 - 24.0 * sq.y, 0.0));
    const double v1 = 2.0 * sq.y - 2.0 * sq.x;
    cube = glm::sqrt(glm::max((3.0 - glm::dvec2(u1 + v1, u0 + v0)) / 2.0, 0.0)) * glm::sign(cube);

    return { cube, side };
}

FirstOrderDerivatives derivatives(glm::dvec2 pos, int side)
{
    glm::dvec2 sq = pos * pos;
    glm::dvec3 dx = glm::dvec3{ glm::sqrt(.5 - sq.y / 6.0),
        -pos.x * pos.y / (6.0 * glm::sqrt(.5 - sq.x / 6.0)),
        (-pos.x + 2.0 * pos.x * sq.y / 3.0) / (2.0 * glm::sqrt(1.0 - sq.x / 2.0 - sq.y / 2.0 + sq.x * sq.y / 3.0)) };
    glm::dvec3 dy = glm::dvec3{ -pos.x * pos.y / (6.0 * glm::sqrt(.5 - sq.y / 6.0)),
        glm::sqrt(.5 - sq.x / 6.0),
        (-pos.y + 2.0 * sq.x * pos.y / 3.0) / (2.0 * glm::sqrt(1.0 - sq.x / 2.0 - sq.y / 2.0 + sq.x * sq.y / 3.0)) };
    return { applySide(dx, side), applySide(dy, side) };
}

SecondOrderDerivatives curvature(glm::dvec2 pos, int side)
{
    glm::dvec2 sq = pos * pos;
    double tx = glm::sqrt(.5 - sq.x / 6.0);
    double ty = glm::sqrt(.5 - sq.y / 6.0);
    double tt = glm::sqrt(1.0 - sq.x / 2.0 - sq.y / 2.0 + sq.x * sq.y / 3.0);
    double t0 = -pos.x + 2 * pos.x * sq.y / 3.0;
    glm::dvec3 fxx = glm::dvec3{ 0,
        -sq.x * pos.y / 36.0 * tx * tx * tx - pos.y / (6.0 * tx),
        -t0 * t0 / (4.0 * glm::pow(1.0 - sq.x / 2.0 - sq.y / 2.0 + sq.x * sq.y / 3.0, 1.5))
            + (-1.0 + 2.0 * sq.y / 3.0) / (2.0 * tt) };

    glm::dvec3 fxy = glm::dvec3{
        -pos.y / (6.0 * ty), -pos.x / (6.0 * tx),
        -(pos.y + 2.0 * sq.x * pos.y / 3.0) * (pos.x + 2.0 * sq.y * pos.x / 3.0) / (4.0 * tt * tt * tt)
            + 2.0 * pos.x * pos.y / (3.0 * tt)
    };

    double t1 = -pos.y + 2 * pos.y * sq.x / 3.0;
    glm::dvec3 fyy = glm::dvec3{
        -sq.y * pos.x / 36.0 * ty * ty * ty - pos.x / (6.0 * ty),
        0,
        -t1 * t1 / (4.0 * glm::pow(1.0 - sq.x / 2.0 - sq.y / 2.0 + sq.x * sq.y / 3.0, 1.5))
            + (-1.0 + 2.0 * sq.x / 3.0) / (2.0 * tt)
    };

    return { applySide(fxx, side), applySide(fxy, side), applySide(fyy, side) };
}
}
