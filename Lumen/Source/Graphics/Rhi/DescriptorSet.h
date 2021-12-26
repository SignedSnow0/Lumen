#pragma once
#include "Core/Types.h"

namespace Lumen::Graphics
{
	class Texture;
	class Shader;

	class DescriptorSet
	{
	public:
		static DescriptorSet* Create(Shader* shader, u32 setIndex);
		virtual ~DescriptorSet() = default;

		virtual void Init() = 0;
		virtual void Release() = 0;

		virtual void Update(u32 frame) = 0;
		virtual void UpdateUniform(u32 binding, const void* data, u32 frame) = 0;
		virtual void SetTexture(u32 binding, Texture* texture) = 0;

	protected:
		static DescriptorSet* (*sCreateFunc)(Shader*, u32);
	};
}
