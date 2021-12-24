#include "RenderPass.h"

#include <cassert>

#include "Surface.h"

namespace Lumen::Graphics
{
	RenderPass* (*RenderPass::sCreateFunc)(Surface*) = nullptr;

	RenderPass* RenderPass::Create(Surface* target)
	{
		assert(sCreateFunc && "No graphics api set!");
		return sCreateFunc(target);
	}
}
