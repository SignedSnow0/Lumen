#pragma once
#include <filesystem>

#include "Core/Application.h"

namespace Lumen::Graphics
{
	class Texture
	{
	public:
		Texture() = default;
		Texture(const Texture&) = delete;
		Texture(Texture&&) = delete;
		Texture& operator=(const Texture&) = delete;
		Texture& operator=(Texture&&) = delete;

		/**
		 * @brief Creates a new Texture instance
		 * @param source Path to the texture file
		 * @return The new Texture instance
		 */
		static Texture* Create(std::filesystem::path source);
		virtual ~Texture() = default;

		/**
		 * @brief Returns the default textures folder
		 */
		static std::filesystem::path GetTexturesPath() { return Application::Get()->GetAssetsPath() / "Textures"; }

		/**
		 * @brief Releases the texture resources
		 */
		virtual void Release() = 0;

	protected:
		static Texture* (*sCreateFunc)(std::filesystem::path);
	};
}
