#include "RenderPass.h"

#include <cassert>

namespace Lumen::Graphics
{
	RenderPass* (*RenderPass::sCreateFunc)(Surface*) = nullptr;

	RenderPass* RenderPass::Create(Surface* target)
	{
		assert(sCreateFunc && "No graphics api set!");
		return sCreateFunc(target);
	}
}
