#pragma once
#include <vector>

#include <vulkan/vulkan.h>
#include <imgui/imgui.h>

#include "Graphics/Rhi/Gui.h"

namespace Lumen
{
	namespace Graphics
	{
		class Surface;
	}

	class Window;
}

namespace Lumen::Graphics::Vulkan
{
	class VkSurface;
	class VkRenderPass;

	class VkGui : public Gui
	{
	public:
		VkGui() = default;
		~VkGui() override { VkGui::Release(); }

		ImFont* BoldFont() override { return mBoldFont; }
		ImFont* ItalicFont() override { return mItalicFont; }

		void Init(const Window* target, Surface* surface) override;
		void Release() override;

		void Begin() override;
		void End() override;
		void SetViewportImages(RenderPass* scene) override;

		static void SetInterface();

	private:
		void CreatePool();
		void CreateRenderPass();
		void InitImgui(const Window* target);

		ImFont* mBoldFont{ nullptr };
		ImFont* mItalicFont{ nullptr };
		std::vector<ImTextureID> mSceneTextures{};
		VkSurface* mTarget{ nullptr };
		VkDescriptorPool mPool{};
		VkRenderPass* mRenderPass{ nullptr };
	};
}
