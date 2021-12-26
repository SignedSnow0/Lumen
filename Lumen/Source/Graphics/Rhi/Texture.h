#pragma once
#include <filesystem>

namespace Lumen::Graphics
{
	class Texture
	{
	public:
		static Texture* Create(std::filesystem::path source);
		virtual ~Texture() = default;

		virtual void Release() = 0;

	protected:
		static Texture* (*sCreateFunc)(std::filesystem::path);
	};
}
