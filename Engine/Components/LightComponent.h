#pragma once
#include "RendererComponent.h"
#include "Renderer/Light.h"

namespace gl
{
	class LightComponent : public Component
	{
	public:
		CLASS_PROTOTYPE(LightComponent)

		void Read(const json::value_t& value) override;

		LightData GetLightData() const;

	private:
		LightType m_type = LightType::Point;
		glm::vec3 m_color{ 1.0f };
		float m_intensity = 1.0f;
		float m_range = 20.0f;
		float m_innerAngle = 20.0f;
		float m_outerAngle = 30.0f;
	};
}