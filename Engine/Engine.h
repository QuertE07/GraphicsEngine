#pragma once

#include "Core/EngineTime.h"
#include "Core/Factory.h"
#include "Core/File.h"
#include "Core/Random.h"
#include "Core/Singleton.h"
#include "Core/StringUtils.h"

#include "Renderer/Font.h"
#include "Renderer/Model.h"
#include "Renderer/ParticleSystem.h"
#include "Renderer/Renderer.h"
#include "Renderer/Text.h"
#include "Renderer/Texture.h"

#include "Math/MathUtils.h"
#include "Math/Transform.h"

#include "Framework/Actor.h"
#include "Framework/Scene.h"
#include "Framework/Game.h"

#include "Resources/ResourceManager.h"

#include "Serialization/Json.h"
#include "Input/Input.h"
#include "Audio/Audio.h"

#include <iostream>
#include <vector>


namespace gl
{
	class Engine
	{
	public:
		static Engine& Get() { static Engine engine; return engine; }

		bool Initialize();
		void Shutdown();
		
		void Update();

		Input& GetInput() { return m_input; }
		Renderer& GetRenderer() { return m_renderer; }
		Time& GetTime() { return m_time; }
		ParticleSystem& GetPS() { return m_particleSystem; }
		Audio& GetAudio() { return m_audio; }

	private:
		Engine() = default;

		Input m_input;
		Renderer m_renderer;

		Time m_time;

		ParticleSystem m_particleSystem;
		Audio m_audio;
	};
}