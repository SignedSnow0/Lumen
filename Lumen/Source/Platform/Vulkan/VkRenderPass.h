#pragma once
#include <vulkan/vulkan.h>

#include "VkSurface.h"
#include "Graphics/Rhi/RenderPass.h"

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

	class VkRenderPass final : public RenderPass
	{
	public:
		explicit VkRenderPass(std::vector<Attachment> attachments, VkSurface& target);
		~VkRenderPass() override { VkRenderPass::Release(); }

		[[nodiscard]] constexpr u32 Width() const			{ return mTarget->Width(); }
		[[nodiscard]] constexpr u32 Height() const			{ return mTarget->Height(); }
		[[nodiscard]] constexpr ::VkRenderPass Get() const	{ return mRenderPass; }

		void Init() override;
		void Release() override;

		void Begin(u32 frame) override;
		void End() override;

		static void SetInterface();

	private:
		void CreateFramebuffers();
		void Resize();

		std::pair<u32, u32>				mSize{};
		const std::vector<Attachment>	mAttachments{};
		VkSurface*						mTarget{ nullptr };
		std::vector<VkFramebuffer>		mFramebuffers{};
		::VkRenderPass					mRenderPass{};
	};
}
