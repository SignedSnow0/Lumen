#pragma once
#include <filesystem>

#include "Core/Application.h"

namespace Lumen::Graphics
{
	class Texture
	{
	public:
		static Texture* Create(std::filesystem::path source);
		virtual ~Texture() = default;

		[[nodiscard]] static std::filesystem::path TexturesPath() { return Application::Get()->AssetsPath() / "Textures"; }

		virtual void Release() = 0;

	protected:
		static Texture* (*sCreateFunc)(std::filesystem::path);
	};
}
