#include "DescriptorSet.h"

#include <cassert>

namespace Lumen::Graphics
{
	DescriptorSet* (*DescriptorSet::sCreateFunc)(Shader*, u32);

	DescriptorSet* DescriptorSet::Create(Shader* shader, u32 setIndex)
	{
		assert(sCreateFunc && "No graphics api set!");
		return sCreateFunc(shader, setIndex);
	}
}
