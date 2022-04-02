#pragma once

#include "Core/Types.h"

#ifdef _WIN64
struct GLFWwindow;
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
		decltype(auto) Native() const;

		static void Update();

	private:
		void* mNative{ nullptr };
	};
}
