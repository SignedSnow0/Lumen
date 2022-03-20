#pragma once
#include "Core/Types.h"
#include <mono/metadata/assembly.h>

#pragma comment(lib, "mono-2.0-sgen.lib")

namespace Lumen
{
    class ScriptManager
    {
    public:
        b8 Init();
        void Shutdown();

    private:
        MonoAssembly* mAssembly{ nullptr };
    };

    namespace Script
    { 
        void HelloBack();
    }
}
