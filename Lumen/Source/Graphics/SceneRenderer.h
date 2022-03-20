#pragma once
#include "DefaultRenderer.h"
#include "Core/Scene.h"

namespace Lumen::Graphics
{
    class SceneRenderer
    {
    public:
        explicit SceneRenderer(Scene* scene);
        ~SceneRenderer();
        
        void Render(const RenderTarget& target);

        [[nodiscard]] DefaultRenderer* GetDefaultRenderer() { return mRenderer; }
        
    private:
        DefaultRenderer* mRenderer{ nullptr };
        Scene* mScene{ nullptr };
    };
}
