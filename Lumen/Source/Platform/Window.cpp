#include "Core/Window.h"

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
		if (!glfwInitialized)
			glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);//todo remove this
		if (!visible)
			glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		mNative = glfwCreateWindow(static_cast<s32>(initInfo.Width), static_cast<s32>(initInfo.Height), initInfo.Title, nullptr, nullptr);

		windowCount++;
	}

	Window::~Window()
	{
		glfwDestroyWindow(static_cast<GLFWwindow*>(mNative));
		windowCount--;
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
	b8 Window::ShouldClose() const { return glfwWindowShouldClose(static_cast<GLFWwindow*>(mNative)); }

	/**
	 * @brief Updates all the windows by polling native events and messages.
	 */
	void Window::Update() { glfwPollEvents(); }
}
#else
#error "Unsupported platform"
#endif	