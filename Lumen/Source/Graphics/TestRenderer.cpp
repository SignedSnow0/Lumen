#include "TestRenderer.h"

#include "Rhi/Buffers.h"
#include "Rhi/DescriptorSet.h"
#include "Rhi/Texture.h"

namespace Lumen::Graphics
{
	namespace 
	{
		const std::vector<Vertex> vertices =
		{
			{ { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
			{ {  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
			{ {  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
			{ { -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } }
		};
		const std::vector<u32> indices =
		{
			0, 1, 2,
			2, 3, 0
		};
		VertexBuffer* vBuffer{ nullptr };
		IndexBuffer* iBuffer{ nullptr };
		Texture* texture{ nullptr };
	}

	TestRenderer::TestRenderer(Window* target)
	{
		mSurface = Surface::Create(target);
		mSurface->Init();
		mRenderPass = RenderPass::Create();
		mShader = Shader::Create({ { R"(D:\Dev\Lumen\Lumen\Assets\Shaders\vertex.vert)", ShaderStage::Vertex }, { R"(D:\Dev\Lumen\Lumen\Assets\Shaders\fragment.frag)", ShaderStage::Fragment } });
		mPipeline = Pipeline::Create({ CullMode::None, PolygonMode::Fill, DrawType::Triangle, BlendMode::None, 1920, 1080, mShader, mRenderPass });
		mPipeline->Init();
		mDescriptorSet = DescriptorSet::Create(mShader, 0);
		mDescriptorSet->Init();

		mGui = Gui::Create();
		mGui->Init(target, mSurface);

		vBuffer = VertexBuffer::Create(vertices.data(), static_cast<u32>(vertices.size()));
		iBuffer = IndexBuffer::Create(indices.data(), static_cast<u32>(indices.size()));

		texture = Texture::Create(R"(D:\Dev\Lumen\Lumen\Assets\Textures\texture.jpg)");
		mDescriptorSet->SetTexture(1, texture);
	}

	TestRenderer::~TestRenderer()
	{
		delete texture;

		delete vBuffer;
		delete iBuffer;

		delete mGui;
		delete mDescriptorSet;
		delete mPipeline;
		delete mShader;
		delete mRenderPass;
		delete mSurface;
	}

	void TestRenderer::Render()
	{
		mSurface->Begin();
		const u32 currentFrame = mSurface->CurrentFrame();

		mDescriptorSet->Update(currentFrame);

		mRenderPass->Begin(currentFrame, mSurface);
		mPipeline->Bind();

		constexpr glm::mat4 mat{ 1.0 };
		static struct UBO
		{
			glm::mat4 Model;
			glm::mat4 View;
			glm::mat4 Proj;
		} ubo;
		ubo.Model = mat;
		ubo.View = mat;
		ubo.Proj = mat;

		mDescriptorSet->UpdateUniform(0, &ubo, currentFrame);

		mPipeline->BindDescriptorSet(*mDescriptorSet, currentFrame);

		vBuffer->Bind(mSurface);
		iBuffer->Bind(mSurface);

		iBuffer->Draw(mSurface);

		mRenderPass->End(mSurface);

		mGui->Begin();

		//todo: guidraw

		mGui->End();

		mSurface->End();
	}
}
