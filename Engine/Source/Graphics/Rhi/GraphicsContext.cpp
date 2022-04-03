#include "GraphicsContext.h"

#include <cassert>

#include "Platform/Vulkan/VkInterface.h"

namespace Lumen::Graphics
{
	GraphicsContext* (*GraphicsContext::sCreateFunc)() = nullptr;
	RendererApi GraphicsContext::sRenderAPI;
	GraphicsContext* GraphicsContext::sInstance = nullptr;

	/**
	 * @brief Used to create an instance of graphics context with the appropriate renderer api.\n
	 * Set a render api using SetRenderApi before calling this
	 * @return A pointer to the created class
	 */
	GraphicsContext* GraphicsContext::Create()
	{
		assert(sCreateFunc && "No graphics api set!");
		sInstance = sCreateFunc();
		return sInstance;
	}

	RendererApi GraphicsContext::GetRenderApi() { return sRenderAPI; }

	/**
	 * @brief Sets the render api to the one passed as argument
	 * @param api The graphics api to set
	 */
	void GraphicsContext::SetRenderApi(RendererApi api)
	{
		sRenderAPI = api;

		switch (sRenderAPI)
		{
		case RendererApi::Vulkan:
			Vulkan::SetApiInterface();
			break;
		case RendererApi::Opengl:
		case RendererApi::Direct3D:
		case RendererApi::Metal:
			break;
		}
	}

	GraphicsContext& GraphicsContext::Get() { return *sInstance; }
}
