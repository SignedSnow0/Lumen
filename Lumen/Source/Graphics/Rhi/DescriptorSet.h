#pragma once
#include "Core/Types.h"

namespace Lumen::Graphics
{
	class Shader;

	class DescriptorSet
	{
	public:
		static DescriptorSet* Create(Shader* shader, u32 setIndex);
		virtual ~DescriptorSet() = default;

		virtual void Init() = 0;
		virtual void Release() = 0;

		virtual void Update() = 0;
		virtual void SetUniform(u32 binding, const void* data) = 0;

	protected:
		static DescriptorSet* (*sCreateFunc)(Shader*, u32);
	};
}
