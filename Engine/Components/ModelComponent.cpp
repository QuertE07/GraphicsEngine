#include "pch.h"
#include "ModelComponent.h"
#include "Engine.h"

namespace gl
{
	FACTORY_REGISTER(ModelComponent)

	void ModelComponent::Draw(const Renderer& renderer)
	{
		if (!m_model)
		{
			std::cerr << "Could not draw model for actor " << m_owner->GetName() << ".\n";
			return;
		}

		ObjectUniform objectUniform
		{
			.model = m_owner->GetTransform().GetMatrix(),
			.normalMatrix = glm::transpose(glm::inverse(m_owner->GetTransform().GetMatrix()))
		};

		renderer.SetVertexUniform(UniformSlot::Object, &objectUniform, sizeof(objectUniform));
		renderer.SetFragmentUniform(UniformSlot::Object, &objectUniform, sizeof(objectUniform));

		m_model->Draw(renderer, m_materials);
	}

	void ModelComponent::Read(const json::value_t& value)
	{
		RendererComponent::Read(value);

		std::string modelName;
		JSON_READ_NAME_REQ(value, "model", modelName);

		m_model = Resources().Get<Model>(modelName, Engine::Get().GetRenderer());

		// "materials": [ "materials/brick.instance.json", null ] - one entry per slot, null keeps the model's material
		if (JSON_HAS_NAME(value, "materials"))
		{
			for (const auto& entry : JSON_GET_NAME(value, "materials").GetArray())
			{
				if (entry.IsString())
				{
					auto material = Resources().Get<MaterialInstance>(entry.GetString(), Engine::Get().GetRenderer());
					if (!material) std::cerr << "Could not load material: " << entry.GetString() << std::endl;
					m_materials.push_back(material);
				}
				else
				{
					m_materials.push_back(nullptr);
				}
			}
		}

	}
}