#pragma once
#include "Rhi/Gui.h"
#include "Rhi/Pipeline.h"
#include "Rhi/RenderPass.h"
#include "Rhi/Shader.h"
#include "Rhi/Surface.h"

namespace Lumen::Graphics
{
	class DefaultRenderer
	{
	public:
		explicit DefaultRenderer();
		~DefaultRenderer();

		[[nodiscard]] constexpr RenderPass* GetRenderPass() const { return mRenderPass; }

		void Render(Surface* surface);

	private:
		RenderPass* mRenderPass{ nullptr };
		Shader* mShader{ nullptr };
		Pipeline* mPipeline{ nullptr };
		DescriptorSet* mDescriptorSet{ nullptr };
	};
}
