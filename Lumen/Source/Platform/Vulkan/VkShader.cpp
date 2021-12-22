#include "VkShader.h"

#include <fstream>
#include <iostream>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include "VkBuffer.h"
#include "VkContext.h"

#ifdef _DEBUG
#pragma comment(lib, "shaderc_combinedd.lib")
#pragma comment(lib, "spirv-cross-cored.lib")
#else
#pragma comment(lib, "shaderc_combined.lib")
#pragma comment(lib, "spirv-cross-core.lib")
#endif

namespace Lumen::Graphics::Vulkan
{
	namespace 
	{
		shaderc::Compiler compiler;

		struct ShaderInfos
		{
			std::filesystem::path SourcePath;
			VkShaderStageFlagBits Stage;
		};

		ShaderInfos engineShaderInfos[]
		{
			{ "D:/Dev/FrostEngine/Engine/Source/Graphics/Vulkan/Shaders/vertex.vert", VK_SHADER_STAGE_VERTEX_BIT },
			{ "D:/Dev/FrostEngine/Engine/Source/Graphics/Vulkan/Shaders/fragment.frag", VK_SHADER_STAGE_FRAGMENT_BIT }
		};

		/**
		 * \brief Converts the vulkan stages to equivalent shaderc stages
		 * \param stage Shader stage to convert
		 * \return The converted shader stage
		 */
		shaderc_shader_kind VulkanToShaderc(VkShaderStageFlagBits stage)
		{
			switch (stage)
			{
			case VK_SHADER_STAGE_VERTEX_BIT:					return shaderc_vertex_shader;
			case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:		return shaderc_tess_control_shader;
			case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:	return shaderc_tess_evaluation_shader;
			case VK_SHADER_STAGE_GEOMETRY_BIT:				return shaderc_geometry_shader;
			case VK_SHADER_STAGE_FRAGMENT_BIT:				return shaderc_fragment_shader;
			case VK_SHADER_STAGE_COMPUTE_BIT:					return shaderc_compute_shader;
			//case VK_SHADER_STAGE_ALL_GRAPHICS:				return shaderc_callable_shader; //?
			//case VK_SHADER_STAGE_ALL:						return shaderc_callable_shader; //?
			case VK_SHADER_STAGE_RAYGEN_BIT_KHR:				return shaderc_raygen_shader;
			case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:				return shaderc_anyhit_shader;
			case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:			return shaderc_closesthit_shader;
			case VK_SHADER_STAGE_MISS_BIT_KHR:					return shaderc_miss_shader;
			case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:			return shaderc_intersection_shader;
			case VK_SHADER_STAGE_CALLABLE_BIT_KHR:				return shaderc_callable_shader;
			case VK_SHADER_STAGE_TASK_BIT_NV:					return shaderc_task_shader;
			case VK_SHADER_STAGE_MESH_BIT_NV:					return shaderc_glsl_mesh_shader;
			//case VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI:	return shaderc_callable_shader; //?
			}
			return {};
		}

		/**
		 * \brief Compiles the shader to a blob
		 * \param source Shader`s source code
		 * \param stage Shader`s pipeline stage
		 * \return A buffer with the spir-v bytecode
		 */
		std::vector<uint32_t> Compile(const std::string& source, VkShaderStageFlagBits stage)
		{
			shaderc::CompileOptions options;
			options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);

			const shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, VulkanToShaderc(stage), "", options);

			assert(module.GetCompilationStatus() == shaderc_compilation_status_success);

			return { module.cbegin(), module.cend() };
		}

		std::string ReadFile(const std::filesystem::path& filePath)
		{
			std::ifstream file(filePath, std::ios::ate | std::ios::binary);
			if (file.is_open())
			{
				const size_t size = file.tellg();
				std::string buffer;
				buffer.resize(size);
				file.seekg(0);
				file.read(buffer.data(), size);
				file.close();
				return buffer;
			}

			return "";
		}
	}

	VkDescriptorSet::VkDescriptorSet(const VkDescriptorSet& right)
		: Uniforms{right.Uniforms}, Set{ right.Set }
	{
	}

	VkDescriptorSet& VkDescriptorSet::operator=(const VkDescriptorSet& right)
	{
		this->Uniforms = right.Uniforms;
		this->Set = right.Set;
		return *this;
	}

	void VkDescriptorSet::Initialize()
	{
		{
			std::vector<VkDescriptorSetLayoutBinding> bindings{};
			for (const auto& [binding, uniform] : Uniforms)
				bindings.emplace_back(uniform.LayoutBinding);

			VkDescriptorSetLayoutCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
			createInfo.pBindings	= bindings.data();

			VK_ASSERT(vkCreateDescriptorSetLayout(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &Layout), "Failed to create descriptor set layout");
		}
		{
			std::vector<VkDescriptorPoolSize> sizes{};
			if(!Uniforms.empty())
			{
				VkDescriptorPoolSize& size = sizes.emplace_back();
				size.type				= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				size.descriptorCount	= static_cast<uint32_t>(Uniforms.size());
			}

			VkDescriptorPoolCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			createInfo.maxSets			= VkSurface::BufferCount;
			createInfo.poolSizeCount	= static_cast<uint32_t>(sizes.size());
			createInfo.pPoolSizes		= sizes.data();

			VK_ASSERT(vkCreateDescriptorPool(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &Pool), "Failed to create descriptor pool");
		}
		{
			const std::vector<VkDescriptorSetLayout> layouts{ VkSurface::BufferCount, Layout };

			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.descriptorPool		= Pool;
			allocInfo.descriptorSetCount	= static_cast<uint32_t>(layouts.size());
			allocInfo.pSetLayouts			= layouts.data();

			VK_ASSERT(vkAllocateDescriptorSets(VkContext::Get().LogicalDevice(), &allocInfo, Sets.data()), "Failed to allocate descriptor sets");
		}
	}

	void VkDescriptorSet::Release()
	{
		vkFreeDescriptorSets(VkContext::Get().LogicalDevice(), Pool, static_cast<uint32_t>(Sets.size()), Sets.data());
		vkDestroyDescriptorPool(VkContext::Get().LogicalDevice(), Pool, nullptr);
		vkDestroyDescriptorSetLayout(VkContext::Get().LogicalDevice(), Layout, nullptr);
	}

	void VkDescriptorSet::AttachUniformBuffer(const VkUniformBuffer& buffer, uint32_t binding)
	{
		for(uint32_t i{ 0 }; i < VkSurface::BufferCount; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = buffer.Buffer(i);
			bufferInfo.offset = 0;
			bufferInfo.range = buffer.Size();
	
			VkWriteDescriptorSet uniformWrite{};
			uniformWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			uniformWrite.dstSet = Sets[i];
			uniformWrite.dstBinding = binding;
			uniformWrite.dstArrayElement = 0;
			uniformWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uniformWrite.descriptorCount = 1;
			uniformWrite.pBufferInfo = &bufferInfo;
			uniformWrite.pImageInfo = nullptr;
			uniformWrite.pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(VkContext::Get().LogicalDevice(), 1, &uniformWrite, 0, nullptr);
		}
	}

	VkShader::VkShader(const std::unordered_map<std::string, VkShaderStageFlagBits>& sources)
	{
		for(auto& [path, stage] : sources)
		{
			const auto blob = Compile(ReadFile(path), stage);
			CreateModule(blob, stage);
			Reflect(blob, stage);
		}
	}

	std::vector<VkDescriptorSetLayout> VkShader::DescriptorsLayout() const
	{
		std::vector<VkDescriptorSetLayout> layouts{ mDescriptorSets.size() };
		uint32_t i{ 0 };
		for (auto [set, descriptor] : mDescriptorSets)
		{
			descriptor.Initialize();
			layouts[i] = descriptor.Layout;
			i++;
		}

		return layouts;
	}

	/**
	 * \brief Destroys all the class` resources
	 */
	void VkShader::Release()
	{
		for(const auto& module : mModules)
			vkDestroyShaderModule(VkContext::Get().LogicalDevice(), module, nullptr);
	}

	/**
	 * \brief Creates a shader module
	 * \param blob The shader`s spir-v bytecode
	 * \param stage Pipeline stage of the specified module
	 */
	void VkShader::CreateModule(const std::vector<uint32_t>& blob, VkShaderStageFlagBits stage)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = blob.size() * sizeof(uint32_t);
		createInfo.pCode = blob.data();

		mModules.emplace_back();
		VK_ASSERT(vkCreateShaderModule(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mModules.back()), "Failed to create shader module");

		auto& info = mPipelineInfos.emplace_back();
		info.stage = stage;
		info.module = mModules.back();
		info.pName = "main";
	}

	void VkShader::Reflect(const std::vector<uint32_t>& blob, VkShaderStageFlagBits stage)
	{
		const spirv_cross::Compiler compiler{ blob };
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		for(const spirv_cross::Resource& resource : resources.uniform_buffers)
		{
			const std::string& name = resource.name;
			const spirv_cross::SPIRType& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			uint32_t set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);

			auto memberCount = static_cast<uint32_t>(bufferType.member_types.size());
			for(const auto& memberType : bufferType.member_types)
			{
				const spirv_cross::SPIRType& member = compiler.get_type(memberType);
				spirv_cross::SPIRType::BaseType type = member.basetype;
				//todo get members info
			}

			VkDescriptorSetLayoutBinding layoutBinding{};
			layoutBinding.binding = binding;
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layoutBinding.descriptorCount = 1;
			layoutBinding.stageFlags = stage;
			layoutBinding.pImmutableSamplers = nullptr;

			if(!mDescriptorSets.contains(set))
			{
				VkDescriptorSet dSet{};
				dSet.Set = set;
				mDescriptorSets.insert({ set, dSet });
			}

			assert(!mDescriptorSets[set].Uniforms.contains(binding) && "Duplicated resource");
			if(!mDescriptorSets[set].Uniforms.contains(binding))
			{
				VkUniform uBuffer{ layoutBinding, name, stage };
				mDescriptorSets[set].Uniforms.insert({ binding, uBuffer });
			}
		}
	}

	//------------------------------Engine shaders--------------------------------//
	std::vector<VkShader*> EngineShaders::sShaders{ Count };

	/**
	 * \brief Initializes the default shaders subsystem
	 */
	void EngineShaders::Initialize()
	{
		sShaders[0] = new VkShader{ { { engineShaderInfos[0].SourcePath.string(), engineShaderInfos[0].Stage }, { engineShaderInfos[1].SourcePath.string(), engineShaderInfos[1].Stage } } };
	}

	/**
	 * \brief Shuts down the subsystem
	 */
	void EngineShaders::Shutdown()
	{
		for (const auto& shader : sShaders)
			delete shader;
	}
}