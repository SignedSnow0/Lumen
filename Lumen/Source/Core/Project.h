#pragma once
#include <filesystem>

#include "Scene.h"
#include "Graphics/Rhi/GraphicsContext.h"

namespace Lumen
{
	class Project
	{
	public:
		std::string				Name{};
		std::filesystem::path	Path{};
		Graphics::RendererAPI	Api{ Graphics::RendererAPI::Count };
		Scene*					Scene{ nullptr };
	};
}
