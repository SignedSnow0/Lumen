﻿#pragma once
#include "Rhi/Gui.h"
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

		void Render();

	private:
		Surface* mSurface{ nullptr };
		RenderPass* mRenderPass{ nullptr };
		Shader* mShader{ nullptr };
		Pipeline* mPipeline{ nullptr };
		DescriptorSet* mDescriptorSet{ nullptr };
		Gui* mGui{ nullptr };
	};
}
