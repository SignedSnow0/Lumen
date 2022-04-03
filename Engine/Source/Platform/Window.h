#pragma once

#include <GLFW/glfw3.h>

#include "Core/Types.h"

#ifdef _WIN64
struct GLFWwindow;
using WindowHandle = GLFWwindow*;
#endif

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

		b8 ShouldClose() const;
		WindowHandle Native() const;

		static void Update();

	private:
		WindowHandle mNative{ nullptr };
	};
}
