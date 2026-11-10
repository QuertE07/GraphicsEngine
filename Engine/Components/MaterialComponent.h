#pragma once
#include "RendererComponent.h"

namespace gl
{
	class MaterialComponent : public RendererComponent
	{
	public:
		CLASS_PROTOTYPE(MaterialComponent)

			void Draw(const Renderer& renderer) override;

	private:
	};
}