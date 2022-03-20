#include "Surface.h"

#include <cassert>

namespace Lumen::Graphics
{
	Surface* (*Surface::sCreateFunc)(Window*) = nullptr;

	Surface* Surface::Create(Window* target)
	{
		assert(sCreateFunc && "No graphics context set");
		return sCreateFunc(target);
	}
}
