#include "pch.h"
#include "Actor.h"
#include "Engine.h" 
#include "Components/RendererComponent.h"

namespace gl
{
    FACTORY_REGISTER(Actor)

    Actor::Actor(const Actor& other) :
        Object{ other },
        m_tag{ other.m_tag },
        m_transform{ other.m_transform },
        m_lifespan{ other.m_lifespan }
    {
        for (auto& component : other.m_components)
        {
            auto clone = std::unique_ptr<Component>(dynamic_cast<Component*>(component->Clone().release()));
            AddComponent(std::move(clone));
        }
    }

    void Actor::Update(float dt)
    {
        if (m_lifespan > 0.0f) {
            m_lifespan -= dt;
            m_destroyed = (m_lifespan <= 0.0f);
        }

        for (auto& component : m_components)
        {
            if (component->IsActive()) component->Update(dt);
        }

        //m_transform.position += (m_velocity * dt);
        //m_velocity *= 0.9f;
    }

    void Actor::Draw(const Renderer& renderer) const
    {
        for (auto& component : m_components)
        {
            auto rendererComponent = dynamic_cast<RendererComponent*>(component.get());
            if (rendererComponent)
            {
                if (rendererComponent->IsActive()) rendererComponent->Draw(renderer);
            }
        }
    }

    void Actor::Start()
    {
        for (auto& component : m_components)
        {
            component->Start();
        }
    }

    void Actor::OnDestroy()
    {
        for (auto& component : m_components)
        {
            component->OnDestroy();
        }
    }

    void Actor::AddComponent(std::unique_ptr<Component> component)
    {
        component->SetOwner(this);
        m_components.push_back(std::move(component));
    }

    void Actor::Read(const json::value_t& value)
    {
        Object::Read(value);

        if (JSON_HAS_NAME(value, "transform"))
        {
            m_transform.Read(JSON_GET_NAME(value, "transform"));
        }

        JSON_READ_NAME(value, "tag", m_tag);
        JSON_READ_NAME(value, "lifespan", m_lifespan);
        JSON_READ_NAME(value, "persistent", m_persistent);

        if (JSON_HAS_NAME(value, "components"))
        {
            for (auto& componentValue : JSON_GET_NAME(value, "components").GetArray())
            {
                std::string typeName;
                JSON_READ_NAME(componentValue, "type", typeName);

                auto component = Factory::Instance().Create<Component>(typeName);
                if (component) {
                    component->Read(componentValue);
                    AddComponent(std::move(component));
                }
            }
        }
    }

    void Actor::UpdateGui()
    {
        ImGui::Checkbox("##Active", &m_active);
        ImGui::SameLine();
        
        Object::UpdateGui();

        if (!m_tag.empty()) ImGui::Text("(%s)", m_tag.c_str());
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            m_transform.UpdateGui();
        }

        // components information
        for (auto& component : m_components)
        {
            ImGui::PushID(component.get());
            ImGui::Separator();

            bool active = component->IsActive();
            if (ImGui::Checkbox("##active", &active)) component->SetActive(active);
            ImGui::SameLine();

            if (ImGui::CollapsingHeader(component->GetClassName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                component->UpdateGui();
            }
            ImGui::PopID();
        }
    }
}