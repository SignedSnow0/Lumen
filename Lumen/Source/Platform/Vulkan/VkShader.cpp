#include "VkShader.h"

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
			std::vector<vk::DescriptorSetLayoutBinding> bindings{};
			for (const auto& [binding, uniform] : Uniforms)
				bindings.emplace_back(uniform.LayoutBinding);

			vk::DescriptorSetLayoutCreateInfo createInfo{};
			createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
			createInfo.pBindings	= bindings.data();

			VK_ASSERT(VkGraphics::Device().Device().createDescriptorSetLayout(&createInfo, nullptr, &Layout), "Failed to create descriptor set layout");
		}
		{
			std::vector<vk::DescriptorPoolSize> sizes{};
			if(!Uniforms.empty())
			{
				vk::DescriptorPoolSize& size = sizes.emplace_back();
				size.type				= vk::DescriptorType::eUniformBuffer;
				size.descriptorCount	= static_cast<uint32_t>(Uniforms.size());
			}

			vk::DescriptorPoolCreateInfo createInfo{};
			createInfo.maxSets			= VkSurface::BufferCount;
			createInfo.poolSizeCount	= static_cast<uint32_t>(sizes.size());
			createInfo.pPoolSizes		= sizes.data();

			VK_ASSERT(VkGraphics::Device().Device().createDescriptorPool(&createInfo, nullptr, &Pool), "Failed to create descriptor pool");
		}
		{
			const std::vector<vk::DescriptorSetLayout> layouts{ VkSurface::BufferCount, Layout };

			vk::DescriptorSetAllocateInfo allocInfo{};
			allocInfo.descriptorPool		= Pool;
			allocInfo.descriptorSetCount	= static_cast<uint32_t>(layouts.size());
			allocInfo.pSetLayouts			= layouts.data();

			VK_ASSERT(VkGraphics::Device().Device().allocateDescriptorSets(&allocInfo, Sets.data()), "Failed to allocate descriptor sets");
		}
	}

	void VkDescriptorSet::Release()
	{
		VkGraphics::Device().Device().freeDescriptorSets(Pool, static_cast<uint32_t>(Sets.size()), Sets.data());
		VkGraphics::Device().Device().destroyDescriptorPool(Pool);
		VkGraphics::Device().Device().destroyDescriptorSetLayout(Layout);
	}

	void VkDescriptorSet::AttachUniformBuffer(const VkUniformBuffer& buffer, uint32_t binding)
	{
		for(uint32_t i{ 0 }; i < VkSurface::BufferCount; i++)
		{
			vk::DescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = buffer.Buffer(i);
			bufferInfo.offset = 0;
			bufferInfo.range = buffer.Size();
	
			vk::WriteDescriptorSet uniformWrite{};
			uniformWrite.dstSet = Sets[i];
			uniformWrite.dstBinding = binding;
			uniformWrite.dstArrayElement = 0;
			uniformWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
			uniformWrite.descriptorCount = 1;
			uniformWrite.pBufferInfo = &bufferInfo;
			uniformWrite.pImageInfo = nullptr;
			uniformWrite.pTexelBufferView = nullptr;

			VkGraphics::Device().Device().updateDescriptorSets(1, &uniformWrite, 0, nullptr);
		}
	}

	VkShader::VkShader(const std::unordered_map<std::string, vk::ShaderStageFlagBits>& sources)
	{
		for(auto& [path, stage] : sources)
		{
			const auto blob = Compile(Utils::ReadFile(path), stage);
			CreateModule(blob, stage);
			Reflect(blob, stage);
		}
	}

	std::vector<vk::DescriptorSetLayout> VkShader::DescriptorsLayout() const
	{
		std::vector<vk::DescriptorSetLayout> layouts{ mDescriptorSets.size() };
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
			VkGraphics::Device().Device().destroyShaderModule(module);
	}

	/**
	 * \brief Creates a shader module
	 * \param blob The shader`s spir-v bytecode
	 * \param stage Pipeline stage of the specified module
	 */
	void VkShader::CreateModule(const std::vector<uint32_t>& blob, vk::ShaderStageFlagBits stage)
	{
		vk::ShaderModuleCreateInfo createInfo{};
		createInfo.codeSize = blob.size() * sizeof(uint32_t);
		createInfo.pCode = blob.data();

		mModules.emplace_back();
		VK_ASSERT(VkGraphics::Device().Device().createShaderModule(&createInfo, nullptr, &mModules.back()), "Failed to create shader module");

		auto& info = mPipelineInfos.emplace_back();
		info.stage = stage;
		info.module = mModules.back();
		info.pName = "main";
	}

	void VkShader::Reflect(const std::vector<uint32_t>& blob, vk::ShaderStageFlagBits stage)
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

			vk::DescriptorSetLayoutBinding layoutBinding{};
			layoutBinding.binding = binding;
			layoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
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