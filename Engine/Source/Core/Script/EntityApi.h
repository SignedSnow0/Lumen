#pragma once
#include <string>
#include <glm/vec3.hpp>
#include <mono/metadata/object.h>

#include "Core/Types.h"

namespace Lumen::Script
{
    class EntityApi
    {
    public:
        static void BindCalls();

    private:
        static u32 Create();
        static b8 Exists(u32 id);
        static void AddComponent(u32 id, MonoString* compName);
    };
    
}
