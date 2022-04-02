#include "DescriptorSet.h"

namespace Lumen::Graphics
{
	DescriptorSet* (*DescriptorSet::sCreateFunc)(Shader*, u32);

	DescriptorSet* DescriptorSet::Create(Shader* shader, const u32 setIndex)
	{
		assert(sCreateFunc && "No graphics api set!");
		return sCreateFunc(shader, setIndex);
	}
}
