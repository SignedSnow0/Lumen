#include "VkShader.h"

#include <fstream>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_glsl.hpp>

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

		/**
		 * @brief Converts the vulkan stages to equivalent shaderc stages
		 * @param stage Shader stage to convert
		 * @return The converted shader stage
		 */
		shaderc_shader_kind VulkanToShaderc(VkShaderStageFlagBits stage)
		{
			switch (stage)
			{
			case VK_SHADER_STAGE_VERTEX_BIT:					return shaderc_vertex_shader;
			case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:		return shaderc_tess_control_shader;
			case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:	return shaderc_tess_evaluation_shader;
			case VK_SHADER_STAGE_GEOMETRY_BIT:					return shaderc_geometry_shader;
			case VK_SHADER_STAGE_FRAGMENT_BIT:					return shaderc_fragment_shader;
			case VK_SHADER_STAGE_COMPUTE_BIT:					return shaderc_compute_shader;
			//case VK_SHADER_STAGE_ALL_GRAPHICS:				return shaderc_callable_shader; //?
			//case VK_SHADER_STAGE_ALL:							return shaderc_callable_shader; //?
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
		 * @brief Compiles the shader to a blob
		 * @param source Shader`s source code
		 * @param stage Shader`s pipeline stage
		 * @return A buffer with the spir-v bytecode
		 */
		std::vector<u32> Compile(const std::string& source, VkShaderStageFlagBits stage)
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
				const u64 size = file.tellg();
				std::string buffer;
				buffer.resize(size);
				file.seekg(0);
				file.read(buffer.data(), size);
				file.close();
				return buffer;
			}

			return "";
		}

		Shader* CreateFuncVulkan(const std::unordered_map<std::string, ShaderStage>& sources)
		{
			return new VkShader{ sources };
		}
	}

	VkShader::VkShader(const std::unordered_map<std::string, ShaderStage>& sources)
	{
		for (auto& [path, stage] : sources)
		{
			VkShaderStageFlagBits vkStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
			switch (stage)
			{
			case ShaderStage::Vertex:
				vkStage = VK_SHADER_STAGE_VERTEX_BIT;
				break;
			case ShaderStage::Fragment:
				vkStage = VK_SHADER_STAGE_FRAGMENT_BIT;
				break;
			case ShaderStage::Compute:
				vkStage = VK_SHADER_STAGE_COMPUTE_BIT;
				break;
			}

			const auto blob = Compile(ReadFile(path), vkStage);
			CreateModule(blob, vkStage);
			Reflect(blob, vkStage);
		}
	}

	std::vector<VkDescriptorSetLayout> VkShader::DescriptorsLayout() const
	{
		std::vector<VkDescriptorSetLayout> layouts{ mDescriptorSets.size() };

		u32 i{ 0 };
		for (auto [set, descriptor] : mDescriptorSets)
		{
			descriptor.CreateLayout();
			layouts[i] = descriptor.mLayout;
			i++;
		}
		return layouts;
	}

	void VkShader::Init()
	{

	}

	/**
	 * @brief Destroys all the class` resources
	 */
	void VkShader::Release()
	{
		for (const auto& module : mModules)
			vkDestroyShaderModule(VkContext::Get().LogicalDevice(), module, nullptr);
	}

	void VkShader::SetInterface()
	{
		sCreateFunc = CreateFuncVulkan;
	}

	/**
	 * @brief Creates a shader module
	 * @param blob The shader`s spir-v bytecode
	 * @param stage Pipeline stage of the specified module
	 */
	void VkShader::CreateModule(const std::vector<u32>& blob, VkShaderStageFlagBits stage)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType	= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = blob.size() * sizeof(u32);
		createInfo.pCode	= blob.data();

		mModules.emplace_back();
		VK_ASSERT(vkCreateShaderModule(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mModules.back()), "Failed to create shader module");

		auto& info	= mPipelineInfos.emplace_back();
		info.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		info.stage	= stage;
		info.module = mModules.back();
		info.pName	= "main";
	}

	void VkShader::Reflect(const std::vector<u32>& blob, VkShaderStageFlagBits stage)
	{
		const spirv_cross::Compiler compiler{ blob };
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		for (const spirv_cross::Resource& resource : resources.uniform_buffers)
		{
			const std::string& name					= resource.name;
			const spirv_cross::SPIRType& bufferType = compiler.get_type(resource.base_type_id);
			u32 binding								= compiler.get_decoration(resource.id, spv::DecorationBinding);
			u32 set									= compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			u32 size								= static_cast<u32>(compiler.get_declared_struct_size(bufferType));

			auto memberCount = static_cast<u32>(bufferType.member_types.size());
			for (const auto& memberType : bufferType.member_types)
			{
				const spirv_cross::SPIRType& member = compiler.get_type(memberType);
				spirv_cross::SPIRType::BaseType type = member.basetype;
				//todo get members info
			}

			VkDescriptorSetLayoutBinding layoutBinding{};
			layoutBinding.binding				= binding;
			layoutBinding.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layoutBinding.descriptorCount		= 1;
			layoutBinding.stageFlags			= stage;
			layoutBinding.pImmutableSamplers	= nullptr;

			if (!mDescriptorSets.contains(set))
			{
				VkDescriptorSet dSet{};
				dSet.mSet = set;
				mDescriptorSets.insert({ set, dSet });
			}

			assert(!mDescriptorSets[set].mUniforms.contains(binding) && "Duplicated resource");
			if (!mDescriptorSets[set].mUniforms.contains(binding))
			{
				VkUniform uBuffer{ size, layoutBinding, name, stage };
				mDescriptorSets[set].mUniforms.insert({ binding, uBuffer });
			}
		}
		
		for (const spirv_cross::Resource& resource : resources.sampled_images)
		{
			u32 set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			u32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			const std::string& name = resource.name;

			VkDescriptorSetLayoutBinding layoutBinding{};
			layoutBinding.binding				= binding;
			layoutBinding.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			layoutBinding.descriptorCount		= 1;
			layoutBinding.stageFlags			= stage;
			layoutBinding.pImmutableSamplers	= nullptr;

			if (!mDescriptorSets.contains(set))
			{
				VkDescriptorSet dSet{};
				dSet.mSet = set;
				mDescriptorSets.insert({ set, dSet });
			}

			assert(!mDescriptorSets[set].mSamplers.contains(binding) && "Duplicated resource");
			if (!mDescriptorSets[set].mSamplers.contains(binding))
			{
				VkSampler vkSampler{ layoutBinding, name, stage };
				mDescriptorSets[set].mSamplers.insert({ binding, vkSampler });
			}
		}

		for (const spirv_cross::Resource& resource : resources.push_constant_buffers)
		{
			auto& type = compiler.get_type(resource.base_type_id);
			u32 size{ static_cast<u32>(compiler.get_declared_struct_size(type)) };
			u32 offset{ 0 };
			if (!mPushConstants.empty())
				offset = mPushConstants.back().offset + mPushConstants.back().size;
			
			VkPushConstantRange pushConstant;
			pushConstant.offset = offset;
			pushConstant.size = size;
			pushConstant.stageFlags = stage;

			mPushConstants.push_back(pushConstant);
		}
	}
}