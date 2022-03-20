#pragma once
#include "Core/Types.h"

namespace Lumen::Graphics
{
	class Surface;

	class RenderPass
	{
	public:
		static RenderPass* Create(Surface* target = nullptr);
		virtual ~RenderPass() = default;

		virtual void Init() = 0;
		virtual void Release() = 0;

		virtual void Begin(u32 frame, Surface* target = nullptr) = 0;
		virtual void End(Surface* target = nullptr) = 0;
		virtual void Resize(u32 width, u32 height) = 0;

	protected:
		static RenderPass* (*sCreateFunc)(Surface*);
	};
}
