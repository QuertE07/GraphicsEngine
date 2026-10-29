#pragma once

#include "Object.h"
#include "Framework/Component.h"
#include "Math/Transform.h"
#include "Renderer/Texture.h"
#include <string>
#include <vector>

namespace gl
{
    class Scene;

    struct ActorDesc
    {
        std::string name;
        std::string tag;
        Transform transform {};
        glm::vec2 velocity = { 0.0f, 0.0f };
        float lifespan = 0.0f;
        bool persistent = false;
        std::shared_ptr<Texture> sprite;
    };

    class Actor : public Object
    {
    public:
        Actor() = default;
        Actor(const ActorDesc& actorDesc) :
            m_tag{ actorDesc.tag },
            m_transform{ actorDesc.transform },
            m_lifespan{ actorDesc.lifespan },
            m_persistent{ actorDesc.persistent }
        {}

        Actor(const Transform& transform, const std::shared_ptr<Texture> sprite) : m_transform{ transform } {}

        Actor(const Actor& other);

        CLASS_PROTOTYPE(Actor)

        virtual void Update(float dt);
        virtual void Draw(const class Renderer& renderer) const;

        virtual void Start();
        virtual void OnDestroy();

        virtual void OnCollision(Actor* other) {}

        const Transform& GetTransform() const { return m_transform; }
        void SetTransform(const Transform& transform) { m_transform = transform; }
        void SetPosition(const glm::vec3& position) { m_transform.position = position; }
        void SetRotation(const glm::vec3& rotation) { m_transform.rotation = rotation; }
        void SetScale(const glm::vec3& scale) { m_transform.scale = scale; }

        const std::string& GetName() const { return m_name; }

        const std::string& GetTag() const { return m_tag; }
        void SetTag(const std::string& tag) { m_tag = tag; }

        Scene* GetScene() { return m_scene; }

        void SetDestroyed(bool destroy = true) { m_destroyed = destroy; }
        bool GetDestroyed() const { return m_destroyed; }

        bool GetPersistent() const { return m_persistent; }

        virtual void Read(const json::value_t& value);

        void AddComponent(std::unique_ptr<Component> component);

        template<std::derived_from<Component> T>
        T* GetComponent();

        friend Scene;

    protected:
        std::string m_tag;

        Transform m_transform;
        float m_lifespan{ 0 };
        bool m_destroyed{ false };
        bool m_persistent = 0;

        std::vector<std::unique_ptr<Component>> m_components;

        Scene* m_scene = nullptr;
    };

    template<std::derived_from<Component> T>
    inline T* Actor::GetComponent()
    {
        for (auto& component : m_components)
        {
            auto result = dynamic_cast<T*>(component.get());
            if (result) return result;
        }

        return nullptr;
    }
}