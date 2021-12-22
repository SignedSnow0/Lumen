#pragma once
#include <vulkan/vulkan.h>

#include "VkSurface.h"

namespace Lumen::Graphics::Vulkan
{
	struct Attachment
	{
		VkAttachmentDescription	Description{};
		VkAttachmentReference	Reference{};
		VkSubpassDescription	Subpass{};
		VkSubpassDependency		Dependency{};

		void AsColor(VkFormat format);
	};

	class VkRenderPass
	{
	public:
		VkRenderPass() = default;
		explicit VkRenderPass(std::vector<Attachment> attachments, VkSurface& target);
		~VkRenderPass() { Release(); }

		[[nodiscard]] constexpr uint32_t Width() const { return mTarget->Width(); }
		[[nodiscard]] constexpr uint32_t Height() const { return mTarget->Height(); }
		[[nodiscard]] constexpr ::VkRenderPass Get() const { return mRenderPass; }

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
		std::vector<VkFramebuffer>		mFramebuffers{};
		::VkRenderPass					mRenderPass{};
	};
}
