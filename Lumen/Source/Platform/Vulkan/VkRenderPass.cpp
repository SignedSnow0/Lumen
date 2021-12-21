#include "VkRenderPass.h"

#include "VkGraphics.h"
#include "VkSurface.h"

namespace FrostEngine::Vulkan
{
	/**
	 * \brief Fills the structure with default settings for a color attachment
	 * \param format The attachment`s format used
	 */
	void Attachment::AsColor(vk::Format format)
	{
		Description.format = format;
		Description.samples = vk::SampleCountFlagBits::e1;
		Description.loadOp = vk::AttachmentLoadOp::eClear;
		Description.storeOp = vk::AttachmentStoreOp::eStore;
		Description.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		Description.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		Description.initialLayout = vk::ImageLayout::eUndefined;
		Description.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		Reference.attachment = 0;
		Reference.layout = vk::ImageLayout::eColorAttachmentOptimal;

		Subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		Subpass.colorAttachmentCount = 1;
		Subpass.pColorAttachments = &Reference;

		Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		Dependency.dstSubpass = 0;
		Dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		Dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		Dependency.srcAccessMask = vk::AccessFlagBits{ 0 };
		Dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
	}

	VkRenderPass::VkRenderPass(std::vector<Attachment> attachments, VkSurface& target)
		: mAttachments{ std::move(attachments) }, mTarget{ &target }
	{
		Initialize();
	}

	void VkRenderPass::Initialize()
	{
		{
			std::vector<vk::AttachmentDescription> descriptions{ mAttachments.size() };
			std::vector<vk::SubpassDescription> subpasses{ mAttachments.size() };
			std::vector<vk::SubpassDependency> dependencies{ mAttachments.size() };
			uint32_t i{ 0 };
			for (const auto description : mAttachments)
			{
				descriptions[i] = description.Description;
				subpasses[i] = description.Subpass;
				dependencies[i] = description.Dependency;
				i++;
			}
			vk::RenderPassCreateInfo createInfo{};
			createInfo.attachmentCount = static_cast<uint32_t>(mAttachments.size());
			createInfo.pAttachments = descriptions.data();
			createInfo.subpassCount = static_cast<uint32_t>(mAttachments.size());
			createInfo.pSubpasses = subpasses.data();
			createInfo.dependencyCount = static_cast<uint32_t>(mAttachments.size());
			createInfo.pDependencies = dependencies.data();

			VK_ASSERT(VkGraphics::Device().Device().createRenderPass(&createInfo, nullptr, &mRenderPass), "Failed to create render pass");
		}
		CreateFramebuffers();
	}

	/**
	 * \brief Destroys all the class` resources
	 */
	void VkRenderPass::Release()
	{
		for (const auto& framebuffer : mFramebuffers)
			VkGraphics::Device().Device().destroyFramebuffer(framebuffer);

		VkGraphics::Device().Device().destroyRenderPass(mRenderPass);
	}

	/**
	 * \brief Sets the current target for all the future draw calls
	 * \param frame Frame index to record commands to
	 */
	void VkRenderPass::Begin(uint32_t frame)
	{
		if (mSize.first != mTarget->Width() || mSize.second != mTarget->Height())
			Resize();

		const vk::ClearValue color{ vk::ClearColorValue{ std::array{ 0.f,0.f,0.f,1.f } } };

		vk::RenderPassBeginInfo beginInfo{};
		beginInfo.renderPass = mRenderPass;
		beginInfo.framebuffer = mFramebuffers[frame];
		beginInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
		beginInfo.renderArea.extent = vk::Extent2D{ mTarget->Width(), mTarget->Height() };
		beginInfo.clearValueCount = 1;
		beginInfo.pClearValues = &color;

		mTarget->CommandBuffer().beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
	}

	/**
	 * \brief Unbinds the current target from future draw calls
	 */
	void VkRenderPass::End()
	{
		mTarget->CommandBuffer().endRenderPass();
	}

	void VkRenderPass::CreateFramebuffers()
	{
		mFramebuffers.resize(VkSurface::BufferCount);
		for (uint32_t i{ 0 }; i < VkSurface::BufferCount; i++)
		{
			const vk::ImageView views[]{ mTarget->ImageView(i) };

			vk::FramebufferCreateInfo createInfo{};
			createInfo.renderPass = mRenderPass;
			createInfo.attachmentCount = 1;
			createInfo.pAttachments = views;
			createInfo.width = mTarget->Width();
			createInfo.height = mTarget->Height();
			createInfo.layers = 1;

			VK_ASSERT(VkGraphics::Device().Device().createFramebuffer(&createInfo, nullptr, &mFramebuffers[i]), "Failed to create framebuffer");
		}

		mSize.first = mTarget->Width();
		mSize.second = mTarget->Height();
	}

	void VkRenderPass::Resize()
	{
		for (uint32_t i{ 0 }; i < mFramebuffers.size(); i++)
		{
			VkGraphics::Device().Device().destroyFramebuffer(mFramebuffers[i]);
		}

		CreateFramebuffers();
	}
}
