#pragma once
#include "Core/Types.h"

namespace Lumen
{
	class Window;

	namespace Graphics
	{
		class Surface
		{
		public:
			static Surface* Create(Window* target);
			virtual ~Surface() = default;

			virtual void Init() = 0;
			virtual void Release() = 0;

			[[nodiscard]] virtual u32 CurrentFrame() const = 0;

			virtual void Begin() = 0;
			virtual	void End() = 0;

		protected:
			static Surface* (*sCreateFunc)(Window*);
		};
	}
}
