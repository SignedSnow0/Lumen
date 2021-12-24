#pragma once
#include "Core/Types.h"

namespace Lumen::Graphics
{
	class Surface;

	class RenderPass
	{
	public:
		static RenderPass* Create(Surface* target);
		virtual ~RenderPass() = default;

		virtual void Init() = 0;
		virtual void Release() = 0;

		virtual void Begin(u32 frame) = 0;
		virtual void End() = 0;

	protected:
		static RenderPass* (*sCreateFunc)(Surface*);
	};
}
