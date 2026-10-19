#include "Engine.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Pipeline.h"

#include <fmod.hpp>
#include <map>
#include <memory>
#include <fstream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/transform.hpp"

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
    glm::mat4 mvp;
};

std::vector<Vertex> vertices =
{
    Vertex{ { 0.5f, -0.5f, 0.0f }, { 0.0, 1.0, 0.0, 1.0 } }, // Bottom-Right
    Vertex{ { 0.5f,  0.5f, 0.0f }, { 0.0, 0.0, 1.0, 1.0 } }, // Top-Right
    Vertex{ { -0.5f, -0.5f, 0.0f }, { 1.0, 0.0, 0.0, 1.0 } }, // Bottom-Left
    Vertex{ { -0.5f, 0.5f, 0.0f }, { 1.0, 1.0, 0.0, 1.0 } }, // Top-Left
};

std::vector<uint32_t> indices = { 0, 1, 2, 2, 1, 3 };

int main()
{
    SetWorkingDirectory("Assets");
    if (Engine::Get().Initialize() == false) return 0;

    auto vb = std::make_shared<VertexBuffer>();
    vb->Create<Vertex>(vertices, Engine::Get().GetRenderer());

    auto ib = std::make_shared<IndexBuffer>();
    ib->Create(indices, Engine::Get().GetRenderer());

    auto vshader = Resources().Get<gl::Shader>("shaders/position.vert", Engine::Get().GetRenderer(), 1);
    auto fshader = Resources().Get<gl::Shader>("shaders/color.frag", Engine::Get().GetRenderer(), 1);

    auto pipeline = std::make_shared<Pipeline>();
    pipeline->AddVertexBuffer(sizeof(Vertex));
    pipeline->AddVertexAttribute(0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Vertex, position));
    pipeline->AddVertexAttribute(1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4, offsetof(Vertex, color));

    pipeline->Create(*vshader.get(), *fshader.get(),
        Engine::Get().GetRenderer().GetGPUDevice(),
        Engine::Get().GetRenderer().GetWindow());

    vertexUniform uniform;
    glm::vec3 cameraPosition{ 0, 0, 5 };

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
        float time = Engine::Get().GetTime().GetTime();

        glm::vec3 direction{ 0 };
        if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_A)) direction.x = -1;
        if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_D)) direction.x = +1;
        if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_Q)) direction.y = -1;
        if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_E)) direction.y = +1;
        if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_W)) direction.z = -1;
        if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_S)) direction.z = +1;

        cameraPosition += direction * 4.0f * dt;

        // MODEL MATRIX
        auto mxt = glm::translate(glm::vec3( 0.0f, 0.0f, 0.0f));
        auto mxr = glm::eulerAngleXYZ(0.0f, 0.0f, 0.0f);
        auto mxs = glm::scale(glm::vec3{ 1 });

        auto model = mxt * mxr * mxs;

        // VIEW MATRIX
        auto view = glm::lookAtRH(cameraPosition, cameraPosition + glm::vec3{ 0, 0, -1 }, glm::vec3{ 0, 1, 0 });

        // PROJECTION MATRIX
        float aspectRatio = (float)Engine::Get().GetRenderer().GetWidth() / Engine::Get().GetRenderer().GetHeight();
        auto projection = glm::perspective(glm::radians(70.0f), aspectRatio, 0.01f, 20.0f);

        uniform.mvp = projection * view * model;

        // RENDER
        Engine::Get().GetRenderer().BeginFrame();

        Engine::Get().GetRenderer().SetPipeline(*pipeline);
        Engine::Get().GetRenderer().SetVertexBuffer(*vb);
        Engine::Get().GetRenderer().SetIndexBuffer(*ib);

        Engine::Get().GetRenderer().SetVertexUniform(0, &uniform, sizeof(uniform));

        Engine::Get().GetRenderer().DrawIndex(*ib);

        Engine::Get().GetRenderer().EndFrame();
    }

    Engine::Get().Shutdown();

    return 0;
}