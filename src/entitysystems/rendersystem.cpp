#include "rendersystem.h"
#include "components.h"
#include "ecsengine.h"
#include "framebuffer.h"
#include "input.h"
#include "parameters.h"
#include "planetmath.h"
#include "shaders.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

namespace ou {

struct InstanceAttrib {
    glm::vec2 offset;
    short side;
    float scale;
    glm::vec4 discardRegion;
    short texIdx;
};

RenderSystem::RenderSystem(const Parameters& params)
    : m_hdrShader(quadVertShaderSrc, hdrFragShaderSrc)
    , m_planetShader(planetVertShaderSrc, planetFragShaderSrc)
    , m_skyShader(skyVertShaderSrc, skyFragShaderSrc)
    , m_terrainGenerator(terrainShaderSrc)
    , m_terrainDetailGenerator(terrain2ShaderSrc)
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
    m_meshBuf.setData(gridPoints, GL_STATIC_DRAW);

    // Bind vertex buffer to vao binding position 0
    VertexArray::BufferBinding vertexBinding = m_planetVao.getBinding(0);
    vertexBinding.bindVertexBuffer(m_meshBuf, 0, sizeof(glm::vec2));

    // Enable binding position 0
    VertexArray::Attribute posAttr = m_planetVao.enableVertexAttrib(0);
    posAttr.setFormat(2, GL_FLOAT, GL_FALSE, 0);
    posAttr.setBinding(vertexBinding);

    // Bind instance buffer to vao binding position 1
    VertexArray::BufferBinding instanceBinding = m_planetVao.getBinding(1);
    instanceBinding.bindVertexBuffer(m_instanceAttrBuf, 0, sizeof(InstanceAttrib));
    instanceBinding.setBindingDivisor(1);

    // Enable instance-wise attribute binding positions
    VertexArray::Attribute offsetAttr = m_planetVao.enableVertexAttrib(1);
    offsetAttr.setFormat(2, GL_FLOAT, GL_FALSE, offsetof(InstanceAttrib, offset));
    offsetAttr.setBinding(instanceBinding);

    VertexArray::Attribute sideAttr = m_planetVao.enableVertexAttrib(2);
    sideAttr.setIFormat(1, GL_SHORT, offsetof(InstanceAttrib, side));
    sideAttr.setBinding(instanceBinding);

    VertexArray::Attribute scaleAttr = m_planetVao.enableVertexAttrib(3);
    scaleAttr.setFormat(1, GL_FLOAT, GL_FALSE, offsetof(InstanceAttrib, scale));
    scaleAttr.setBinding(instanceBinding);

    VertexArray::Attribute discardRegionAttr = m_planetVao.enableVertexAttrib(4);
    discardRegionAttr.setFormat(4, GL_FLOAT, GL_FALSE, offsetof(InstanceAttrib, discardRegion));
    discardRegionAttr.setBinding(instanceBinding);

    VertexArray::Attribute texIdxAttr = m_planetVao.enableVertexAttrib(5);
    texIdxAttr.setIFormat(1, GL_SHORT, offsetof(InstanceAttrib, texIdx));
    texIdxAttr.setBinding(instanceBinding);
}

struct PBOSync {
    DeviceBuffer buf;
    int texIdx;
    GLsync sync;
};

struct PlanetRenderStates {
    Texture terrainTextures;
    Texture heightBases;
    CircularBuffer<PBOSync> pbos;

    std::vector<glm::i64vec2> snapNums{};
    std::int64_t baseHeight = 0.0f;
    glm::vec2 storedBase{};
    DeviceBuffer planetUboBuf{};

    PlanetRenderStates(Parameters const& params, Shader& terrainGenerator)
        : terrainTextures(GL_TEXTURE_2D_ARRAY)
        , heightBases(GL_TEXTURE_1D)
        , pbos(params.numPbos)
    {
        // terrainTextures
        terrainTextures.setWrapS(GL_CLAMP_TO_BORDER);
        terrainTextures.setWrapT(GL_CLAMP_TO_BORDER);
        terrainTextures.setMinFilter(GL_LINEAR);
        terrainTextures.setMagFilter(GL_LINEAR);
        terrainTextures.allocateStoarge3D(1, GL_R32F,
            params.terrainTextureSize, params.terrainTextureSize, // width, height
            params.terrainTextureCount); // array size

        // initialize top-level lod
        terrainGenerator.use();
        terrainTextures.useAsImage(0, 0, GL_WRITE_ONLY, GL_R32F);
        glDispatchCompute(params.terrainTextureSize / 32, params.terrainTextureSize / 32, 6);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // heightBases
        heightBases.allocateStorage1D(1, GL_RG32F, params.terrainTextureCount);

        std::vector<glm::vec2> data(params.terrainTextureCount, { 0.0f, 0.0f });
        heightBases.uploadTexture1D(0, 0, params.terrainTextureCount, GL_RG, GL_FLOAT, data);

        // pbos
        for (PBOSync& pbo : pbos) {
            pbo.buf.allocateStorage(sizeof(GLfloat) * 3, GL_STREAM_COPY);
        }
    }
};

void RenderSystem::render(ECSEngine& engine)
{
    SceneComponent const& scene = engine.getOne<SceneComponent>();
    Parameters const& params = engine.getOne<Parameters>();

    for (Entity& ent : engine.iterate<PlanetComponent>()) {
        PlanetComponent& planet = ent.get<PlanetComponent>();

        VoxelCoords centeredPos = scene.position - planet.position;
        if (centeredPos.voxel != glm::i64vec3()) {
            // planet is more than a voxel away; skip rendering
            return;
        }

        glm::dmat4 rotationMat = glm::rotate(glm::dmat4(1.0), -planet.angle, glm::dvec3(0, 0, 1));
        glm::i64vec3 pos = rotationMat * glm::dvec4(centeredPos.pos, 1);

        // initialize states
        if (!planet.r) {
            planet.r = std::make_shared<PlanetRenderStates>(params, m_terrainGenerator);
        }

        glm::dvec3 normPos = glm::normalize(glm::dvec3(pos));
        auto cubeCoords = cubizePoint(normPos);
        auto derivs = derivatives(cubeCoords.pos, cubeCoords.side);
        auto curvs = curvature(cubeCoords.pos, cubeCoords.side);

        glm::i64vec3 basePos = normPos * static_cast<double>(planet.radius + planet.r->baseHeight);
        glm::i64vec3 baseOffset = pos - basePos;

        const double normRadius = static_cast<double>(planet.radius) / params.rUnit;
        const glm::vec3 normOffset = glm::dvec3(baseOffset) / static_cast<double>(params.rUnit);

        std::int64_t playerDistFromCore = planet.radius + planet.playerTerrainHeight;

        // instance buffer data for lod 0
        std::vector<InstanceAttrib> lod0Attribs(6);
        for (int i = 0; i < 6; ++i) {
            lod0Attribs[i] = { { 0, 0 }, short(i), 1, {}, short(i) };
            if (i == cubeCoords.side) {
                lod0Attribs[i].offset = -cubeCoords.pos;
            }
        }

        double distance = glm::length(glm::dvec3(pos)) - playerDistFromCore;
        double normalizedDistance = distance / static_cast<double>(planet.radius);

        const int logDistance = std::ilogb(normalizedDistance);
        int levelsOfDetail = glm::clamp(params.zoomFactor - logDistance, 1, params.maxLods + 1);

        std::vector<glm::i64vec2> currentSnapNums = { { 0, 0 } };

        const int snapSize = params.snapSize;
        const double cellSize = 1.0 / snapSize;

        int lodUpdateIdx = -1;

        std::vector<InstanceAttrib> higherLodAttribs;
        for (int lod = 1; lod < levelsOfDetail; ++lod) {
            double scale = glm::exp2(static_cast<double>(-lod));
            double mod = scale * 2. * cellSize;

            glm::i64vec2 snapNums = glm::round(cubeCoords.pos / mod);

            bool updateNow = false;
            if (lodUpdateIdx < 0) {
                if (lod >= int(planet.r->snapNums.size())) {
                    updateNow = true;
                } else if (planet.r->snapNums[lod] != snapNums) {
                    updateNow = true;
                }
            }

            if (updateNow) {
                // generate update info
                lodUpdateIdx = 5 + lod;

                std::cout << "Update lod " << lod << " " << snapNums.x << ", " << snapNums.y << std::endl;
            } else {
                // parent map pending update, update later
                snapNums = planet.r->snapNums[lod];

                if (lod >= int(planet.r->snapNums.size())) {
                    std::cout << "delayed lod creation " << lod << std::endl;
                    levelsOfDetail = lod;
                    break;
                }
                if (planet.r->snapNums[lod] != snapNums) {
                    std::cout << "delayed update " << lod << std::endl;
                }
            }

            if (lod == 1) {
                glm::dvec2 off = mod * glm::dvec2(snapNums);
                lod0Attribs[cubeCoords.side].discardRegion = {
                    -.5 + off.x, -.5 + off.y, .5 + off.x, .5 + off.y
                };
            } else if (lod > 1) {
                glm::ivec2 r = snapNums - currentSnapNums.back() * std::int64_t(2);
                higherLodAttribs.back().discardRegion = {
                    -.5 + r.x * cellSize, -.5 + r.y * cellSize, .5 + r.x * cellSize, .5 + r.y * cellSize
                };
            }

            glm::vec2 offset = mod * glm::dvec2(snapNums) - cubeCoords.pos;

            InstanceAttrib attrib;
            attrib.offset = offset;
            attrib.side = short(cubeCoords.side);
            attrib.scale = static_cast<float>(scale);
            attrib.discardRegion = {};
            attrib.texIdx = short(5 + lod);
            higherLodAttribs.push_back(attrib);

            currentSnapNums.push_back(snapNums);
        }
        planet.r->snapNums = currentSnapNums;

        // update terrain textures
        if (lodUpdateIdx >= 0) {
            struct LodData {
                glm::vec2 align;
                glm::vec2 pDiff;
                float scale;
                int imgIdx;
                int parentIdx;
                int lod;
            };

            std::vector<LodData> lodDataList(params.terrainTextureCount);
            for (int i = 0; i < 6; ++i) {
                lodDataList[i] = { { 0, 0 }, {}, 1.0f, i, -1, 0 };
            }

            glm::dvec2 updateCenter;
            for (int lod = 1; lod < levelsOfDetail; ++lod) {
                double scale = glm::exp2(static_cast<double>(-lod));
                double mod = scale * 2. * cellSize;
                int index = 5 + lod;
                int parentIdx = lod == 1 ? cubeCoords.side : index - 1;
                glm::dvec2 center = glm::dvec2(planet.r->snapNums[lod]) * mod;
                glm::dvec2 pCenter = glm::dvec2(planet.r->snapNums[lod - 1]) * mod * 2.0;

                if (lod == lodUpdateIdx) {
                    updateCenter = center;
                }

                LodData lodData;
                lodData.align = glm::dvec2(eucmod(planet.r->snapNums[lod], snapSize)) * cellSize;
                lodData.pDiff = (center - pCenter) / (scale * 4);
                lodData.scale = static_cast<float>(scale);
                lodData.imgIdx = index;
                lodData.parentIdx = parentIdx;
                lodData.lod = lod;
                lodDataList[index] = lodData;
            }

            m_lodUboBuf.setData(lodDataList, GL_DYNAMIC_DRAW);
            m_terrainDetailGenerator.setUniform(0, cubeCoords.side);

            auto updateCenterDrivs = derivatives(updateCenter, cubeCoords.side);
            m_terrainDetailGenerator.setUniform(1, glm::vec3(updateCenterDrivs.fx));
            m_terrainDetailGenerator.setUniform(2, glm::vec3(updateCenterDrivs.fy));

            m_terrainDetailGenerator.setUniform(3, lodUpdateIdx);

            // write to texture
            m_terrainDetailGenerator.use();
            planet.r->terrainTextures.useLayerAsImage(0, 0, lodDataList[lodUpdateIdx].imgIdx, GL_WRITE_ONLY, GL_R32F);
            planet.r->terrainTextures.useAsTexture(1);
            planet.r->heightBases.useAsImage(2, 0, GL_READ_WRITE, GL_RG32F);
            m_lodUboBuf.use(GL_UNIFORM_BUFFER, 3);

            const int numWorkGroups = params.terrainTextureSize / 32;
            glDispatchCompute(numWorkGroups, numWorkGroups, 1);
            glMemoryBarrier(GL_ALL_BARRIER_BITS);
        }

        // select LODs to be rendered
        std::vector<InstanceAttrib> instanceAttribs;
        if (levelsOfDetail < 10) {
            instanceAttribs = lod0Attribs;
            std::copy(higherLodAttribs.begin(), higherLodAttribs.end(),
                std::back_inserter(instanceAttribs));
        } else {
            instanceAttribs.push_back(lod0Attribs[cubeCoords.side]);
            std::copy(higherLodAttribs.end() - params.maxRenderLods, higherLodAttribs.end(),
                std::back_inserter(instanceAttribs));
        }

        // upload instance attribs
        m_instanceAttrBuf.setData(instanceAttribs, GL_STATIC_DRAW);

        // build proj view matrix
        glm::dmat4 viewMat = glm::lookAt({}, scene.lookDirection, scene.upDirection)
            * glm::transpose(rotationMat); // transpose == inverse for rotation matrix
        double aspectRatio = static_cast<double>(scene.windowSize.x) / scene.windowSize.y;
        glm::dmat4 projMat = glm::perspective(glm::radians(90.0), aspectRatio, 0.1, 10.0);

        // set uniforms
        struct PlanetUbo {
            glm::mat4 viewProjMat;
            glm::vec4 xJac;
            glm::vec4 yJac;
            glm::vec4 xxCurv;
            glm::vec4 xyCurv;
            glm::vec4 yyCurv;
            glm::vec4 eyeOffset;
            glm::vec4 lightDir;
            glm::vec4 eyePos;
            glm::vec2 origin;
            glm::vec2 uBase;
            int playerSide;
            float terrainFactor;
            float radius;
        };

        PlanetUbo ubo;
        ubo.viewProjMat = projMat * viewMat;
        ubo.origin = cubeCoords.pos;
        ubo.xJac = glm::vec4(derivs.fx, 0);
        ubo.yJac = glm::vec4(derivs.fy, 0);
        ubo.xxCurv = glm::vec4(curvs.fxx, 0);
        ubo.xyCurv = glm::vec4(curvs.fxy, 0);
        ubo.yyCurv = glm::vec4(curvs.fyy, 0);
        ubo.playerSide = cubeCoords.side;
        ubo.eyeOffset = glm::vec4(normOffset, 0);
        ubo.terrainFactor = static_cast<float>(planet.terrainFactor);
        ubo.uBase = planet.r->storedBase;
        ubo.radius = static_cast<float>(normRadius);
        ubo.lightDir = glm::vec4(0, 0, 1, 0);
        ubo.eyePos = glm::vec4(glm::dvec3(pos) / static_cast<double>(params.rUnit), 0);

        planet.r->planetUboBuf.setData(RawBufferView(ubo), GL_DYNAMIC_DRAW);

        // render planet
        m_planetShader.use();
        m_planetVao.use();
        planet.r->planetUboBuf.use(GL_UNIFORM_BUFFER, 0);
        planet.r->terrainTextures.useAsTexture(1);
        planet.r->heightBases.useAsImage(2, 0, GL_READ_ONLY, GL_RG32F);
        glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertexCount, instanceAttribs.size());

        // render sky
        glFrontFace(GL_CW);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        m_skyShader.use();
        m_planetVao.use();
        planet.r->planetUboBuf.use(GL_UNIFORM_BUFFER, 0);
        glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertexCount, instanceAttribs.size());
        glDisable(GL_BLEND);
        glFrontFace(GL_CCW);

        while (planet.r->pbos.count()) {
            PBOSync& pbo = planet.r->pbos.top();

            GLenum pboStatus = glClientWaitSync(pbo.sync, 0, 0);
            if (pboStatus == GL_ALREADY_SIGNALED || pboStatus == GL_CONDITION_SATISFIED) {
                glDeleteSync(pbo.sync);

                GLfloat* data = static_cast<GLfloat*>(pbo.buf.map(GL_READ_ONLY));
                double height = static_cast<double>(data[0]);
                planet.r->storedBase = { data[1], data[2] };
                double base = static_cast<double>(data[1]) + static_cast<double>(data[2]);
                pbo.buf.unmap();

                planet.r->baseHeight = std::int64_t(base * planet.terrainFactor * planet.radius);
                double adjustedHeight = height * planet.terrainFactor * planet.radius;
                planet.playerTerrainHeight = std::int64_t(adjustedHeight) + planet.r->baseHeight;

                planet.r->pbos.pop();
            } else {
                break;
            }
        }

        // read height value
        if (planet.r->pbos.available() && higherLodAttribs.size()) {
            PBOSync& pbo = planet.r->pbos.push();

            InstanceAttrib hLod = instanceAttribs.back();
            glm::vec2 coords = (hLod.offset + 1.0f) * 0.5f * float(params.terrainTextureSize);
            glm::ivec2 iCoords = glm::clamp(glm::ivec2(glm::round(coords)), 0, params.terrainTextureSize);

            pbo.texIdx = hLod.texIdx;

            pbo.buf.copyTexture(planet.r->terrainTextures, 0,
                { iCoords, pbo.texIdx },
                { 1, 1, 1 },
                GL_RED, GL_FLOAT, sizeof(GLfloat) * 1,
                0); // offset into pbo

            pbo.buf.copyTexture(planet.r->heightBases, 0,
                { pbo.texIdx, 0, 0 },
                { 1, 1, 1 },
                GL_RG, GL_FLOAT, sizeof(GLfloat) * 2,
                sizeof(GLfloat)); // offset into pbo

            pbo.sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        }
    }
}

void RenderSystem::update(ECSEngine& engine, float)
{
    SceneComponent& scene = engine.getOne<SceneComponent>();
    Parameters const& params = engine.getOne<Parameters>();

    // window resize event
    if (scene.windowResized) {
        scene.windowResized = false;

        // Resize viewport
        glViewport(0, 0, scene.windowSize.x, scene.windowSize.y);

        // Build framebuffer
        m_hdrColorTexture = Texture(GL_TEXTURE_2D_MULTISAMPLE);
        m_hdrColorTexture.allocateMultisample2D(params.msaaSamples, GL_RGBA16F, scene.windowSize.x, scene.windowSize.y, GL_TRUE);
        m_hdrFrameBuffer.bindTexture(GL_COLOR_ATTACHMENT0, m_hdrColorTexture);

        m_hdrDepthRenderBuffer = RenderBuffer();
        m_hdrDepthRenderBuffer.allocateMultisample(params.msaaSamples, GL_DEPTH24_STENCIL8, scene.windowSize.x, scene.windowSize.y);
        m_hdrFrameBuffer.bindRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, m_hdrDepthRenderBuffer);

        if (!m_hdrFrameBuffer.isComplete()) {
            std::cerr << "Error building hdr framebuffer\n";
            throw std::runtime_error("Framebuffer is not complete");
        }

        m_hdrResolveColorTexture = Texture(GL_TEXTURE_2D);
        m_hdrResolveColorTexture.allocateStorage2D(1, GL_RGBA16F, scene.windowSize.x, scene.windowSize.y);
        m_hdrResolveFrameBuffer.bindTexture(GL_COLOR_ATTACHMENT0, m_hdrResolveColorTexture);

        if (!m_hdrResolveFrameBuffer.isComplete()) {
            std::cerr << "Error building hdr resolve framebuffer\n";
            throw std::runtime_error("Framebuffer is not complete");
        }
    }

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
    glDisable(GL_DEPTH_TEST);
    glBlitNamedFramebuffer(m_hdrFrameBuffer.id(), m_hdrResolveFrameBuffer.id(),
        0, 0, scene.windowSize.x, scene.windowSize.y,
        0, 0, scene.windowSize.x, scene.windowSize.y,
        GL_COLOR_BUFFER_BIT, GL_NEAREST);

    if (params.renderWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    FrameBuffer::defaultBuffer().use(GL_FRAMEBUFFER);
    m_quadVao.use();
    m_hdrShader.use();
    m_hdrResolveColorTexture.useAsTexture(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
}
