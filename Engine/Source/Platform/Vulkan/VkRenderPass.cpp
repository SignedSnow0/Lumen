#include "VkRenderPass.h"

#include "VkContext.h"
#include "VkSurface.h"
#include "VkTexture.h"

namespace Lumen::Graphics::Vulkan
{
	namespace 
	{
		VkFormat FormatToVk(AttachmentFormat format)
		{
			switch (format)
			{
				case AttachmentFormat::B8G8R8A8Srgb: return VK_FORMAT_B8G8R8A8_SRGB;
				case AttachmentFormat::R8G8B8A8Srgb: return VK_FORMAT_R8G8B8A8_SRGB;
				case AttachmentFormat::D32SFloat: return VK_FORMAT_D32_SFLOAT;
				case AttachmentFormat::D24UNormS8Uint: return VK_FORMAT_D24_UNORM_S8_UINT;
				default: return VK_FORMAT_R8G8B8A8_SRGB;
			}
		}

		VkImageLayout LayoutToVk(AttachmentUsage usage)
		{
			switch (usage)
			{
				case AttachmentUsage::Color: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				case AttachmentUsage::Depth: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				default: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
		}
		
		RenderPass* CreateFuncVulkan(const std::vector<Attachment>& attachments, Surface* target)
		{
			std::vector<VkAttachment> vkAttachments{ attachments.size() };
			u32 i{ 0 };
			for (auto& attachment : attachments)
			{
				VkAttachment vkAttachment{};
				switch (attachment.Usage)
				{
				case AttachmentUsage::Color:
						 vkAttachment.AsColor(FormatToVk(attachment.Format), target ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : LayoutToVk(attachment.Usage));
						break;
					case AttachmentUsage::Depth:
						vkAttachment.AsDepth(FormatToVk(attachment.Format), LayoutToVk(attachment.Usage));
						break;
					case AttachmentUsage::Count:
						break;
				}
				vkAttachments[i] = vkAttachment;
				
				i++;
			}
			return new VkRenderPass{ vkAttachments, dynamic_cast<VkSurface*>(target) };
		}
	}

	/**
	 * @brief Fills the structure with default settings for a color attachment
	 * @param format The attachment`s format used
	 */
	void VkAttachment::AsColor(VkFormat format, VkImageLayout layout)
	{
		Description.format			= format;
		Description.samples			= VK_SAMPLE_COUNT_1_BIT;
		Description.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
		Description.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
		Description.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		Description.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		Description.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		Description.finalLayout		= layout;

		Reference.layout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	void VkAttachment::AsDepth(VkFormat format, VkImageLayout layout)
	{
		Description.format			= format;
		Description.samples			= VK_SAMPLE_COUNT_1_BIT;
		Description.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
		Description.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
		Description.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		Description.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		Description.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		Description.finalLayout		= layout;

		Reference.layout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	VkRenderPass::VkRenderPass(const std::vector<VkAttachment>& attachments, VkSurface* target)
		: mAttachments{ attachments }, mTarget{ target }
	{
		Init();
	}

	void VkRenderPass::Init()
	{
		std::vector<VkAttachmentDescription> descriptions{ mAttachments.size() };
		std::vector<VkAttachmentReference> colorRefs{};
		std::optional<VkAttachmentReference> depthRef;
		u32 i{ 0 };
		for (auto& description : mAttachments)
		{
			descriptions[i] = description.Description;
			description.Reference.attachment = i;
			
			if (description.Reference.layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
				colorRefs.push_back(description.Reference);
			else if (description.Reference.layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
				depthRef = description.Reference;
			i++;
		}

		const b8 hasDepth{ depthRef.has_value() };
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount	= static_cast<u32>(colorRefs.size());
		subpass.pColorAttachments		= colorRefs.data();
		subpass.pDepthStencilAttachment = hasDepth ? &depthRef.value() : nullptr;

		VkSubpassDependency dependency{};
		dependency.srcSubpass		= VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass		= 0;
		dependency.srcStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | (hasDepth ? VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT : 0);
		dependency.dstStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | (hasDepth ? VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT : 0);
		dependency.srcAccessMask	= 0;
		dependency.dstAccessMask	= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | (hasDepth ? VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT : 0);
		
		VkRenderPassCreateInfo createInfo{};
		createInfo.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.attachmentCount	= static_cast<u32>(mAttachments.size());
		createInfo.pAttachments		= descriptions.data();
		createInfo.subpassCount		= 1;
		createInfo.pSubpasses		= &subpass;
		createInfo.dependencyCount	= 1;
		createInfo.pDependencies	= &dependency;

		VK_ASSERT(vkCreateRenderPass(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mRenderPass), "Failed to create render pass");

		if (!mTarget)
			mColorTextures.resize(VkSurface::BufferCount);
		if (hasDepth)
			mDepthTextures.resize(VkSurface::BufferCount);
		
		if (mTarget)
			CreateFramebuffers(mTarget->Width(), mTarget->Height());
		else
			CreateFramebuffers(1920, 1080);
	}

	/**
	 * @brief Destroys all the class` resources
	 */
	void VkRenderPass::Release()
	{
		for (const auto& framebuffer : mFramebuffers)
			vkDestroyFramebuffer(VkContext::Get().LogicalDevice(), framebuffer, nullptr);

		vkDestroyRenderPass(VkContext::Get().LogicalDevice(), mRenderPass, nullptr);

		for (const auto* texture : mDepthTextures)
			delete texture;
		
		for (const auto* texture : mColorTextures)
			delete texture;
	}

	/**
	 * @brief Sets the current target for all the future draw calls
	 * @param frame Frame index to record commands to
	 */
	void VkRenderPass::Begin(u32 frame, Surface* target)
	{
		if (mTarget)
		{
			if (mSize.first != mTarget->Width() || mSize.second != mTarget->Height())
				Resize(mTarget->Width(), mTarget->Height());
		}
		
		std::vector<VkClearValue> clearValues{ 2 };
		clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clearValues[1].depthStencil = { 1.0f, 0 };
		
		VkRenderPassBeginInfo beginInfo{};
		beginInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		beginInfo.renderPass		= mRenderPass;
		beginInfo.framebuffer		= mFramebuffers[frame];
		beginInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		beginInfo.renderArea.extent = VkExtent2D{ mSize.first, mSize.second };
		beginInfo.clearValueCount	= static_cast<u32>(clearValues.size());
		beginInfo.pClearValues		= clearValues.data();

		vkCmdBeginRenderPass(target ? dynamic_cast<VkSurface*>(target)->CommandBuffer() : mTarget->CommandBuffer(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);//todo clean this
	}

	/**
	 * @brief Unbinds the current target from future draw calls
	 */
	void VkRenderPass::End(Surface* target)
	{
		vkCmdEndRenderPass(target ? dynamic_cast<VkSurface*>(target)->CommandBuffer() : mTarget->CommandBuffer());
	}

	void VkRenderPass::Resize(u32 width, u32 height)
	{
		for (u32 i{ 0 }; i < mFramebuffers.size(); i++)
			vkDestroyFramebuffer(VkContext::Get().LogicalDevice(), mFramebuffers[i], nullptr);

		CreateFramebuffers(width, height);
	}

	void VkRenderPass::SetInterface()
	{
		sCreateFunc = CreateFuncVulkan;
	}

	void VkRenderPass::CreateFramebuffers(u32 width, u32 height)
	{
		mFramebuffers.resize(VkSurface::BufferCount);
		
		if (!mTarget)
		{
			for (auto& texture : mColorTextures)
			{
				texture = new VkTexture{ width, height, TextureUsage::RenderTarget };
			}
		}

		if (!mDepthTextures.empty())
		{
			for(auto& texture : mDepthTextures)
			{
				texture = new VkTexture{ width, height, TextureUsage::DepthTarget };
			}
		}

		for (u32 i{ 0 }; i < VkSurface::BufferCount; i++)
		{
			std::vector<VkImageView> views{};
			views.push_back(mTarget ? mTarget->ImageView(i) : mColorTextures[i]->View());
			if (!mDepthTextures.empty())
				views.push_back(mDepthTextures[i]->View());
			
			VkFramebufferCreateInfo createInfo{};
			createInfo.sType			= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			createInfo.renderPass		= mRenderPass;
			createInfo.attachmentCount	= static_cast<u32>(views.size());
			createInfo.pAttachments		= views.data();
			createInfo.width			= width;
			createInfo.height			= height;
			createInfo.layers			= 1;

			VK_ASSERT(vkCreateFramebuffer(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mFramebuffers[i]), "Failed to create framebuffer");
		}

		mSize.first = width;
		mSize.second = height;
	}
}
