#pragma once
#include <glm/glm.hpp>

#include "Core/Types.h"

namespace Lumen::Graphics
{
	class RenderPass;
	class Shader;
	
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Color;
		glm::vec2 TextureCoordinates;
	};

	enum class RendererApi : u32
	{
		Opengl = 0,
		Vulkan,
		Direct3D,
		Metal
	};

	enum class CullMode : u32
	{
		Front = 0,
		Back,
		FrontAndBack,
		None
	};

	enum class PolygonMode : u32
	{
		Fill = 0,
		Line,
		Point
	};

	enum class DrawType : u32
	{
		Point = 0,
		Triangle,
		Line
	};

	enum class BlendMode : u32
	{
		None = 0,
		OneZero,
		ZeroSrcColor,
		SrcAlphaOneMinusSrcAlpha
	};

	struct PipelineInitInfo 
	{
		CullMode CullMode{ CullMode::Back };
		PolygonMode PolygonMode{ PolygonMode::Fill };
		DrawType DrawType{ DrawType::Triangle };
		BlendMode BlendMode{ BlendMode::None };
		u32 InitialWidth{ 1920 };
		u32 InitialHeight{ 1080 };

		Shader* Shader{ nullptr };
		RenderPass* RenderPass{ nullptr };
	};

	enum class AttachmentUsage : u32
	{
		Color = 0,
		Depth
	};

	enum class AttachmentFormat : u32
	{
		B8G8R8A8Srgb = 0,
		R8G8B8A8Srgb,

		D32SFloat,
		D24UNormS8Uint
	};
	
	struct Attachment
	{
		AttachmentUsage Usage{};
		AttachmentFormat Format{};
	};

	enum class ShaderStage : u32
	{
		Vertex = 0,
		Fragment,
		Compute,

		Count
	};
}
