#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "devicebuffer.h"
#include "entitysystem.h"
#include "framebuffer.h"
#include "parameters.h"
#include "renderbuffer.h"
#include "shader.h"
#include "texture.h"
#include "vertexarray.h"

namespace ou {

class RenderSystem : public EntitySystem {
    // HDR
    FrameBuffer m_hdrFrameBuffer;
    Texture m_hdrColorTexture;
    RenderBuffer m_hdrDepthRenderBuffer;
    Shader m_hdrShader;
    VertexArray m_hdrVao;

    FrameBuffer m_hdrResolveFrameBuffer;
    Texture m_hdrResolveColorTexture;

    // Planet
    Shader m_planetShader;
    Shader m_terrainGenerator, m_terrainDetailGenerator;
    VertexArray m_vao;
    DeviceBuffer m_gridBuf, m_instanceAttrBuf;
    std::size_t m_vertexCount;
    DeviceBuffer m_lodUboBuf;

public:
    RenderSystem(Parameters const& params);

    // EntitySystem interface
public:
    void update(ECSEngine& engine, float deltaTime) override;

private:
    void render(ECSEngine& engine);
};
}

#endif // RENDERSYSTEM_H
