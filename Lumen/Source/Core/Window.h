#pragma once

#include "Types.h"

struct GLFWwindow;

namespace Lumen
{
	struct WindowInitInfo
	{
		const char* Title{ nullptr };
		u32 Width{ 0 };
		u32 Height{ 0 };
	};

	class Window
	{
	public:
		explicit Window(const WindowInitInfo& initInfo, bool visible = true);
		~Window();

		[[nodiscard]] b8 ShouldClose() const;
		[[nodiscard]] decltype(auto) Native() const
		{
#ifdef _WIN64
			return static_cast<GLFWwindow*>(mNative);
#else
#error "Unsupported platform"
#endif
		}

		static void Update();

	private:
		void* mNative;
	};
}
