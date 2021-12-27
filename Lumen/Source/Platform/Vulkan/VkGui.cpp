﻿#include "VkGui.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_vulkan_with_textures.h>
#ifdef _WIN64
#include <imgui/backends/imgui_impl_glfw.h>
#endif
#include "VkContext.h"
#include "VkRenderPass.h"
#include "VkSurface.h"
#include "Core/Window.h"

namespace Lumen::Graphics::Vulkan
{
	namespace 
	{
		Gui* CreateFuncVulkan()
		{
			return new VkGui{};
		}
	}

	void VkGui::Init(const Window* target, Surface* surface)
	{
		mTarget = dynamic_cast<VkSurface*>(surface);

		CreatePool();
		CreateRenderPass();
		InitImgui(target);

		VkCommandBuffer cmd = VkContext::Get().StartCommand();

		ImGui_ImplVulkan_CreateFontsTexture(cmd);

		VkContext::Get().EndCommand(cmd);

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	void VkGui::Release()
	{
		ImGui_ImplVulkan_Shutdown();

		delete mRenderPass;
		vkDestroyDescriptorPool(VkContext::Get().LogicalDevice(), mPool, nullptr);
	}

	void VkGui::Begin()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();
	}

	void VkGui::End()
	{
		mRenderPass->Begin(mTarget->CurrentFrame());

		ImGui::Render();

		if(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		ImGui_ImplVulkan_RenderDrawData(::ImGui::GetDrawData(), mTarget->CommandBuffer());

		mRenderPass->End();
	}

	void VkGui::SetInterface()
	{
		sCreateFunc = CreateFuncVulkan;
	}

	void VkGui::CreatePool()
	{
		constexpr VkDescriptorPoolSize poolSizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo createInfo = {};
		createInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.flags			= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		createInfo.maxSets			= 1000;
		createInfo.poolSizeCount	= std::size(poolSizes);
		createInfo.pPoolSizes		= poolSizes;

		VK_ASSERT(vkCreateDescriptorPool(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mPool), "Failed to create imgui descriptor pool");
	}

	void VkGui::CreateRenderPass()
	{
		Attachment color{};
		color.AsColor(VK_FORMAT_B8G8R8A8_SRGB);
		mRenderPass = new VkRenderPass{ { color }, mTarget };
	}

	void VkGui::InitImgui(const Window* target)
	{
		ImGui::CreateContext();

		ImGuiIO& io{ ImGui::GetIO() };
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

#ifdef _WIN64
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui_ImplGlfw_InitForVulkan(target->Native(), true);
#endif

		ImGui_ImplVulkan_InitInfo initInfo{};
		initInfo.Instance = VkContext::Get().Instance();
		initInfo.PhysicalDevice = VkContext::Get().PhysicalDevice();
		initInfo.Device = VkContext::Get().LogicalDevice();
		initInfo.Queue = VkContext::Get().Device().GraphicsQueue();
		initInfo.DescriptorPool = mPool;
		initInfo.MinImageCount = VkSurface::BufferCount;
		initInfo.ImageCount = VkSurface::BufferCount;
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&initInfo, mRenderPass->Get());
	}
}
