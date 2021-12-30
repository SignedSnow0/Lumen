#pragma once
#include <filesystem>
#include <unordered_map>

#include "Core/Application.h"
#include "Core/Types.h"

namespace Lumen::Graphics
{
	enum class ShaderStage : u32
	{
		Vertex = 0,
		Fragment,
		Compute,

		Count
	};

	class Shader
	{
	public:
		static Shader* Create(const std::unordered_map<std::string, ShaderStage>& sources);
		virtual ~Shader() = default;

		[[nodiscard]] static std::filesystem::path ShadersPath() { return Application::Get()->AssetsPath() / "Shaders"; }

		virtual void Init() = 0;
		virtual void Release() = 0;

	protected:
		static Shader* (*sCreateFunc)(const std::unordered_map<std::string, ShaderStage>&);
	};
}
