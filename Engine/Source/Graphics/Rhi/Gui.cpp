#include "Gui.h"

#include <cassert>

namespace Lumen::Graphics
{
	Gui* (*Gui::sCreateFunc)() = nullptr;

	Gui* Gui::Create()
	{
		assert(sCreateFunc && "No graphics api set!");
		return sCreateFunc();
	}
}