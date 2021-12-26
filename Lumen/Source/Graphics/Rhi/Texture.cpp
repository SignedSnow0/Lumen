#include "Texture.h"

#include <cassert>
#include <utility>

namespace Lumen::Graphics
{
	Texture* (*Texture::sCreateFunc)(std::filesystem::path);

	Texture* Texture::Create(std::filesystem::path source)
	{
		assert(sCreateFunc && "No graphics api set!");
		return sCreateFunc(std::move(source));
	}
}
