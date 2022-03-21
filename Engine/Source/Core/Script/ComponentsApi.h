#pragma once
#include "Core/Types.h"

namespace Lumen::Script
{
    class ComponentsApi
    {
    public:
        static void BindCalls();

    private:
        static void* GetTranslation(u32 id);
        static void SetTranslation(u32 id, float x, float y, float z);
        
        static void* GetRotation(u32 id);
        static void SetRotation(u32 id, float x, float y, float z);

        static void* GetScale(u32 id);
        static void SetScale(u32 id, float x, float y, float z);
    };
}

