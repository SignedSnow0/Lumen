#include "VkRenderPass.h"

#include "VkContext.h"
#include "VkSurface.h"

namespace Lumen::Graphics::Vulkan
{
	/**
	 * \brief Fills the structure with default settings for a color attachment
	 * \param format The attachment`s format used
	 */
	void Attachment::AsColor(VkFormat format)
	{
		Description.format = format;
		Description.samples = VK_SAMPLE_COUNT_1_BIT;
		Description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		Description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		Description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		Description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		Description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		Description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		Reference.attachment = 0;
		Reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		Subpass.colorAttachmentCount = 1;
		Subpass.pColorAttachments = &Reference;

		Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		Dependency.dstSubpass = 0;
		Dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		Dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		Dependency.srcAccessMask = 0;
		Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	VkRenderPass::VkRenderPass(std::vector<Attachment> attachments, VkSurface& target)
		: mAttachments{ std::move(attachments) }, mTarget{ &target }
	{
		Initialize();
	}

	void VkRenderPass::Initialize()
	{
		{
			std::vector<VkAttachmentDescription> descriptions{ mAttachments.size() };
			std::vector<VkSubpassDescription> subpasses{ mAttachments.size() };
			std::vector<VkSubpassDependency> dependencies{ mAttachments.size() };
			uint32_t i{ 0 };
			for (const auto& description : mAttachments)
			{
				descriptions[i] = description.Description;
				subpasses[i] = description.Subpass;
				dependencies[i] = description.Dependency;
				i++;
			}
			VkRenderPassCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			createInfo.attachmentCount = static_cast<uint32_t>(mAttachments.size());
			createInfo.pAttachments = descriptions.data();
			createInfo.subpassCount = static_cast<uint32_t>(mAttachments.size());
			createInfo.pSubpasses = subpasses.data();
			createInfo.dependencyCount = static_cast<uint32_t>(mAttachments.size());
			createInfo.pDependencies = dependencies.data();

			VK_ASSERT(vkCreateRenderPass(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mRenderPass), "Failed to create render pass");
		}
		CreateFramebuffers();
	}

	/**
	 * \brief Destroys all the class` resources
	 */
	void VkRenderPass::Release()
	{
		for (const auto& framebuffer : mFramebuffers)
			vkDestroyFramebuffer(VkContext::Get().LogicalDevice(), framebuffer, nullptr);

		vkDestroyRenderPass(VkContext::Get().LogicalDevice(), mRenderPass, nullptr);
	}

	/**
	 * \brief Sets the current target for all the future draw calls
	 * \param frame Frame index to record commands to
	 */
	void VkRenderPass::Begin(uint32_t frame)
	{
		if (mSize.first != mTarget->Width() || mSize.second != mTarget->Height())
			Resize();

		const VkClearValue color{ VkClearColorValue{{ 0.f,0.f,0.f,1.f } } };

		VkRenderPassBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		beginInfo.renderPass = mRenderPass;
		beginInfo.framebuffer = mFramebuffers[frame];
		beginInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		beginInfo.renderArea.extent = VkExtent2D{ mTarget->Width(), mTarget->Height() };
		beginInfo.clearValueCount = 1;
		beginInfo.pClearValues = &color;

		vkCmdBeginRenderPass(mTarget->CommandBuffer(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	/**
	 * \brief Unbinds the current target from future draw calls
	 */
	void VkRenderPass::End()
	{
		vkCmdEndRenderPass(mTarget->CommandBuffer());
	}

	void VkRenderPass::CreateFramebuffers()
	{
		mFramebuffers.resize(VkSurface::BufferCount);
		for (uint32_t i{ 0 }; i < VkSurface::BufferCount; i++)
		{
			const VkImageView views[]{ mTarget->ImageView(i) };

			VkFramebufferCreateInfo createInfo{};
			createInfo.renderPass = mRenderPass;
			createInfo.attachmentCount = 1;
			createInfo.pAttachments = views;
			createInfo.width = mTarget->Width();
			createInfo.height = mTarget->Height();
			createInfo.layers = 1;

			VK_ASSERT(vkCreateFramebuffer(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mFramebuffers[i]), "Failed to create framebuffer");
		}

		mSize.first = mTarget->Width();
		mSize.second = mTarget->Height();
	}

	void VkRenderPass::Resize()
	{
		for (uint32_t i{ 0 }; i < mFramebuffers.size(); i++)
		{
			vkDestroyFramebuffer(VkContext::Get().LogicalDevice(), mFramebuffers[i], nullptr);
		}

		CreateFramebuffers();
	}
}
