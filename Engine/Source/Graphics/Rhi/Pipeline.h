#pragma once
#include "Core/Types.h"

namespace Lumen::Graphics
{
	class DescriptorSet;
	class RenderPass;
	class Shader;

	enum class CullMode : u32
    {
        Front = 0,
        Back,
        FrontAndBack,
        None,

        Count
    };

    enum class PolygonMode : u32
    {
        Fill = 0,
        Line,
        Point,

        Count
    };

    enum class DrawType : u32
    {
        Point = 0,
        Triangle,
        Line,

        Count
    };

    enum class BlendMode : u32
    {
        None = 0,
        OneZero,
        ZeroSrcColor,
        SrcAlphaOneMinusSrcAlpha,

        Count
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

	class Pipeline
	{
	public:
        static Pipeline* Create(const PipelineInitInfo& initInfo);
        virtual ~Pipeline() = default;

        virtual void Init() = 0;
        virtual void Release() = 0;

        virtual void Bind() = 0;
        virtual void BindDescriptorSet(const DescriptorSet& set, u32 frame) = 0;
		virtual void SetPushConstant(u32 index, const void* data) = 0;
		
	protected:
        static Pipeline* (*sCreateFunc)(const PipelineInitInfo&);
	};
}
