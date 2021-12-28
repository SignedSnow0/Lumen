#pragma once
#include <vulkan/vulkan.h>

#include "VkSurface.h"
#include "Graphics/Rhi/RenderPass.h"

namespace Lumen::Graphics::Vulkan
{
	class VkTexture;

	struct Attachment
	{
		VkAttachmentDescription	Description{};
		VkAttachmentReference	Reference{};
		VkSubpassDescription	Subpass{};
		VkSubpassDependency		Dependency{};

		void AsColor(VkFormat format, VkImageLayout layout);
	};

	class VkRenderPass final : public RenderPass
	{
	public:
		explicit VkRenderPass(std::vector<Attachment> attachments, VkSurface* target = nullptr);
		~VkRenderPass() override { VkRenderPass::Release(); }

		[[nodiscard]] constexpr u32 Width() const			{ return mSize.first; }
		[[nodiscard]] constexpr u32 Height() const			{ return mSize.second; }
		[[nodiscard]] constexpr ::VkRenderPass Get() const	{ return mRenderPass; }
		[[nodiscard]] VkTexture* Texture(u32 frame) const { return mTarget ? nullptr : mTextures[frame]; }
		void Init() override;
		void Release() override;

		void Begin(u32 frame, Surface* target = nullptr) override;
		void End(Surface* target = nullptr) override;
		void Resize(u32 width, u32 height) override;

		static void SetInterface();

	private:
		void CreateFramebuffers(u32 width, u32 height);

		std::pair<u32, u32>				mSize{};
		const std::vector<Attachment>	mAttachments{};

		std::vector<VkTexture*>			mTextures{};
		VkSurface*						mTarget{ nullptr };
		std::vector<VkFramebuffer>		mFramebuffers{};
		::VkRenderPass					mRenderPass{};
	};
}
