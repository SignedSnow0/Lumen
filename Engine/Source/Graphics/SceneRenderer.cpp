#include "SceneRenderer.h"

#include "Core/Application.h"
#include "Core/Components.h"
#include "Core/Entity.h"

namespace Lumen::Graphics
{
    SceneRenderer::SceneRenderer(Scene* scene)
        : mScene{ scene }
    {
        mRenderer = new Graphics::DefaultRenderer{};
    }

    SceneRenderer::~SceneRenderer()
    {
        delete mRenderer;
    }

    void SceneRenderer::Render(const RenderTarget& target)
    {
        mRenderer->Begin(target.Surface);

        for (const auto entity : mScene->mRegistry.view<Components::Transform>())
        {
            Entity e{ entity, mScene };
            const auto transform{ e.GetComponents<Components::Transform>() };
            mRenderer->Render(transform->GetTransform());
        }

        mRenderer->End();
    }
}
