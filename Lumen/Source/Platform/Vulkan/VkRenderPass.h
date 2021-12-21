#pragma once
#include <vulkan/vulkan.hpp>

#include "VkSurface.h"

namespace FrostEngine::Vulkan
{
	class VkSurface;

	struct Attachment
	{
		vk::AttachmentDescription	Description{};
		vk::AttachmentReference		Reference{};
		vk::SubpassDescription		Subpass{};
		vk::SubpassDependency		Dependency{};

		void AsColor(vk::Format format);
	};

	class VkRenderPass
	{
	public:
		VkRenderPass() = default;
		explicit VkRenderPass(std::vector<Attachment> attachments, VkSurface& target);
		~VkRenderPass() { Release(); }

		[[nodiscard]] constexpr uint32_t Width() const { return mTarget->Width(); }
		[[nodiscard]] constexpr uint32_t Height() const { return mTarget->Height(); }
		[[nodiscard]] constexpr vk::RenderPass Get() const { return mRenderPass; }

		void Initialize();
		void Release();

		void Begin(uint32_t frame);
		void End();

	private:
		void CreateFramebuffers();
		void Resize();

		std::pair<uint32_t, uint32_t>	mSize{};
		const std::vector<Attachment>	mAttachments{};
		VkSurface*						mTarget{ nullptr };
		std::vector<vk::Framebuffer>	mFramebuffers{};
		vk::RenderPass					mRenderPass{};
	};
}
