#pragma once
#include <glm/vec3.hpp>

#include "Core/Types.h"

namespace Lumen::Script
{
    class ComponentsApi
    {
    public:
        static void BindCalls();

    private:
        static glm::vec3 GetTranslation(u32 id);
        static void SetTranslation(u32 id, float x, float y, float z);
        
        static glm::vec3 GetRotation(u32 id);
        static void SetRotation(u32 id, float x, float y, float z);

        static glm::vec3 GetScale(u32 id);
        static void SetScale(u32 id, float x, float y, float z);
    };
}

