#include "DefaultRenderer.h"

#include <glm/gtc/type_ptr.hpp>

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

	void DefaultRenderer::Begin(Surface* surface)
	{
		mBoundSurface = surface;
		mCurrentFrame = mBoundSurface->CurrentFrame();

		mDescriptorSet->Update(mCurrentFrame);

		mRenderPass->Begin(mCurrentFrame, mBoundSurface);
		mPipeline->Bind();
	}

	void DefaultRenderer::Render(const glm::mat4& model)
	{
		static constexpr glm::mat4 mat{ 1.0 };
		static struct UBO
		{
			glm::mat4 View;
			glm::mat4 Proj;
		} ubo;
		ubo.View = mat;
		ubo.Proj = mat;

		mDescriptorSet->UpdateUniform(0, &ubo, mCurrentFrame);
		
		mPipeline->SetPushConstant(0, glm::value_ptr(model));
		mPipeline->BindDescriptorSet(*mDescriptorSet, mCurrentFrame);

		vBuffer->Bind(mBoundSurface);
		iBuffer->Bind(mBoundSurface);

		iBuffer->Draw(mBoundSurface);
	}

	void DefaultRenderer::End()
	{
		mRenderPass->End(mBoundSurface);
	}
}
