#include "pch.h"
#include "CameraComponent.h"
#include "Core/Factory.h"
#include "Framework/Actor.h"
#include "Engine.h"

namespace gl
{
	FACTORY_REGISTER(CameraComponent)

	void CameraComponent::Start()
	{
		m_aspectRatio = (float)Engine::Get().GetRenderer().GetWidth() / Engine::Get().GetRenderer().GetHeight();
	}

	void CameraComponent::Update(float dt)
	{
		glm::vec3 forward = m_owner->GetTransform().Forward();

		m_view = glm::lookAtRH(m_owner->GetTransform().position, m_owner->GetTransform().position + forward, glm::vec3{0, 1, 0});
		m_projection = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
	}

	void CameraComponent::Read(const json::value_t& value)
	{
		Component::Read(value);

		JSON_READ_NAME(value, "fov", m_fov);
		JSON_READ_NAME(value, "near_plane", m_nearPlane);
		JSON_READ_NAME(value, "far_plane", m_farPlane);
	}
}