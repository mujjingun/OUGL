#include "planet.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "parameters.h"
#include "player.h"
#include "scene.h"
#include "shader.h"

namespace ou {

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

struct CubeCoords {
    glm::dvec2 pos;
    int side;
};

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
    const double u0 = std::sqrt(std::max(t0 * t0 - 24.0 * sq.x, 0.0));
    const double v0 = 2.0 * sq.x - 2.0 * sq.y;

    const double t1 = 2.0 * sq.x - 2.0 * sq.y - 3.0;
    const double u1 = std::sqrt(std::max(t1 * t1 - 24.0 * sq.y, 0.0));
    const double v1 = 2.0 * sq.y - 2.0 * sq.x;
    cube = glm::sqrt(glm::max((3.0 - glm::dvec2(u1 + v1, u0 + v0)) / 2.0, 0.0)) * glm::sign(cube);

    return { cube, side };
}

std::pair<glm::dvec3, glm::dvec3> derivatives(glm::dvec2 pos, int side)
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

struct SecondOrderDerivatives {
    glm::dvec3 fxx, fxy, fyy;
};

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

struct InstanceAttrib {
    glm::vec2 offset;
    float side;
    float scale;
};

VoxelCoords Planet::position() const
{
    return m_position;
}

std::int64_t Planet::planetRadius() const
{
    return m_planetRadius;
}

Shader &Planet::shader()
{
    static Shader shader("shaders/planet.vert.glsl", "shaders/planet.frag.glsl");
    return shader;
}

Planet::Planet(Scene const* scene)
    : Planet(scene, 6371000000000, {})
{
}

Planet::Planet(Scene const* scene, int64_t planetRadius, VoxelCoords position)
    : m_scene(scene)
    , m_planetRadius(planetRadius)
    , m_position(position)
    , m_vao()
    , m_buf()
    , m_instanceAttrBuf()
{
    std::vector<glm::vec2> gridPoints;
    for (float col = 0; col < scene->params().gridSize; ++col) {
        for (float row = 0; row < scene->params().gridSize; ++row) {
            gridPoints.push_back({ row, col });
            gridPoints.push_back({ row + 1, col });
            gridPoints.push_back({ row + 1, col + 1 });

            gridPoints.push_back({ row, col });
            gridPoints.push_back({ row + 1, col + 1 });
            gridPoints.push_back({ row, col + 1 });
        }
    }

    for (auto& point : gridPoints) {
        point = point / static_cast<float>(scene->params().gridSize) * 2.f - 1.f;
    }

    m_vertexCount = gridPoints.size();

    // Create vertex buffer and fill it
    m_buf.setData(gridPoints, GL_STATIC_DRAW);

    // Bind vertex buffer to vao binding position 0
    VertexArray::BufferBinding vertexBinding = m_vao.getBinding(0);
    vertexBinding.bindVertexBuffer(m_buf, 0, sizeof(glm::vec2));

    // Enable binding position 0
    VertexArray::Attribute posAttr = m_vao.enableVertexAttrib(0);

    // Set format of binding position 0
    posAttr.setFormat(2, GL_FLOAT, GL_FALSE, 0);
    posAttr.setBinding(vertexBinding);

    // Bind instance buffer to vao binding position 1
    VertexArray::BufferBinding instanceBinding = m_vao.getBinding(1);
    instanceBinding.bindVertexBuffer(m_instanceAttrBuf, 0, sizeof(InstanceAttrib));
    instanceBinding.setBindingDivisor(1);

    // Enable binding position 1 and 2
    VertexArray::Attribute offsetAttr = m_vao.enableVertexAttrib(1);
    offsetAttr.setFormat(2, GL_FLOAT, GL_FALSE, offsetof(InstanceAttrib, offset));
    offsetAttr.setBinding(instanceBinding);

    VertexArray::Attribute sideAttr = m_vao.enableVertexAttrib(2);
    sideAttr.setFormat(1, GL_FLOAT, GL_FALSE, offsetof(InstanceAttrib, side));
    sideAttr.setBinding(instanceBinding);

    VertexArray::Attribute scaleAttr = m_vao.enableVertexAttrib(3);
    scaleAttr.setFormat(1, GL_FLOAT, GL_FALSE, offsetof(InstanceAttrib, scale));
    scaleAttr.setBinding(instanceBinding);
}

void Planet::render()
{
    shader().use();
    m_vao.use();

    VoxelCoords centeredPos = m_scene->player().position() - m_position;
    if (centeredPos.voxel != glm::i64vec3()) {
        // planet is more than a voxel away; abort rendering
        return;
    }

    glm::dvec3 normPos = glm::normalize(glm::dvec3(centeredPos.pos));
    auto cubeCoords = cubizePoint(normPos);
    auto derivs = derivatives(cubeCoords.pos, cubeCoords.side);
    auto curvs = curvature(cubeCoords.pos, cubeCoords.side);

    glm::i64vec3 surfacePos = normPos * static_cast<double>(m_planetRadius);
    glm::i64vec3 surfaceOffset = centeredPos.pos - surfacePos;
    glm::dvec3 surfaceOffsetInRadiusUnit = glm::dvec3(surfaceOffset) / static_cast<double>(m_planetRadius);

    // set instance buffer data
    std::vector<InstanceAttrib> instanceBuffer;

    double distance = glm::length(glm::dvec3(centeredPos.pos - surfacePos));
    double normalizedDistance = distance / static_cast<double>(m_planetRadius);

    int levelsOfDetail = glm::clamp(2 - std::ilogb(normalizedDistance), 1, m_scene->params().maxLods);
    for (int lod = 0; lod < levelsOfDetail; ++lod) {
        float side = cubeCoords.side;
        float scale = glm::pow(.5, lod);
        double mod = scale * 2. / static_cast<double>(m_scene->params().gridSize);
        glm::vec2 offset = glm::mod(cubeCoords.pos, mod);
        instanceBuffer.push_back({ -offset, side, scale });
    }

    for (int i = 0; i < 6; ++i) {
        if (i != cubeCoords.side) {
            instanceBuffer.push_back({ { 0, 0 }, static_cast<float>(i), 1 });
        }
    }

    //std::cout << std::fixed << distance / 1000.0 << "m" << std::endl;

    m_instanceAttrBuf.setData(instanceBuffer, GL_STATIC_DRAW);

    // view matrix
    glm::dmat4 viewMat = glm::lookAt(surfaceOffsetInRadiusUnit,
            surfaceOffsetInRadiusUnit + m_scene->player().lookDirection(),
            m_scene->player().upDirection());

    // projection matrix
    double aspectRatio = static_cast<double>(m_scene->windowWidth()) / m_scene->windowHeight();
    glm::dmat4 projMat = glm::perspective(glm::radians(45.0), aspectRatio, 0.1, 10.0);

    // view-projection matrix
    shader().setUniform(0, projMat * viewMat);

    // origin
    shader().setUniform(1, glm::vec2(cubeCoords.pos));

    // xJac
    shader().setUniform(2, glm::vec3(derivs.first));

    // yJac
    shader().setUniform(3, glm::vec3(derivs.second));

    // xxCurv
    shader().setUniform(4, glm::vec3(curvs.fxx));

    // xyCurv
    shader().setUniform(5, glm::vec3(curvs.fxy));

    // yyCurv
    shader().setUniform(6, glm::vec3(curvs.fyy));

    // playerSide
    shader().setUniform(7, cubeCoords.side);

    // levelsOfDetail
    shader().setUniform(8, static_cast<float>(glm::pow(0.5, levelsOfDetail - 1)));

    glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertexCount, instanceBuffer.size());
}
}
