#include "RenderPass.h"

#include <cassert>

namespace Lumen::Graphics
{
	RenderPass* (*RenderPass::sCreateFunc)(const std::vector<Attachment>&, Surface*) = nullptr;

	RenderPass* RenderPass::Create(const std::vector<Attachment>& attachments, Surface* target)
	{
		assert(sCreateFunc && "No graphics api set!");
		return sCreateFunc(attachments, target);
	}
}
