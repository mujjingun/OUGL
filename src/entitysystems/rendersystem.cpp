#include "rendersystem.h"
#include "components.h"
#include "ecsengine.h"
#include "framebuffer.h"
#include "parameters.h"
#include "planetmath.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace ou {

struct InstanceAttrib {
    glm::vec2 offset;
    float side;
    float scale;
    glm::vec4 discardRegion;
};

RenderSystem::RenderSystem(const Parameters& params)
    : m_hdrShader("shaders/hdr.vert.glsl", "shaders/hdr.frag.glsl")
    , m_planetShader("shaders/planet.vert.glsl", "shaders/planet.frag.glsl")
    , m_terrainGenerator("shaders/terrain.comp.glsl")
{
    glEnable(GL_CULL_FACE);

    std::vector<glm::vec2> gridPoints;
    for (float col = 0; col < params.gridSize; ++col) {
        for (float row = 0; row < params.gridSize; ++row) {
            gridPoints.push_back({ row, col });
            gridPoints.push_back({ row + 1, col });
            gridPoints.push_back({ row + 1, col + 1 });

            gridPoints.push_back({ row, col });
            gridPoints.push_back({ row + 1, col + 1 });
            gridPoints.push_back({ row, col + 1 });
        }
    }

    for (auto& point : gridPoints) {
        point = point / static_cast<float>(params.gridSize) * 2.f - 1.f;
    }

    m_vertexCount = gridPoints.size();

    // Create vertex buffer and fill it
    m_gridBuf.setData(gridPoints, GL_STATIC_DRAW);

    // Bind vertex buffer to vao binding position 0
    VertexArray::BufferBinding vertexBinding = m_vao.getBinding(0);
    vertexBinding.bindVertexBuffer(m_gridBuf, 0, sizeof(glm::vec2));

    // Enable binding position 0
    VertexArray::Attribute posAttr = m_vao.enableVertexAttrib(0);
    posAttr.setFormat(2, GL_FLOAT, GL_FALSE, 0);
    posAttr.setBinding(vertexBinding);

    // Bind instance buffer to vao binding position 1
    VertexArray::BufferBinding instanceBinding = m_vao.getBinding(1);
    instanceBinding.bindVertexBuffer(m_instanceAttrBuf, 0, sizeof(InstanceAttrib));
    instanceBinding.setBindingDivisor(1);

    // Enable instance-wise attribute binding positions
    VertexArray::Attribute offsetAttr = m_vao.enableVertexAttrib(1);
    offsetAttr.setFormat(2, GL_FLOAT, GL_FALSE, offsetof(InstanceAttrib, offset));
    offsetAttr.setBinding(instanceBinding);

    VertexArray::Attribute sideAttr = m_vao.enableVertexAttrib(2);
    sideAttr.setFormat(1, GL_FLOAT, GL_FALSE, offsetof(InstanceAttrib, side));
    sideAttr.setBinding(instanceBinding);

    VertexArray::Attribute scaleAttr = m_vao.enableVertexAttrib(3);
    scaleAttr.setFormat(1, GL_FLOAT, GL_FALSE, offsetof(InstanceAttrib, scale));
    scaleAttr.setBinding(instanceBinding);

    VertexArray::Attribute discardRegionAttr = m_vao.enableVertexAttrib(4);
    discardRegionAttr.setFormat(4, GL_FLOAT, GL_FALSE, offsetof(InstanceAttrib, discardRegion));
    discardRegionAttr.setBinding(instanceBinding);
}

void RenderSystem::render(ECSEngine& engine)
{
    SceneComponent const& scene = engine.getOne<SceneComponent>();
    Parameters const& params = engine.getOne<Parameters>();

    for (Entity& ent : engine.iterate<PlanetComponent>()) {
        PlanetComponent& planet = ent.get<PlanetComponent>();

        // initialize top-level lod
        if (!planet.terrainTextures) {
            planet.terrainTextures = std::make_shared<Texture>(GL_TEXTURE_2D_ARRAY);

            planet.terrainTextures->setWrapS(GL_CLAMP_TO_BORDER);
            planet.terrainTextures->setWrapT(GL_CLAMP_TO_BORDER);
            planet.terrainTextures->setMinFilter(GL_LINEAR);
            planet.terrainTextures->setMagFilter(GL_LINEAR);
            planet.terrainTextures->allocateStoarge3D(1, GL_R32F,
                params.terrainTextureSize, params.terrainTextureSize, // width, height
                params.terrainTextureCount); // array size

            // mode/side
            m_terrainGenerator.setUniform(0, -1);

            m_terrainGenerator.use();
            planet.terrainTextures->useAsImage(0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);
            glDispatchCompute(params.terrainTextureSize / 32, params.terrainTextureSize / 32, 6);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        }

        VoxelCoords centeredPos = scene.position - planet.position;
        if (centeredPos.voxel != glm::i64vec3()) {
            // planet is more than a voxel away; skip rendering
            return;
        }

        glm::dvec3 normPos = glm::normalize(glm::dvec3(centeredPos.pos));
        auto cubeCoords = cubizePoint(normPos);
        auto derivs = derivatives(cubeCoords.pos, cubeCoords.side);
        auto curvs = curvature(cubeCoords.pos, cubeCoords.side);

        glm::i64vec3 surfacePos = normPos * static_cast<double>(planet.planetRadius);
        glm::i64vec3 surfaceOffset = centeredPos.pos - surfacePos;
        glm::dvec3 surfaceOffsetInRadiusUnits = glm::dvec3(surfaceOffset) / static_cast<double>(planet.planetRadius);

        // set instance buffer data
        std::vector<InstanceAttrib> instanceBuffer;

        for (int i = 0; i < 6; ++i) {
            if (i == cubeCoords.side) {
                instanceBuffer.push_back({ -cubeCoords.pos, static_cast<float>(i), 1, {} });
            } else {
                instanceBuffer.push_back({ { 0, 0 }, static_cast<float>(i), 1, {} });
            }
        }

        double distance = glm::length(glm::dvec3(centeredPos.pos)) - planet.planetRadius;
        double normalizedDistance = distance / static_cast<double>(planet.planetRadius);

        const int logDistance = std::ilogb(normalizedDistance);
        const int levelsOfDetail = glm::clamp(params.zoomFactor - logDistance, 1, params.maxLods - 1);

        std::vector<glm::i64vec2> currentSnapNums = { { 0, 0 } };

        const int snapSize = params.snapSize;
        const double cellSize = 1.0 / snapSize;

        int lodUpdateIdx = -1;

        for (int lod = 1; lod < levelsOfDetail; ++lod) {
            double scale = glm::exp2(static_cast<double>(-lod));
            double mod = scale * 2. * cellSize;

            glm::i64vec2 snapNums = glm::round(cubeCoords.pos / mod);

            bool updateNow = false;
            if (lodUpdateIdx < 0) {
                if (lod >= int(planet.snapNums.size())) {
                    updateNow = true;
                }
                else if (planet.snapNums[lod] != snapNums) {
                    updateNow = true;
                }
            }

            if (updateNow) {
                // generate update info
                lodUpdateIdx = 5 + lod;

                std::cout << "Update lod " << lod << " " << snapNums.x << ", " << snapNums.y << std::endl;
            } else {
                // parent map pending update, update later
                snapNums = planet.snapNums[lod];

                if (lod >= int(planet.snapNums.size())) {
                    std::cout << "delayed lod creation " << lod << std::endl;
                    break;
                }
                if (planet.snapNums[lod] != snapNums) {
                    std::cout << "delayed update " << lod << std::endl;
                }
            }

            if (lod == 1) {
                glm::dvec2 off = mod * glm::dvec2(snapNums);
                instanceBuffer[cubeCoords.side].discardRegion = {
                    -.5 + off.x, -.5 + off.y, .5 + off.x, .5 + off.y
                };
            }
            if (lod > 1) {
                glm::ivec2 r = snapNums - currentSnapNums.back() * std::int64_t(2);
                instanceBuffer.back().discardRegion = {
                    -.5 + r.x * cellSize, -.5 + r.y * cellSize, .5 + r.x * cellSize, .5 + r.y * cellSize
                };
            }

            glm::vec2 offset = mod * glm::dvec2(snapNums) - cubeCoords.pos;

            InstanceAttrib attrib;
            attrib.offset = offset;
            attrib.side = cubeCoords.side;
            attrib.scale = static_cast<float>(scale);
            attrib.discardRegion = {};
            instanceBuffer.push_back(attrib);

            currentSnapNums.push_back(snapNums);
        }
        planet.snapNums = currentSnapNums;

        // update terrain textures
        if (lodUpdateIdx >= 0) {

            struct LodData {
                glm::vec2 align;
                glm::vec2 pDiff;
                float scale;
                int imgIdx;
                int parentIdx;
                int unused[1]{};
            };

            std::vector<LodData> lodDataList(params.terrainTextureCount);
            for (int i = 0; i < 6; ++i) {
                lodDataList[i] = { { 0, 0 }, {}, 1.0f, i, -1 };
            }

            glm::dvec2 updateCenter;
            for (int lod = 1; lod < levelsOfDetail; ++lod) {
                double scale = glm::exp2(static_cast<double>(-lod));
                double mod = scale * 2. * cellSize;
                int index = 5 + lod;
                int parentIdx = lod == 1 ? cubeCoords.side : index - 1;
                glm::dvec2 center = glm::dvec2(planet.snapNums[lod]) * mod;
                glm::dvec2 pCenter = glm::dvec2(planet.snapNums[lod - 1]) * mod * 2.0;

                if (lod == lodUpdateIdx) {
                    updateCenter = center;
                }

                LodData lodData;
                lodData.align = glm::dvec2(eucmod(planet.snapNums[lod], snapSize)) * cellSize;
                lodData.pDiff = (center - pCenter) / (scale * 4);
                lodData.scale = static_cast<float>(scale);
                lodData.imgIdx = index;
                lodData.parentIdx = parentIdx;
                lodDataList[index] = lodData;
            }

            m_lodUboBuf.setData(lodDataList, GL_DYNAMIC_DRAW);
            m_terrainGenerator.setUniform(0, cubeCoords.side);

            auto derivs = derivatives(updateCenter, cubeCoords.side);
            m_terrainGenerator.setUniform(1, glm::vec3(derivs.fx));
            m_terrainGenerator.setUniform(2, glm::vec3(derivs.fy));

            m_terrainGenerator.setUniform(3, lodUpdateIdx);

            // write to texture
            m_terrainGenerator.use();
            planet.terrainTextures->useAsImage(0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);
            planet.terrainTextures->useAsTexture(1);
            m_lodUboBuf.use(GL_UNIFORM_BUFFER, 1);

            const int numWorkGroups = params.terrainTextureSize / 32;
            glDispatchCompute(numWorkGroups, numWorkGroups, 1);
            glMemoryBarrier(GL_ALL_BARRIER_BITS);
        }

        m_instanceAttrBuf.setData(instanceBuffer, GL_STATIC_DRAW);

        // view matrix
        glm::dmat4 viewMat = glm::lookAt({}, scene.lookDirection, scene.upDirection);

        // projection matrix
        double aspectRatio = static_cast<double>(scene.windowSize.x) / scene.windowSize.y;
        glm::dmat4 projMat = glm::perspective(glm::radians(60.0), aspectRatio, 0.1, 10.0);

        // view-projection matrix
        m_planetShader.setUniform(0, projMat * viewMat);

        // origin
        m_planetShader.setUniform(1, glm::vec2(cubeCoords.pos));

        // xJac
        m_planetShader.setUniform(2, glm::vec3(derivs.fx));

        // yJac
        m_planetShader.setUniform(3, glm::vec3(derivs.fy));

        // xxCurv
        m_planetShader.setUniform(4, glm::vec3(curvs.fxx));

        // xyCurv
        m_planetShader.setUniform(5, glm::vec3(curvs.fxy));

        // yyCurv
        m_planetShader.setUniform(6, glm::vec3(curvs.fyy));

        // playerSide
        m_planetShader.setUniform(7, cubeCoords.side);

        // eyePos
        m_planetShader.setUniform(8, glm::vec3(surfaceOffsetInRadiusUnits));

        // terrainFactor
        m_planetShader.setUniform(9, planet.terrainFactor);

        m_planetShader.use();
        m_vao.use();
        planet.terrainTextures->useAsTexture(0);
        glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertexCount, instanceBuffer.size());
    }
}

void RenderSystem::update(ECSEngine& engine, float)
{
    SceneComponent& scene = engine.getOne<SceneComponent>();

    // window resize event
    if (scene.windowResized) {
        scene.windowResized = false;

        // Resize viewport
        glViewport(0, 0, scene.windowSize.x, scene.windowSize.y);

        // Build framebuffer
        m_hdrFrameBuffer = FrameBuffer();

        m_hdrColorTexture = Texture(GL_TEXTURE_2D);
        m_hdrColorTexture.setMinFilter(GL_NEAREST);
        m_hdrColorTexture.setMagFilter(GL_NEAREST);
        m_hdrColorTexture.allocateStorage2D(1, GL_RGBA16F, scene.windowSize.x, scene.windowSize.y);
        m_hdrFrameBuffer.bindTexture(GL_COLOR_ATTACHMENT0, m_hdrColorTexture);

        m_hdrDepthRenderBuffer = RenderBuffer();
        m_hdrDepthRenderBuffer.allocateStorage(GL_DEPTH24_STENCIL8, scene.windowSize.x, scene.windowSize.y);
        m_hdrFrameBuffer.bindRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, m_hdrDepthRenderBuffer);

        if (!m_hdrFrameBuffer.isComplete()) {
            std::cerr << "Error building framebuffer\n";
            throw std::runtime_error("Framebuffer is not complete");
        }
    }

    Parameters params = engine.getOne<Parameters>();

    // Clear screen & framebuffer
    float clearColor[] = { 0, 0, 0, 0 };
    FrameBuffer::defaultBuffer().clear(GL_COLOR, 0, clearColor);
    m_hdrFrameBuffer.clear(GL_COLOR, 0, clearColor);

    float clearDepth[] = { 1 };
    FrameBuffer::defaultBuffer().clear(GL_DEPTH, 0, clearDepth);
    m_hdrFrameBuffer.clear(GL_DEPTH, 0, clearDepth);

    // Render stuff to framebuffer
    m_hdrFrameBuffer.use(GL_FRAMEBUFFER);
    glEnable(GL_DEPTH_TEST);

    if (params.renderWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // render actual stuff
    render(engine);

    // apply HDR
    FrameBuffer::defaultBuffer().use(GL_FRAMEBUFFER);
    glDisable(GL_DEPTH_TEST);

    if (params.renderWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    m_hdrVao.use();
    m_hdrShader.use();
    m_hdrColorTexture.useAsTexture(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
}
