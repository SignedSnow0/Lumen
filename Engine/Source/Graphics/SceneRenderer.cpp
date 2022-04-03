#include "SceneRenderer.h"

#include "Core/Application.h"
#include "Core/Components.h"
#include "Core/Entity.h"
#include "Utils/Logger.h"

namespace Lumen::Graphics
{
	SceneRenderer::SceneRenderer(Scene* scene)
		: mScene{ scene }
	{
		mRenderer = new Graphics::DefaultRenderer{};

		LOG_TRACE("SceneRenderer created for scene {}", static_cast<void*>(scene));
	}

	SceneRenderer::~SceneRenderer()
	{
		delete mRenderer;

		LOG_TRACE("SceneRenderer destroyed for scene {}", static_cast<void*>(mScene));
	}

	void SceneRenderer::Render(const RenderTarget& target)
	{
		mRenderer->Begin(target.Surface);

		//renders all the entities with a transform component
		for (const auto entity : mScene->mRegistry.view<Components::Transform>())
		{
			Entity e{ entity, mScene };
			const auto transform{ e.GetComponents<Components::Transform>() };
			mRenderer->Render(transform->GetTransform());
		}

		mRenderer->End();
	}
}
