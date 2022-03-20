#include "Shader.h"

#include <cassert>

namespace Lumen::Graphics
{
	Shader* (*Shader::sCreateFunc)(const std::unordered_map<std::string, ShaderStage>&) = nullptr;

	Shader* Shader::Create(const std::unordered_map<std::string, ShaderStage>& sources)
	{
		assert(sCreateFunc && "No graphics api set!");
		return sCreateFunc(sources);
	}
}
