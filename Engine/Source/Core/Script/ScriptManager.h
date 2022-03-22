#pragma once
#include <unordered_map>

#include "Core/Types.h"
#include <mono/jit/jit.h>

namespace Lumen
{
    class ScriptManager
    {
    public:
        b8 Init();
        void Shutdown();

        [[nodiscard]] const std::unordered_map<std::string, MonoClass*>& Scripts() const { return mScripts; }
        
    private:
        void BindCalls();
        void LoadScripts();
        
        std::unordered_map<std::string, MonoClass*> mScripts{};
        MonoDomain*     mDomain{ nullptr };
        MonoAssembly*   mAssembly{ nullptr };
        MonoImage*      mImage{ nullptr };
    };
}
