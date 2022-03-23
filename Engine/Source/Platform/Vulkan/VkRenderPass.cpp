﻿#include "VkRenderPass.h"

#include "VkContext.h"
#include "VkSurface.h"
#include "VkTexture.h"

namespace Lumen::Graphics::Vulkan
{
	namespace 
	{
		RenderPass* CreateFuncVulkan(Surface* target)
		{
			Attachment color{};
			color.AsColor(target ? VK_FORMAT_B8G8R8A8_SRGB : VK_FORMAT_R8G8B8A8_SRGB, target ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			return new VkRenderPass{ { color }, dynamic_cast<VkSurface*>(target) };
		}
	}

	/**
	 * @brief Fills the structure with default settings for a color attachment
	 * @param format The attachment`s format used
	 */
	void Attachment::AsColor(VkFormat format, VkImageLayout layout)
	{
		Description.format			= format;
		Description.samples			= VK_SAMPLE_COUNT_1_BIT;
		Description.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
		Description.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
		Description.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		Description.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		Description.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		Description.finalLayout		= layout;

		Reference.attachment	= 0;
		Reference.layout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		Subpass.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;
		Subpass.colorAttachmentCount	= 1;
		Subpass.pColorAttachments		= &Reference;

		Dependency.srcSubpass		= VK_SUBPASS_EXTERNAL;
		Dependency.dstSubpass		= 0;
		Dependency.srcStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		Dependency.dstStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		Dependency.srcAccessMask	= 0;
		Dependency.dstAccessMask	= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	VkRenderPass::VkRenderPass(std::vector<Attachment> attachments, VkSurface* target)
		: mAttachments{ std::move(attachments) }, mTarget{ target }
	{
		Init();
	}

	void VkRenderPass::Init()
	{
		std::vector<VkAttachmentDescription> descriptions{ mAttachments.size() };
		std::vector<VkSubpassDescription> subpasses{ mAttachments.size() };
		std::vector<VkSubpassDependency> dependencies{ mAttachments.size() };
		u32 i{ 0 };
		for (const auto& description : mAttachments)
		{
			descriptions[i] = description.Description;
			subpasses[i]	= description.Subpass;
			dependencies[i] = description.Dependency;
			i++;
		}

		VkRenderPassCreateInfo createInfo{};
		createInfo.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.attachmentCount	= static_cast<u32>(mAttachments.size());
		createInfo.pAttachments		= descriptions.data();
		createInfo.subpassCount		= static_cast<u32>(mAttachments.size());
		createInfo.pSubpasses		= subpasses.data();
		createInfo.dependencyCount	= static_cast<u32>(mAttachments.size());
		createInfo.pDependencies	= dependencies.data();

		VK_ASSERT(vkCreateRenderPass(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mRenderPass), "Failed to create render pass");

		if (!mTarget)
		{
			mTextures.resize(VkSurface::BufferCount);

			for (auto& texture : mTextures)
			{
				texture = new VkTexture{ 1920, 1080 };
			}
		}
		mFramebuffers.resize(VkSurface::BufferCount);

		if(mTarget)
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

		if (!mTarget)
		{
			for (const auto* texture : mTextures)
				delete texture;
		}
	}

	/**
	 * @brief Sets the current target for all the future draw calls
	 * @param frame Frame index to record commands to
	 */
	void VkRenderPass::Begin(u32 frame, Surface* target)
	{
		if(mTarget)
		{
			if (mSize.first != mTarget->Width() || mSize.second != mTarget->Height())
				Resize(mTarget->Width(), mTarget->Height());
		}
		
		constexpr VkClearValue color{ {{ 0.f,0.f,0.f,1.f } } };

		VkRenderPassBeginInfo beginInfo{};
		beginInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		beginInfo.renderPass		= mRenderPass;
		beginInfo.framebuffer		= mFramebuffers[frame];
		beginInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		beginInfo.renderArea.extent = VkExtent2D{ mSize.first, mSize.second };
		beginInfo.clearValueCount	= 1;
		beginInfo.pClearValues		= &color;

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
		if (!mTarget)
		{
			for (const auto& texture : mTextures)
			{
				texture->Release();
				texture->Recreate(width, height);
			}
		}

		for (u32 i{ 0 }; i < VkSurface::BufferCount; i++)
		{
			const VkImageView views[]{ mTarget ? mTarget->ImageView(i) : mTextures[i]->View() };

			VkFramebufferCreateInfo createInfo{};
			createInfo.sType			= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			createInfo.renderPass		= mRenderPass;
			createInfo.attachmentCount	= 1;
			createInfo.pAttachments		= views;
			createInfo.width			= width;
			createInfo.height			= height;
			createInfo.layers			= 1;

			VK_ASSERT(vkCreateFramebuffer(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mFramebuffers[i]), "Failed to create framebuffer");
		}

		mSize.first = width;
		mSize.second = height;
	}
}