#pragma once
#include "Texture.h"

#include <vector>
#include <memory>

namespace gl
{
	struct Particle
	{
		bool active{ false };
		float lifespan{ 1 };

		glm::vec2 position{ 0, 0 };
		glm::vec2 velocity{ 0, 0 };

		std::shared_ptr<Texture> sprite;
	};

	class ParticleSystem
	{
	public:
		ParticleSystem() = default;

		bool Initialize(size_t poolSize = 1000);
		void Shutdown();

		void Update(float dt);
		void Draw(const class Renderer& renderer);

		void AddParticle(const Particle& particle);

	private:
		Particle* GetFreeParticle();

	private:
		// store particles in particle pool
		std::vector<Particle> m_particles;
	};
}