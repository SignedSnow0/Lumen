#pragma once
#include "Rhi/Pipeline.h"
#include "Rhi/RenderPass.h"
#include "Rhi/Shader.h"
#include "Rhi/Surface.h"

namespace Lumen::Graphics
{
	class TestRenderer
	{
	public:
		explicit TestRenderer(Window* target);
		~TestRenderer();

	private:
		Surface* mSurface{ nullptr };
		RenderPass* mRenderPass{ nullptr };
		Shader* mShader{ nullptr };
		Pipeline* mPipeline{ nullptr };
		DescriptorSet* mDescriptorSet{ nullptr };
	};
}
