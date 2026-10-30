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

#define CHECK_UNIFORM_BOUNDARY(uniform) static_assert(sizeof(uniform) % 16 == 0)
#define MAX_LIGHTS 8

using namespace gl;

enum class LightType : uint32_t
{
    Directional,
    Point,
    Spot
};

namespace gl {
    struct Color4
    {
        float r, g, b, a;
    };
}

struct Vertex
{
    glm::vec3 position;
    Color4 color;
};

struct VertexUniform
{
    glm::mat4 mvp;
    glm::mat4 model;
    glm::vec3 cameraPosition;
    float padding1;
};

struct Light
{
    glm::vec3 position;
    uint32_t type;
    glm::vec3 direction;
    float range;
    glm::vec3 color;
    float intensity;
    float innerCutoff;
    float outerCutoff;
    glm::vec2 padding;
};

struct LightUniform
{
    Light lights[MAX_LIGHTS];
    uint32_t numLights;
    glm::vec3 ambient;
};

CHECK_UNIFORM_BOUNDARY(LightUniform);

struct MaterialUniform
{
    glm::vec4 color;
    glm::vec2 tiling;
    glm::vec2 offset;
    float smoothness;
    glm::vec3 padding;
};

CHECK_UNIFORM_BOUNDARY(MaterialUniform);

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

    auto model = Resources().Get<Model>("models/sphere.obj", Engine::Get().GetRenderer());
    auto texture = Resources().Get<Texture>("textures/gray_diffuse.png", Engine::Get().GetRenderer());

    auto vshader = Resources().Get<gl::Shader>("shaders/phong.vert", Engine::Get().GetRenderer(), 3);
    auto fshader = Resources().Get<gl::Shader>("shaders/phong.frag", Engine::Get().GetRenderer(), 3, 1);

    auto pipeline = std::make_shared<Pipeline>();

    pipeline->AddVertexBuffer(sizeof(Model::Vertex));
    pipeline->AddVertexAttribute(0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, position));
    pipeline->AddVertexAttribute(1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Model::Vertex, texcoord));
    pipeline->AddVertexAttribute(2, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, normal));
    pipeline->AddVertexAttribute(3, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, tangent));

    pipeline->Create(*vshader.get(), *fshader.get(),
        Engine::Get().GetRenderer().GetGPUDevice(),
        Engine::Get().GetRenderer().GetWindow());

    // UNIFORMS

    VertexUniform vertexUniform;

    Light light0{
        .position{ 0.5f, 5, 2 },
        .type = (uint32_t)LightType::Point,
        .range{ 20.0f },
        .color{ 0.3f, 1, 0.3f },
        .intensity{ 1 }
    };

    Light light1{
        .position{ 2, 2, 4 },
        .type = (uint32_t)LightType::Spot,
        .direction{ -0.5, -0.3, -1 },
        .range{ 120.0f },
        .color{ 0.1f, 0.1f, 1 },
        .intensity{ 1.5f },
        .innerCutoff = glm::cos(glm::radians(40.0f)),
        .outerCutoff = glm::cos(glm::radians(45.0f))
    };

    Light light2{
        .position{ 0, -1, 0 },
        .type = (uint32_t)LightType::Directional,
        .direction{ 0, 1, 0 },
        .color{ 1, 0.3f, 0.3f },
        .intensity{ 1 }
    };

    LightUniform lightUniform{
        .lights = { light0, light1, light2 },
        .numLights = 3,
        .ambient = { 0.4f, 0.4f, 0.4f }
    };

    MaterialUniform materialUniform{
        .color{ 1, 1, 1, 1 },
        .tiling{ 1.0f, 1.0f },
        .offset{ 0.0f, 0.0f },
        .smoothness = 20
    };

    glm::vec3 cameraPosition{ 0, 0, 5 };

    Transform transform;
    transform.scale = { 4, 4, 4 };

    glm::vec3 cameraRotation{ 0, 0, 0 };
    float pitch = 0.0f;
    float yaw = Pi;

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

        direction.x = (float)((Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_D)) - (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_A)));
        direction.z = (float)((Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_S)) - (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_W)));
        direction.y = (float)((Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_SPACE)) - (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_LSHIFT)));

        pitch += ((float)(Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_UP)) - (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_DOWN))) * dt;
        yaw += ((float)(Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_LEFT)) - (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_RIGHT))) * dt;
        SDL_clamp(pitch, -(Pi / 2), (Pi / 2));
        if (pitch > (Pi / 2) - 0.01f) pitch = (Pi / 2) - 0.01f;
        if (pitch < -(Pi / 2) + 0.01f) pitch = -(Pi / 2) + 0.01f;
        cameraRotation = glm::vec3(cosf(pitch) * sinf(yaw), sinf(pitch), cosf(pitch) * cosf(yaw));

        cameraPosition += direction * 6.0f * dt;
        
        //std::cout << cameraPosition.x << ", " << cameraPosition.y << ", " << cameraPosition.z << std::endl;

        //if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_1)) 
        //{
        //    lightUniform.lights[1].type = (uint32_t)LightType::Directional;
        //    lightUniform.lights[1].direction = { -0.5 , -1, 0.2f };
        //}
        //if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_2))
        //{
        //    lightUniform.lights[1].type = (uint32_t)LightType::Point;
        //    lightUniform.lights[1].position = { 0.5f, 6, 2 };
        //}
        //if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_3))
        //{
        //    lightUniform.lights[1].type = (uint32_t)LightType::Spot;
        //    lightUniform.lights[1].position = { 0, 3.5, 3.5 };
        //    lightUniform.lights[1].direction = { 0 , -1, -1 };
        //    lightUniform.lights[1].innerCutoff = glm::cos(glm::radians(3.0f));
        //    lightUniform.lights[1].outerCutoff = glm::cos(glm::radians(4.0f));
        //}

        // TRANSFORM
        transform.rotation.x = time * 90;

        // VIEW MATRIX
        auto view = glm::lookAtRH(cameraPosition, cameraPosition + cameraRotation, glm::vec3{ 0, 1, 0 });

        // PROJECTION MATRIX
        float aspectRatio = (float)Engine::Get().GetRenderer().GetWidth() / Engine::Get().GetRenderer().GetHeight();
        auto projection = glm::perspective(glm::radians(70.0f), aspectRatio, 0.01f, 20.0f);

        // MVP
        vertexUniform.model = transform.GetMatrix();
        vertexUniform.mvp = projection * view * vertexUniform.model;
        vertexUniform.cameraPosition = cameraPosition;

        // RENDER
        Engine::Get().GetRenderer().BeginFrame();
        Engine::Get().GetRenderer().SetPipeline(*pipeline);

        Engine::Get().GetRenderer().SetVertexUniform(0, &vertexUniform, sizeof(vertexUniform));
        Engine::Get().GetRenderer().SetVertexUniform(1, &lightUniform, sizeof(lightUniform));
        Engine::Get().GetRenderer().SetFragmentUniform(0, &vertexUniform, sizeof(vertexUniform));
        Engine::Get().GetRenderer().SetFragmentUniform(1, &lightUniform, sizeof(lightUniform));

        Engine::Get().GetRenderer().SetVertexUniform(2, &materialUniform, sizeof(materialUniform));
        Engine::Get().GetRenderer().SetFragmentUniform(2, &materialUniform, sizeof(materialUniform));
        Engine::Get().GetRenderer().SetFragmentTexture(0, *texture);

        //Engine::Get().GetRenderer().SetVertexUniform(2, &materialUniform, sizeof(materialUniform));
        //Engine::Get().GetRenderer().SetFragmentUniform(2, &materialUniform, sizeof(materialUniform));
        //Engine::Get().GetRenderer().SetFragmentTexture(0, *texture);

        model->Draw(Engine::Get().GetRenderer());

        Engine::Get().GetRenderer().EndFrame();
    }

    Engine::Get().Shutdown();

    return 0;
}