#include "Engine.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Pipeline.h"
#include "Renderer/Uniforms.h"
#include "Components/CameraComponent.h"

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
    glm::vec3 position;
    Color4 color;
};

int main()
{
    SetWorkingDirectory("Assets");
    if (!Engine::Get().Initialize()) return 0;

    Scene scene;
    scene.Load("scenes/scene.json");
    
    auto& renderer = Engine::Get().GetRenderer();

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

        // SCENE
        scene.Update(dt);

        // RENDER
        renderer.BeginFrame();

        scene.Draw(renderer);

        renderer.EndFrame();
    }

    Engine::Get().Shutdown();

    return 0;
}