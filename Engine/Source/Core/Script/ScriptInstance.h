#pragma once
#include <mono/metadata/assembly.h>
#include <mono/utils/mono-forward.h>

#include "Core/Entity.h"

namespace Lumen::Script
{
    class ScriptInstance
    {
    public:
        explicit ScriptInstance(MonoClass* monoClass, MonoDomain* monoDomain, Entity& owner);

        void Start();
        
    private:
        MonoClass* mClass{ nullptr };
        MonoObject* mInstance{ nullptr };
        MonoMethod* mStart{ nullptr };
        MonoMethod* mUpdate{ nullptr };
    };    
}
