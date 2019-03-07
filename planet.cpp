#include "planet.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "player.h"
#include "scene.h"

namespace ou {

Planet::Planet()
    : m_vao()
    , m_buf()
    , m_instanceAttrBuf()
    , m_shader("shaders/planet.vert.glsl", "shaders/planet.frag.glsl")
{
    // Enable binding position 0
    VertexArray::Attribute posAttr = m_vao.enableVertexAttrib(0);

    // Set format of binding position 0
    posAttr.setFormat(2, GL_FLOAT, GL_FALSE, 0);

    const std::size_t GRID_SIZE = 10;
    std::vector<glm::vec2> gridPoints;
    for (float col = 0; col < GRID_SIZE; ++col) {
        for (float row = 0; row < GRID_SIZE; ++row) {
            gridPoints.push_back({ row, col });
            gridPoints.push_back({ row + 1, col + 1 });
            gridPoints.push_back({ row + 1, col });

            gridPoints.push_back({ row, col });
            gridPoints.push_back({ row, col + 1 });
            gridPoints.push_back({ row + 1, col + 1 });
        }
    }

    for (auto& point : gridPoints) {
        point = point / static_cast<float>(GRID_SIZE) - .5f;
    }

    m_vertexCount = gridPoints.size();

    // Create vertex buffer and fill it
    m_buf.setData(gridPoints, GL_STATIC_DRAW);

    // Bind vertex buffer to vao binding position 0
    posAttr.bindVertexBuffer(m_buf, 0, sizeof(glm::vec2));

    // Enable binding position 1
    VertexArray::Attribute offsetAttr = m_vao.enableVertexAttrib(1);
    offsetAttr.setFormat(2, GL_FLOAT, GL_FALSE, 0);
    offsetAttr.setBindingDivisor(1);

    std::vector<glm::vec2> instanceOffsets;
    for (std::size_t i = 0; i < 10; ++i) {
        instanceOffsets.push_back({ i / 3, i % 3 });
    }
    m_instanceAttrBuf.setData(instanceOffsets, GL_STATIC_DRAW);

    offsetAttr.bindVertexBuffer(m_instanceAttrBuf, 0, sizeof(glm::vec2));
}

Planet::~Planet() = default;

void Planet::render(const Scene& scene)
{
    m_shader.use();
    m_vao.use();

    m_shader.setUniform(0, glm::lookAt({}, scene.player().lookDirection(), scene.player().upDirection()));

    float aspectRatio = static_cast<float>(scene.windowWidth()) / scene.windowHeight();
    m_shader.setUniform(1, glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 10.0f));

    glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertexCount, 10);
}
}
