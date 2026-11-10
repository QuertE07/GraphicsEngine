#pragma once
#include "Framework/Component.h"
#include <glm/glm.hpp>

namespace gl
{
	class CameraComponent : public Component
	{
	public:
		CLASS_PROTOTYPE(CameraComponent)

		void Start() override;
		void Update(float dt) override;

		virtual void Read(const json::value_t& value) override;

		const glm::mat4x4& GetView() const { return m_view; }
		const glm::mat4x4& GetProjection() const { return m_projection; }
		const float& GetFov() const { return m_fov; }


	private:
		glm::mat4x4 m_view;
		glm::mat4x4 m_projection;

		float m_fov = 70.0f;
		float m_nearPlane = 0.001f;
		float m_farPlane = 300.0f;
		float m_aspectRatio = 0.5f;
	};
}