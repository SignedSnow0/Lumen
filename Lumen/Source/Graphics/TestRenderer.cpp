#include "TestRenderer.h"

#include "Rhi/DescriptorSet.h"

namespace Lumen::Graphics
{
	TestRenderer::TestRenderer(Window* target)
	{
		mSurface = Surface::Create(target);
		mSurface->Init();
		mRenderPass = RenderPass::Create(mSurface);
		mShader = Shader::Create({ { R"(D:\Dev\Lumen\Lumen\Assets\Shaders\vertex.vert)", ShaderStage::Vertex }, { R"(D:\Dev\Lumen\Lumen\Assets\Shaders\fragment.frag)", ShaderStage::Fragment } });
		mPipeline = Pipeline::Create({ CullMode::None, PolygonMode::Fill, DrawType::Triangle, BlendMode::None, 1920, 1080, mShader, mRenderPass });
		mPipeline->Init();
		mDescriptorSet = DescriptorSet::Create(mShader, 0);
		mDescriptorSet->Init();
	}

	TestRenderer::~TestRenderer()
	{
		delete mDescriptorSet;
		delete mPipeline;
		delete mShader;
		delete mRenderPass;
		delete mSurface;
	}
}
