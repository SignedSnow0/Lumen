#pragma once
#include "Core/Types.h"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

#pragma comment(lib, "mono-2.0-sgen.lib")

namespace Lumen
{
    class ScriptManager
    {
    public:
        b8 Init();
        void Shutdown();

    private:
        void BindCalls();

        MonoDomain*     mDomain{ nullptr };
        MonoAssembly*   mAssembly{ nullptr };
        MonoImage*      mImage{ nullptr };
    };
}
