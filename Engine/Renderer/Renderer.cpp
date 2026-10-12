#include "pch.h"
#include "Renderer.h"
#include "Model.h"
#include "Math/Transform.h"
#include "Math/MathUtils.h"
#include "Math/Rect.h"
#include "Texture.h"
#include "Pipeline.h"
#include "VertexBuffer.h"

bool gl::Renderer::Initialize(const char* name, int width, int height)
{
    SDL_Init(SDL_INIT_VIDEO);
    m_width = width;
    m_height = height;

    m_window = SDL_CreateWindow(name, m_width, m_height, 0);
    if (m_window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    SDL_GPUShaderFormat formats = SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL;
    m_gpuDevice = SDL_CreateGPUDevice(formats, true, NULL);
    if (!m_gpuDevice)
    {
        std::cerr << "Failed to create GPU Device: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return false;
    }

    SDL_ClaimWindowForGPUDevice(m_gpuDevice, m_window);

    std::cout << "GPU Driver Initialized: " << SDL_GetGPUDeviceDriver(m_gpuDevice) << std::endl;

    if (!TTF_Init()) {
        std::cerr << "TTF_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void gl::Renderer::Shutdown()
{
    TTF_Quit();
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void gl::Renderer::Clear() const
{
    SDL_RenderClear(m_renderer);
}

void gl::Renderer::Present() const
{
    SDL_RenderPresent(m_renderer);
}

bool gl::Renderer::BeginFrame()
{
    m_commandBuffer = SDL_AcquireGPUCommandBuffer(m_gpuDevice);
    if (!m_commandBuffer)
    {
        std::cerr << "Could not acquire command buffer: " << SDL_GetError() << std::endl;
        return false;
    }


    SDL_GPUTexture* swapchainTexture = nullptr;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(m_commandBuffer, m_window, &swapchainTexture, nullptr, nullptr))
    {
        std::cerr << "Could not acquire swapchain texture: " << SDL_GetError() << std::endl;
        return false;
    }

    if (swapchainTexture != nullptr)
    {
        // configure the color target attachments (This handles clearing the screen)
        SDL_GPUColorTargetInfo color_target_info{};
        color_target_info.texture = swapchainTexture;
        color_target_info.clear_color = SDL_FColor{ 0.3f, 0.3f, 0.3f, 1.0f };
        color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
        color_target_info.store_op = SDL_GPU_STOREOP_STORE;

        m_renderPass = SDL_BeginGPURenderPass(m_commandBuffer, &color_target_info, 1, nullptr);
    }

    return true;
}

bool gl::Renderer::EndFrame()
{
    if (m_renderPass)
    {
        SDL_EndGPURenderPass(m_renderPass);
        m_renderPass = nullptr;
    }

    if (!SDL_SubmitGPUCommandBuffer(m_commandBuffer))
    {
        std::cerr << "Could not submit command buffer: " << SDL_GetError() << std::endl;
        return false;
    }

    m_commandBuffer = nullptr;

    return true;
}

void gl::Renderer::SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const
{
    SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
}

void gl::Renderer::SetColor(float r, float g, float b, float a) const
{
    SDL_SetRenderDrawColorFloat(m_renderer, r, g, b, a);
}

void gl::Renderer::SetColor(const Color& color) const
{
    SDL_SetRenderDrawColorFloat(m_renderer, color.r, color.g, color.b, 1.0f);
}

void gl::Renderer::DrawPoint(float x, float y) const
{
    SDL_RenderPoint(m_renderer, x, y);
}

void gl::Renderer::DrawLine(float x1, float y1, float x2, float y2) const
{
    SDL_RenderLine(m_renderer, x1, y1, x2, y2);
}

void gl::Renderer::DrawFillRect(float x, float y, float w, float h) const
{
    SDL_FRect rect{ x, y, w, h };
    SDL_RenderFillRect(m_renderer, &rect);
}

void gl::Renderer::DrawRect(float x, float y, float w, float h) const
{
    SDL_FRect rect{ x, y, w, h };
    SDL_RenderRect(m_renderer, &rect);
}

void gl::Renderer::DrawModel(const Model& model, const Transform& transform) const
{
    for (auto mesh : model.GetMeshes())
    {
        SetColor(mesh.GetColor());

        auto& points = mesh.GetPoints();

        for (int i = 0; i + 1 < points.size(); i++)
        {
            Vector2 v1 = points[i];
            Vector2 v2 = points[i + 1];

            v1 *= transform.scale;
            v2 *= transform.scale;

            v1 = v1.Rotate(transform.rotation * DegToRad);
            v2 = v2.Rotate(transform.rotation * DegToRad);

            v1 += transform.position;
            v2 += transform.position;

            DrawLine(v1.x, v1.y, v2.x, v2.y);
        }
    }
}

void gl::Renderer::DrawDebugText(float x, float y, const char* text) const
{
    SDL_RenderDebugText(m_renderer, x, y, text);
}

void gl::Renderer::DrawTexture(Texture& texture, float x, float y, float angle, float scale, bool flipH, const Vector2& origin) const
{
    Vector2 size = texture.m_size;

    float cameraX = (m_cameraEnabled) ? (m_camera.x - m_width / 2) : 0.0f;
    float cameraY = (m_cameraEnabled) ? (m_camera.y - m_height / 2) : 0.0f;

    SDL_FRect destRect;
    destRect.w = size.x * scale;
    destRect.h = size.y * scale;

    destRect.x = (x - cameraX) - (destRect.w * origin.x);
    destRect.y = (y - cameraY) - (destRect.h * origin.y);

    SDL_RenderTextureRotated(m_renderer, texture.m_texture, NULL, &destRect, angle, NULL, flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void gl::Renderer::DrawTexture(Texture& texture, const Rect& source, float x, float y, float angle, float scale, bool flipH, const Vector2& origin) const
{
    float cameraX = (m_cameraEnabled) ? (m_camera.x - m_width / 2) : 0.0f;
    float cameraY = (m_cameraEnabled) ? (m_camera.y - m_height / 2) : 0.0f;

    SDL_FRect sourceRect;
    sourceRect.x = source.x;
    sourceRect.y = source.y;
    sourceRect.w = source.w;
    sourceRect.h = source.h;

    SDL_FRect destRect;
    destRect.w = source.w * scale;
    destRect.h = source.h * scale;

    destRect.x = (x - cameraX) - (destRect.w * origin.x);
    destRect.y = (y - cameraY) - (destRect.h * origin.y);

    SDL_RenderTextureRotated(m_renderer, texture.m_texture, &sourceRect, &destRect, angle, NULL, flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void gl::Renderer::SetPipeline(const Pipeline& pipeline)
{
    // bind the pipeline to the current render pass with SDL_BindGPUGraphicsPipeline(renderPass, gpuPipeline)
    // the pipeline sets the shaders and render settings used by the draw calls that follow
    // use pipeline.GetGPUPipeline() to get the sdl pipeline
    SDL_BindGPUGraphicsPipeline(m_renderPass, pipeline.m_gpuPipeline);
}

void gl::Renderer::SetVertexBuffer(const VertexBuffer& vertexBuffer)
{
    // describe which buffer to bind and where to start reading from
    SDL_GPUBufferBinding binding{
        .buffer = vertexBuffer.m_gpuBuffer, // todo: the vertex buffer's gpu buffer
        .offset = 0   // start reading at the beginning of the buffer
    };

    // bind the vertex buffer to the current render pass with SDL_BindGPUVertexBuffers()
    // the draw calls that follow read their vertices from this buffer
    // parameters:
    //   render pass   - the current render pass (m_renderPass)
    //   first slot    - 0, matches the buffer slot set in Pipeline::AddVertexBuffer()
    //   bindings      - pointer to the binding above
    //   binding count - 1, we are binding one buffer
    SDL_BindGPUVertexBuffers(m_renderPass, 0, &binding, 1);
}

void gl::Renderer::Draw(uint32_t vertexCount)
{
    // draw using the currently bound pipeline and vertex buffer with SDL_DrawGPUPrimitives()
    // parameters:
    //   render pass    - the current render pass (m_renderPass)
    //   vertex count   - the number of vertices to draw (vertexCount)
    //   instance count - 1, draw one copy (more than 1 is used for instancing)
    //   first vertex   - 0, start at the first vertex in the buffer
    //   first instance - 0, start at the first instance
    SDL_DrawGPUPrimitives(m_renderPass, vertexCount, 1, 0, 0);
}

void gl::Renderer::SetVertexUniform(uint32_t slot, const void* data, uint32_t size)
{
    SDL_PushGPUVertexUniformData(m_commandBuffer, slot, data, size);
}

void gl::Renderer::SetFragmentUniform(uint32_t slot, const void* data, uint32_t size)
{
    SDL_PushGPUFragmentUniformData(m_commandBuffer, slot, data, size);
}