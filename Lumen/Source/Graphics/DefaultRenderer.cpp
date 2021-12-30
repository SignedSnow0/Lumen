#include "DefaultRenderer.h"

#include "Rhi/Buffers.h"
#include "Rhi/DescriptorSet.h"
#include "Rhi/Texture.h"

namespace Lumen::Graphics
{
	namespace 
	{
		const std::vector<Vertex> vertices =
		{
			{ { -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
			{ {  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
			{ {  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
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

	DefaultRenderer::DefaultRenderer()
		: mRenderPass(RenderPass::Create())
	{
		mShader = Shader::Create({ { (Shader::ShadersPath() / "vertex.vert").string(), ShaderStage::Vertex }, { (Shader::ShadersPath() / "fragment.frag").string(), ShaderStage::Fragment } });
		mPipeline = Pipeline::Create({ CullMode::None, PolygonMode::Fill, DrawType::Triangle, BlendMode::None, 1920, 1080, mShader, mRenderPass });
		mPipeline->Init();
		mDescriptorSet = DescriptorSet::Create(mShader, 0);
		mDescriptorSet->Init();

		vBuffer = VertexBuffer::Create(vertices.data(), static_cast<u32>(vertices.size()));
		iBuffer = IndexBuffer::Create(indices.data(), static_cast<u32>(indices.size()));

		texture = Texture::Create(Texture::TexturesPath() / "texture.jpg");
		mDescriptorSet->SetTexture(1, texture);
	}

	DefaultRenderer::~DefaultRenderer()
	{
		delete texture;

		delete vBuffer;
		delete iBuffer;

		delete mDescriptorSet;
		delete mPipeline;
		delete mShader;
		delete mRenderPass;
	}

	void DefaultRenderer::Render(Surface* surface)
	{
		const u32 currentFrame = surface->CurrentFrame();

		mDescriptorSet->Update(currentFrame);

		mRenderPass->Begin(currentFrame, surface);
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

		vBuffer->Bind(surface);
		iBuffer->Bind(surface);

		iBuffer->Draw(surface);

		mRenderPass->End(surface);
	}
}
