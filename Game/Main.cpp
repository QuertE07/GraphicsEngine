#include "Engine.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/Pipeline.h"

#include <fmod.hpp>
#include <map>
#include <memory>
#include <fstream>

using namespace gl;

struct Vertex
{
    float x, y, z;
};

std::vector<Vertex> vertices =
{
    Vertex{ -1.0f, -1.0f, 0.0f}, // Bottom-Left
    Vertex{  1.0f, -1.0f, 0.0f}, // Bottom-Right
    Vertex{  0.0f,  1.0f, 0.0f}, // Top-Middle
};

int main()
{
    SetWorkingDirectory("Assets");
    if (Engine::Get().Initialize() == false) return 0;

    auto vb = std::make_shared<VertexBuffer>();
    vb->Create<Vertex>(vertices, Engine::Get().GetRenderer().GetGPUDevice());

    auto vshader = Resources().Get<gl::Shader>("shaders/position.vert", Engine::Get().GetRenderer());
    auto fshader = Resources().Get<gl::Shader>("shaders/color.frag", Engine::Get().GetRenderer());

    auto pipeline = std::make_shared<Pipeline>();
    pipeline->AddVertexBuffer(sizeof(Vertex));
    pipeline->AddVertexAttribute(
        0,
        SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
        offsetof(Vertex, x));

    pipeline->Create(*vshader.get(), *fshader.get(),
        Engine::Get().GetRenderer().GetGPUDevice(),
        Engine::Get().GetRenderer().GetWindow());

    bool quit = false;

    while (!quit) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                quit = true;
            }
        }

        // ENGINE
        Engine::Get().Update();
        float dt = Engine::Get().GetTime().GetDeltaTime();

        // RENDER
        Engine::Get().GetRenderer().BeginFrame();

        Engine::Get().GetRenderer().SetPipeline(*pipeline);
        Engine::Get().GetRenderer().SetVertexBuffer(*vb);
        Engine::Get().GetRenderer().Draw(vb->GetVertexCount());

        Engine::Get().GetRenderer().EndFrame();
    }

    Engine::Get().Shutdown();

    return 0;
}