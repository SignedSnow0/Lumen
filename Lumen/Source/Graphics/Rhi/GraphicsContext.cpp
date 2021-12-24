#include "GraphicsContext.h"

#include <cassert>

#include "Platform/Vulkan/VkInterface.h"

namespace Lumen::Graphics
{
    GraphicsContext* (*GraphicsContext::sCreateFunc)() = nullptr;
    RendererAPI GraphicsContext::sRenderAPI;

    /**
     * @brief Used to create an instance of graphics context with the appropriate renderer api.\n
     * Set a render api using SetRenderApi before calling this
     * @return A pointer to the created class
     */
    GraphicsContext* GraphicsContext::Create()
    {
        assert(sCreateFunc && "No graphics api set!");
        return sCreateFunc();
    }

    /**
     * @brief Sets the render api to the one passed as argument
     * @param api The graphics api to set
     */
    void GraphicsContext::SetRenderAPI(RendererAPI api)
    {
        sRenderAPI = api;

        switch (sRenderAPI)
        {
        case RendererAPI::Vulkan:
            Vulkan::SetApiInterface();
            break;
        case RendererAPI::Opengl:
        case RendererAPI::Direct3D:
        case RendererAPI::Metal:
        case RendererAPI::Count:
            assert(false && "Unsupported graphics api set!");
            break;
        }
    }
}
