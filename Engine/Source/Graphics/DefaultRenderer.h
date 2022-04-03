#pragma once
#include <glm/glm.hpp>

#include "Core/Types.h"

namespace Lumen::Graphics
{
	class Surface;
	class RenderPass;
	class Shader;
	class Pipeline;
	class DescriptorSet;
	
	class DefaultRenderer
	{
	public:
		explicit DefaultRenderer();
		~DefaultRenderer();

		[[nodiscard]] constexpr RenderPass* GetRenderPass() const { return mRenderPass; }

		void Begin(Surface* surface);
		void Render(const glm::mat4& model);
		void End();
		
	private:
		Surface* mBoundSurface{ nullptr };
		u32 mCurrentFrame{};
		RenderPass* mRenderPass{ nullptr };
		Shader* mShader{ nullptr };
		Pipeline* mPipeline{ nullptr };
		DescriptorSet* mDescriptorSet{ nullptr };
	};
}
