#pragma once
#include "pch.h"
#include "ControllerComponent.h"
#include "Engine.h"
#include <glm/glm.hpp>  
#include <glm/gtx/norm.hpp>

namespace gl
{
    FACTORY_REGISTER(ControllerComponent)

	void ControllerComponent::Update(float dt)
	{
        // get camera rotation
        if (Engine::Get().GetInput().GetButtonDown(Input::MouseButton::Right))
        {
            glm::vec2 axis = Engine::Get().GetInput().GetMouseDelta() * m_sensitivity;

            m_yaw -= axis.x;
            m_pitch -= axis.y;
            m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);
        }

        //owner->transform.SetRotation(glm::vec3(pitch, yaw, 0.0f));

        glm::quat q = glm::quat_cast(glm::eulerAngleYXZ(glm::radians(m_yaw), glm::radians(m_pitch), 0.0f));
        m_owner->SetRotation(q);

        //m_owner->SetRotation({m_yaw, m_pitch, 0});

        glm::vec3 direction{ 0 };
        direction += (float)(Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_D) - Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_A)) * m_owner->GetTransform().Right();
        direction += (float)(Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_SPACE) - Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_LSHIFT)) * m_owner->GetTransform().Up();
        direction += (float)(Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_W) - Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_S)) * m_owner->GetTransform().Forward();
        //if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_A)) direction -= m_owner->GetTransform().Right();
        //if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_D)) direction += m_owner->GetTransform().Right();
        //if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_E)) direction += m_owner->GetTransform().Up();
        //if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_Q)) direction -= m_owner->GetTransform().Up();
        //if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_W)) direction += m_owner->GetTransform().Forward();
        //if (Engine::Get().GetInput().GetKeyDown(SDL_SCANCODE_S)) direction -= m_owner->GetTransform().Forward();;

        if (glm::length2(direction) > 0.0f) {
            direction = glm::normalize(direction);
        }
        m_owner->SetPosition(m_owner->GetTransform().position + direction * (m_speed * dt));

	}
    void ControllerComponent::Read(const json::value_t& value)
    {
        Component::Read(value);

        JSON_READ_NAME(value, "speed", m_speed);
        JSON_READ_NAME(value, "sensitivity", m_sensitivity);
    }
}
