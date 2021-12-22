#include "VkDevice.h"

#include <fstream>
#include <set>
#include <vector>
#include <glfw/glfw3.h>

#include "Core/Window.h"

#ifdef _DEBUG
#include <iostream>
#endif

namespace Lumen::Graphics::Vulkan
{
	namespace
	{
#ifdef _DEBUG
		constexpr bool enableValidationLayers = true;
#else
		constexpr bool enableValidationLayers = false;
#endif

		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		/**
		 * @brief Callback function used by the validation layers in debug build.\n
		 * If the program is in debug mode the function will print to the console,\n
		 * if the program is in release mode it will output to a file in ./.FrostEngine/ValidationLayer.txt relative to the cwd.
		 * @param messageSeverity Severity of the message
		 * @param messageType Source of the message
		 * @param pCallbackData Information about the source object
		 * @param pUserData Optional user data
		 * @return False if the application should keep running without trying to fix the error
		 */
		VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
		{
			static bool reset{ true };

			if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			{
#ifdef _DEBUG
				std::cerr << "[Validation layer]: " << pCallbackData->pMessage << std::endl;
				std::string out = "[Validation layer: ] ";
				out.append(pCallbackData->pMessage);
#endif
				if(std::ofstream log{ ".FrostEngine/ValidationLayer.txt", std::ios::out | (reset ? std::ios::trunc : std::ios::app) }; log.is_open())
				{
					log << out << "\n\n";
					log.close();
				}

				reset = false;
			}

			return VK_FALSE;
		}

		/**
		 * @brief Callbakc function to create debug messenger
		 * @param instance Vulkan instance
		 * @param pCreateInfo Creation info struct
		 * @param pAllocator Custom memory allocator
		 * @param pDebugMessenger Output debug messenger
		 * @return Result enum of the function
		 */
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
		{
			const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
			return func ? func(instance, pCreateInfo, pAllocator, pDebugMessenger) : VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		/**
		 * @brief Destroys the debug messenger
		 * @param instance Vulkan instance
		 * @param debugMessenger Messenger to destroy
		 * @param pAllocator Custom memory allocator
		 */
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
		{
			if (const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
				vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")))
				func(instance, debugMessenger, pAllocator);
		}

		/**
		 * @brief Creates a prefilled creation structure
		 * @return The preffilled creation structure
		 */
		VkDebugUtilsMessengerCreateInfoEXT MessengerCreateInfo()
		{
			VkDebugUtilsMessengerCreateInfoEXT createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
										 VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
										 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
										 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
									 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
									 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
			createInfo.pfnUserCallback = DebugCallback;
			createInfo.pUserData = nullptr;

			return createInfo;
		}

		/**
		 * @brief Checks if the validation layers are supported by the current machine
		 * @return true if validation is supported
		 */
		bool CheckValidationSupport()
		{
			//get all available instance layers
			uint32_t layerCount;
			VK_ASSERT(vkEnumerateInstanceLayerProperties(&layerCount, nullptr), "Failed to enumerate instance layers");
			std::vector<VkLayerProperties> availableLayers{ layerCount };
			VK_ASSERT(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()), "Failed to enumerate instance layers");

			//find all layers in validationLayers from enumerations
			for (const char* layerName : validationLayers)
			{
				bool layerFound = false;

				for (const auto& layerProperties : availableLayers) 
				{
					if (strcmp(layerName, layerProperties.layerName) == 0)
					{
						layerFound = true;
						break;
					}
				}

				if (!layerFound)
					return false;
			}

			return true;
		}

		/**
		 * @brief Gets a list of the required instance extension names
		 * @return A vector with all the necessary extensions
		 */
		std::vector<const char*> GetRequiredExtensions()
		{
			glfwInit();

			//get required glfw extensions
			uint32_t glfwExtensionCount{ 0 };
			const char** glfwExtensions{ nullptr };//todo only with glfw
			glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

			std::vector<const char*> extensions{ glfwExtensions, glfwExtensions + glfwExtensionCount };
			if (enableValidationLayers)//add validation extension
				extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

			return extensions;
		}

		/**
		 * @brief Checks if all the extensions in "deviceExtensions" are supported
		 * @param device Physical device to check the support from
		 * @return true if all the extenions are supported
		 */
		bool CheckExtensionSupport(VkPhysicalDevice device)
		{
			uint32_t extensionCount{ 0 };
			VK_ASSERT(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr), "Failed to query device extensions");
			std::vector<VkExtensionProperties> availableExtensions{ extensionCount };
			VK_ASSERT(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data()), "Failed to query device extensions");

			std::set<std::string> requiredExtensions{ deviceExtensions.begin(), deviceExtensions.end() };
			for (const auto& extension : availableExtensions)
				requiredExtensions.erase(extension.extensionName);

			return requiredExtensions.empty();
		}
	}

	/**
	 * @brief Creates all the necessary resources for the class
	 */
	void VkDevice::Init()
	{
		CreateInstance();
		CreateDebugMessenger();
		CreatePhysicalDevice();
		CreateLogicalDevice();
		CreateAllocator();

		std::cout << "Device init" << std::endl;
	}

	/**
	 * @brief Destroys all the class` resources
	 */
	void VkDevice::Release()
	{
		vkDestroyDevice(mDevice, nullptr);
		if (enableValidationLayers)
			DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
		vkDestroyInstance(mInstance, nullptr);
	}
	
	/**
	 * @brief Creates an instance and the application
	 */
	void VkDevice::CreateInstance()
	{
		//init app info
		VkApplicationInfo appInfo{};
		appInfo.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName	= "Frost engine Game";
		appInfo.applicationVersion	= VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName			= "Frost engine";
		appInfo.engineVersion		= VK_MAKE_VERSION(0, 1, 0);
		appInfo.apiVersion			= VK_API_VERSION_1_2;

		//get instance creation info
		std::vector<const char*> extensions = GetRequiredExtensions();
		const bool validationSupport = enableValidationLayers && CheckValidationSupport();
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = MessengerCreateInfo();

		//create instance
		VkInstanceCreateInfo createInfo{};
		createInfo.sType					= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo			= &appInfo;
		createInfo.enabledExtensionCount	= static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames	= extensions.data();
		createInfo.enabledLayerCount		= validationSupport ? static_cast<uint32_t>(validationLayers.size()) : 0;
		createInfo.ppEnabledLayerNames		= validationSupport ? validationLayers.data() : nullptr;
		createInfo.pNext					= &debugCreateInfo;
		VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &mInstance), "Failed to create instance");
	}

	/**
	 * @brief Creates the validations layers` messenger
	 */
	void VkDevice::CreateDebugMessenger()
	{
		assert(enableValidationLayers);
		const VkDebugUtilsMessengerCreateInfoEXT createInfo = MessengerCreateInfo();
		VK_ASSERT(CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebugMessenger), "Failed to set up debug callback");
	}

	/**
	 * @brief Gets all the queue indices if they are available for the given device
	 * @param device Device to get the indices from
	 * @param indices Structure containing the retrieved indices
	 * @return true if all the indices are found
	 */
	bool VkDevice::FindQueueFamilies(VkPhysicalDevice device, QueueFamiliyIndices& indices)
	{
		uint32_t queueFamilyCount{ 0 };
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies{ queueFamilyCount };
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		//create a temporary window to query surface support
		VkSurfaceKHR surface;
		constexpr WindowInitInfo info
		{
			"You should not see this",
			800,
			600
		};
		const Window window{ info, false };

		VK_ASSERT(glfwCreateWindowSurface(mInstance, window.Native(), nullptr, &surface), "Failed to create surface from window");

		uint32_t i{ 0 };
		b32 supported{ false };
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.Graphics = i;

			VK_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supported), "Failed to query surface support")
			if (supported)
				indices.Present = i;

			if (indices.IsComplete())
			{
				vkDestroySurfaceKHR(mInstance, surface, nullptr);
				return true;
			}
		}
		vkDestroySurfaceKHR(mInstance, surface, nullptr);
		return false;
	}

	/**
	 * @brief Checks if a physical device has all the requested features and properties
	 * @param device Device to check from
	 * @param indices Retrieved queue indices from the device if it is suitable
	 * @return true if the device meets all the criterias
	 */
	bool VkDevice::IsDeviceSuitable(VkPhysicalDevice device, QueueFamiliyIndices& indices)
	{
		VkPhysicalDeviceProperties properties{};
		VkPhysicalDeviceFeatures features{};
		vkGetPhysicalDeviceProperties(device, &properties);
		vkGetPhysicalDeviceFeatures(device, &features);

		bool suitable{ true };
		suitable &= properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
		suitable &= FindQueueFamilies(device, indices);
		suitable &= CheckExtensionSupport(device);
		
		return suitable;
	}

	/**
	 * @brief Chooses the most suitable physical devices present in the current machine
	 */
	void VkDevice::CreatePhysicalDevice()
	{
		uint32_t deviceCount{ 0 };
		VK_ASSERT(vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr), "Failed to enumerate physical devices");
		std::vector<VkPhysicalDevice> devices{ deviceCount };
		VK_ASSERT(vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data()), "Failed to enumerate physical devices");

		for(const auto& device : devices)
		{
			//check device suitability and query queue indices
			if(IsDeviceSuitable(device, mIndices))
			{
				mPhysicalDevice = device;
				return;
			}
		}

		assert(false && "No suitable device found");
	}

	/**
	 * @brief Creates a logical device and gets the graphics and presentation queues
	 */
	void VkDevice::CreateLogicalDevice()
	{
		float queuePriority{ 1.f };

		VkDeviceQueueCreateInfo queueInfo{};
		queueInfo.sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex	= mIndices.Graphics.value();
		queueInfo.queueCount		= 1;
		queueInfo.pQueuePriorities	= &queuePriority;

		VkPhysicalDeviceFeatures enabledFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos		= &queueInfo;
		createInfo.queueCreateInfoCount		= 1;
		createInfo.pEnabledFeatures			= &enabledFeatures;
		createInfo.enabledLayerCount		= enableValidationLayers ? static_cast<u32>(validationLayers.size()) : 0;
		createInfo.ppEnabledLayerNames		= enableValidationLayers ? validationLayers.data() : nullptr;
		createInfo.enabledExtensionCount	= static_cast<u32>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames	= deviceExtensions.data();
		VK_ASSERT(vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice), "Failed to create device");

		vkGetDeviceQueue(mDevice, mIndices.Graphics.value(), 0, &mGraphicsQueue);
		vkGetDeviceQueue(mDevice, mIndices.Present.value(), 0, &mGraphicsQueue);
	}

	void VkDevice::CreateAllocator()
	{
		VmaAllocatorCreateInfo createInfo{};
		createInfo.instance = mInstance;
		createInfo.physicalDevice = mPhysicalDevice;
		createInfo.device = mDevice;
		createInfo.vulkanApiVersion = 0;

		vmaCreateAllocator(&createInfo, &mAllocator);
	}
}
