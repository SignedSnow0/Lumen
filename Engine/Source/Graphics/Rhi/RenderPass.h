#pragma once
#include <vector>

#include "Core/Types.h"

namespace Lumen::Graphics
{
	class Surface;

	enum class AttachmentUsage : u32
	{
		Color = 0,
		Depth,

		Count
	};

	enum class AttachmentFormat : u32
	{
		B8G8R8A8Srgb = 0,
		R8G8B8A8Srgb,

		D32SFloat,
		D24UNormS8Uint,

		Count
	};
	
	struct Attachment
	{
		AttachmentUsage Usage{};
		AttachmentFormat Format{};
	};
	
	class RenderPass
	{
	public:
		static RenderPass* Create(const std::vector<Attachment>& attachments, Surface* target = nullptr);
		virtual ~RenderPass() = default;

		virtual void Init() = 0;
		virtual void Release() = 0;

		virtual void Begin(u32 frame, Surface* target = nullptr) = 0;
		virtual void End(Surface* target = nullptr) = 0;
		virtual void Resize(u32 width, u32 height) = 0;

	protected:
		static RenderPass* (*sCreateFunc)(const std::vector<Attachment>&, Surface*);
	};
}
