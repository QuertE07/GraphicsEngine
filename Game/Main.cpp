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

int main()
{
    SetWorkingDirectory("Assets");
    if (!Engine::Get().Initialize()) return 0;

    Scene scene;
    scene.Load("scenes/scene.json");
    
    auto& renderer = Engine::Get().GetRenderer();

    auto candle0 = Factory::Instance().Create<Actor>("CandlePrototype");
    candle0->SetPosition({ -2.54, 4.1, -5.1 });
    auto candle1 = Factory::Instance().Create<Actor>("CandlePrototype");
    candle1->SetPosition({ 2.52, 4.1, -5.1 });
    auto candle2 = Factory::Instance().Create<Actor>("CandlePrototype");
    candle2->SetPosition({ 3.95, 2.1, -2.53 });
    auto candle3 = Factory::Instance().Create<Actor>("CandlePrototype");
    candle3->SetPosition({ -3.82, 2.1, -2.53 });

    scene.AddActor(std::move(candle0));
    scene.AddActor(std::move(candle1));
    scene.AddActor(std::move(candle2));
    scene.AddActor(std::move(candle3));

    bool quit = false;
    while (!quit) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            Engine::Get().GetEditor().ProcessEvent(event);

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

        // EDITOR
        Engine::Get().GetEditor().BeginFrame();
        Engine::Get().GetEditor().Update();

        // RENDER
        renderer.BeginFrame();

        scene.Draw(renderer);

        renderer.EndRenderPass();
        Engine::Get().GetEditor().Draw(renderer);

        renderer.EndFrame();
    }

    Engine::Get().Shutdown();

    return 0;
}