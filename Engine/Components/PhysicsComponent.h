#pragma once
#include "Framework/Component.h"

namespace gl
{
	class PhysicsComponent : public Component
	{
	public:
		virtual void ApplyForce(const glm::vec2& force) = 0;
		virtual void SetVelocity(const glm::vec2& velocity) = 0;
		virtual glm::vec2 GetVelocity() = 0;

		virtual void ApplyTorque(float torque) = 0;
		virtual void SetAngularVelocity(float angularVelocity) = 0;
		virtual float GetAngularVelocity() const = 0;

		virtual void SetPosition(const glm::vec2& position) = 0;
		virtual glm::vec2 GetPosition() const = 0;

		virtual void SetRotation(const float rotation) = 0;
		virtual float GetRotation() const = 0;

		virtual void Read(const json::value_t& value);

	protected:
		float m_mass{ 1.0f };
		float m_damping{ 0.0f };
		float m_angularDamping{ 0.0f };
		float m_gravityScale{ 1.0f };
	};
}