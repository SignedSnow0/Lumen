#pragma once
#include <filesystem>
#include <string>

#include "Scene.h"
#include "Graphics/Rhi/GraphicsContext.h"

namespace Lumen
{
	class Project
	{
	public:
		std::string				Name{};
		std::filesystem::path	Path{};
		Graphics::RendererApi	Api{ Graphics::RendererApi::Count };
		Scene*					Scene{ nullptr };
	};
}
