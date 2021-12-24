#include "Pipeline.h"

#include <cassert>

namespace Lumen::Graphics
{
	Pipeline* (*Pipeline::sCreateFunc)(const PipelineInitInfo&) = nullptr;

	Pipeline* Pipeline::Create(const PipelineInitInfo& initInfo)
	{
		assert(sCreateFunc && "No graphics api set!");
		return sCreateFunc(initInfo);
	}
}
