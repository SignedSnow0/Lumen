#include "VkInterface.h"

#include "VkContext.h"
#include "VkSurface.h"
#include "VkRenderPass.h"
#include "VkDescriptorSet.h"
#include "VkPipeline.h"
#include "VkShader.h"

namespace Lumen::Graphics::Vulkan
{

	/**
	 * @brief Used to set the current graphics api to vulkan.\n
	 * After this call each class in Graphics/Rhi with Class::Create will return this api`s class
	 */
	void SetApiInterface()
	{
		VkContext::SetInterface();
		VkSurface::SetInterface();
		VkRenderPass::SetInterface();
		VkDescriptorSet::SetInterface();
		VkShader::SetInterface();
		VkPipeline::SetInterface();
	}
}
