#pragma once
#include "Core/Application.h"

namespace Lumen::Graphics
{
	class Shader
	{
	public:
		Shader() = delete;
		Shader(const Shader&) = delete;
		Shader(Shader&&) = delete;
		Shader& operator=(const Shader&) = delete;
		Shader& operator=(Shader&&) = delete;

		/**
		 * @brief Creates a new Shader instance
		 * @param sources Code sources to compile the shader with (path, stage)
		 * @return The new Shader instance
		 */
		static Shader* Create(const std::unordered_map<std::string, ShaderStage>& sources);
		virtual ~Shader() = default;

		/**
		 * @brief Returns the default shaders folder
		 */
		static std::filesystem::path GetShadersPath() { return Application::Get()->GetAssetsPath() / "Shaders"; }

		/**
		 * @brief Initializes the shader resources
		 */
		virtual void Init() = 0;
		/**
		 * @brief Releases the shader resources
		 */
		virtual void Release() = 0;

	protected:
		static Shader* (*sCreateFunc)(const std::unordered_map<std::string, ShaderStage>&);
	};
}
