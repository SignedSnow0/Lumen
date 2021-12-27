#pragma once
#include <vulkan/vulkan.h>

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

		void Init(const Window* target, Surface* surface) override;
		void Release() override;

		void Begin() override;
		void End() override;

		static void SetInterface();

	private:
		void CreatePool();
		void CreateRenderPass();
		void InitImgui(const Window* target);

		VkSurface* mTarget{ nullptr };
		VkDescriptorPool mPool{};
		VkRenderPass* mRenderPass{ nullptr };
	};
}
