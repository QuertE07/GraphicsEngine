#include "Engine.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/Pipeline.h"

#include <fmod.hpp>
#include <map>
#include <memory>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace gl;

namespace gl {
    struct Color4
    {
        float r, g, b, a;
    };
}

struct Vertex
{
    Vector3 position;
    Color4 color;
};

struct vertexUniform
{
    glm::mat4 model;
};

vertexUniform uniform;

std::vector<Vertex> vertices =
{
    Vertex{ { -0.8f, -0.4f, 0.0f }, { 1.0, 0.0, 0.0, 1.0 } }, // Bottom-Left
    Vertex{ { 0.7f, -0.8f, 0.0f }, { 0.0, 1.0, 0.0, 1.0 } }, // Bottom-Right
    Vertex{ { 0.2f,  0.9f, 0.0f }, { 0.0, 0.0, 1.0, 1.0 } }, // Top-Middle
};

int main()
{
    SetWorkingDirectory("Assets");
    if (Engine::Get().Initialize() == false) return 0;

    auto vb = std::make_shared<VertexBuffer>();
    vb->Create<Vertex>(vertices, Engine::Get().GetRenderer().GetGPUDevice());

    auto vshader = Resources().Get<gl::Shader>("shaders/position.vert", Engine::Get().GetRenderer(), 1);
    auto fshader = Resources().Get<gl::Shader>("shaders/color.frag", Engine::Get().GetRenderer(), 1);

    auto pipeline = std::make_shared<Pipeline>();
    pipeline->AddVertexBuffer(sizeof(Vertex));
    pipeline->AddVertexAttribute(0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Vertex, position));
    pipeline->AddVertexAttribute(1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4, offsetof(Vertex, color));

    pipeline->Create(*vshader.get(), *fshader.get(),
        Engine::Get().GetRenderer().GetGPUDevice(),
        Engine::Get().GetRenderer().GetWindow());

    float scale = 0.5;

    Color4 color{ 0, 0, 0, 1 };

    uniform.model = glm::mat4(1.0f);
    uniform.model = glm::translate(uniform.model, glm::vec3(-0.2f, 0.0f, 0.0f));
    uniform.model = glm::rotate(uniform.model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    uniform.model = glm::scale(uniform.model, glm::vec3(0.5f, 0.5f, 0.5f));

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

        //scale = sin(Engine::Get().GetTime().GetTime());

        float distance = sin(Engine::Get().GetTime().GetTime());

        uniform.model = glm::translate(uniform.model, glm::vec3(distance / 30, 0.0f, 0.0f));
        uniform.model = glm::rotate(uniform.model, glm::radians(2.0f), glm::vec3(0.0f, 0.5f, 0.5f));

        color.r = sin(Engine::Get().GetTime().GetTime() * 2.6) * 0.5f + 0.5;
        color.g = cos(Engine::Get().GetTime().GetTime() * 3.2) * 0.5f + 0.5;
        color.b = 1 - (sin(Engine::Get().GetTime().GetTime() * 1.4) * 0.5f + 0.5);

        // RENDER
        Engine::Get().GetRenderer().BeginFrame();

        Engine::Get().GetRenderer().SetPipeline(*pipeline);
        Engine::Get().GetRenderer().SetVertexBuffer(*vb);

        Engine::Get().GetRenderer().SetVertexUniform(0, &uniform, sizeof(uniform));

        //Engine::Get().GetRenderer().SetVertexUniform(0, &scale, sizeof(scale));
        Engine::Get().GetRenderer().SetFragmentUniform(0, &color, sizeof(color));

        Engine::Get().GetRenderer().Draw(vb->GetVertexCount());

        Engine::Get().GetRenderer().EndFrame();
    }

    Engine::Get().Shutdown();

    return 0;
}