#pragma once
#include "Core/Types.h"

namespace Lumen::Graphics
{
    enum class RendererAPI : u32
    {
        Opengl = 0, //Unsupported
        Vulkan,
        Direct3D, //Unsupported
    	Metal, //Unsupported

    	Count
    };

	class GraphicsContext
	{
    public:
        virtual ~GraphicsContext();

        static RendererAPI GetRenderAPI() { return sRenderAPI; }
        static void SetRenderAPI(RendererAPI api);

        virtual void Init() = 0;
       
        virtual void WaitIdle() const = 0;

        static GraphicsContext* Create();

    protected:
        static GraphicsContext* (*sCreateFunc)();
        static RendererAPI sRenderAPI;
    };
}
