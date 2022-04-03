#include "Window.h"

#ifdef _WIN64
#include <GLFW/glfw3.h>

namespace Lumen
{
	namespace
	{
		bool glfwInitialized{ false };
		uint32_t windowCount{ 0 };
	}

	/**
	 * @brief Creates a new window using the windows api
	 * @param initInfo Initial settings used to create the window
	 * @param visible If the window will be visible upon creation
	 */
	Window::Window(const WindowInitInfo& initInfo, bool visible)
	{
		//inits glfw if it hasn't been already
		if (!glfwInitialized)
		{
			glfwInit();
			glfwInitialized = true;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		if (!visible)
			glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		mNative = glfwCreateWindow(static_cast<s32>(initInfo.Width), static_cast<s32>(initInfo.Height), initInfo.Title, nullptr, nullptr);

		windowCount++;
	}

	Window::~Window()
	{
		glfwDestroyWindow(mNative);
		windowCount--;
		//terminates glfw if there are no windows
		if (!windowCount)
		{
			glfwTerminate();
			glfwInitialized = false;
		}
	}

	/**
	 * @brief Asks the window if the close button has been pressed.
	 * @return True if the window is closing.
	 */
	b8 Window::ShouldClose() const { return glfwWindowShouldClose(mNative); }

	WindowHandle Window::Native() const { return mNative; }
	
	/**
	 * @brief Updates all the windows by polling native events and messages.
	 */
	void Window::Update() { glfwPollEvents(); }
}
#else
#error "Unsupported platform"
#endif	