#include "VkDescriptorSet.h"

#include "VkBuffer.h"
#include "VkContext.h"
#include "VkShader.h"
#include "VkTexture.h"

namespace Lumen::Graphics::Vulkan
{
	namespace 
	{
		DescriptorSet* CreateFuncVulkan(Shader* shader, u32 setIndex)
		{
			return new VkDescriptorSet{ static_cast<VkShader*>(shader), setIndex};
		}
	}

	VkUniform::~VkUniform()
	{
		delete Buffer;
	}

	VkDescriptorSet::VkDescriptorSet(VkShader* shader, u32 setIndex)
	{
		*this = shader->DescriptorSet(setIndex);
	}

	VkDescriptorSet::VkDescriptorSet(const VkDescriptorSet& right)
		: mUniforms{ right.mUniforms }, mSamplers{ right.mSamplers }, mSet{ right.mSet }
	{
	}

	VkDescriptorSet& VkDescriptorSet::operator=(const VkDescriptorSet& right)
	{
		mUniforms = right.mUniforms;
		mSamplers = right.mSamplers;
		mSet = right.mSet;
		return *this;
	}

	void VkDescriptorSet::Init()
	{
		CreateLayout();
		CreatePool();
		CreateDescriptorSet();

		for (auto& [binding, uniform] : mUniforms)
		{
			uniform.Buffer = new VkUniformBuffer{ uniform.Size };
		}

		for(u32 i{ 0 }; i < VkSurface::BufferCount; i++)
		{
			mToUpdate[i] = true;
			Update(i);
		}

		mCreated = true;
	}

	void VkDescriptorSet::Release()
	{
		if (mCreated)
		{
			vkDestroyDescriptorPool(VkContext::Get().LogicalDevice(), mPool, nullptr); //descriptors are automatically freed
			vkDestroyDescriptorSetLayout(VkContext::Get().LogicalDevice(), mLayout, nullptr);

			mCreated = false;
		}
	}

	void VkDescriptorSet::Update(u32 frame)
	{
		if (mToUpdate[frame])
		{
			std::vector<VkWriteDescriptorSet> writes{};

			for (auto& [binding, uniform] : mUniforms)
			{
				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer	= uniform.Buffer->Buffer(frame);
				bufferInfo.offset	= 0;
				bufferInfo.range	= uniform.Buffer->Size();

				VkWriteDescriptorSet& uniformWrite = writes.emplace_back();
				uniformWrite.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				uniformWrite.dstSet				= mSets[frame];
				uniformWrite.dstBinding			= binding;
				uniformWrite.dstArrayElement	= 0;
				uniformWrite.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				uniformWrite.descriptorCount	= 1;
				uniformWrite.pBufferInfo		= &bufferInfo;
				uniformWrite.pImageInfo			= nullptr;
				uniformWrite.pTexelBufferView	= nullptr;
			}

			for (auto& [binding, sampler] : mSamplers)
			{
				if (sampler.Texture)
				{
					VkDescriptorImageInfo imageInfo{};
					imageInfo.imageLayout	= sampler.Texture->Layout();
					imageInfo.sampler		= sampler.Texture->Sampler();
					imageInfo.imageView		= sampler.Texture->View();

					VkWriteDescriptorSet& samplerWrite = writes.emplace_back();
					samplerWrite.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					samplerWrite.dstSet				= mSets[frame];
					samplerWrite.dstBinding			= binding;
					samplerWrite.dstArrayElement	= 0;
					samplerWrite.descriptorType		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					samplerWrite.descriptorCount	= 1;
					samplerWrite.pImageInfo			= &imageInfo;
					samplerWrite.pBufferInfo		= nullptr;
					samplerWrite.pTexelBufferView	= nullptr;
				}
			}

			vkUpdateDescriptorSets(VkContext::Get().LogicalDevice(), static_cast<u32>(writes.size()), writes.data(), 0, nullptr);

			mToUpdate[frame] = false;
		}
	}

	void VkDescriptorSet::UpdateUniform(u32 binding, const void* data, u32 frame)
	{
		if (mUniforms.contains(binding))
		{
			mUniforms.at(binding).Buffer->Update(data, frame);
		}
	}

	void VkDescriptorSet::SetTexture(u32 binding, Texture* texture)
	{
		if (mSamplers.contains(binding))
		{
			mSamplers.at(binding).Texture = reinterpret_cast<VkTexture*>(texture);

			for (u32 i{ 0 }; i < VkSurface::BufferCount; i++)
				mToUpdate[i] = true;
		}
	}

	void VkDescriptorSet::SetInterface()
	{
		sCreateFunc = CreateFuncVulkan;
	}

	void VkDescriptorSet::CreateLayout()
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings{};
		for (const auto& [binding, uniform] : mUniforms)
			bindings.emplace_back(uniform.LayoutBinding);
		for (const auto& [binding, sampler] : mSamplers)
			bindings.emplace_back(sampler.LayoutBinding);

		VkDescriptorSetLayoutCreateInfo createInfo{};
		createInfo.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.bindingCount = static_cast<u32>(bindings.size());
		createInfo.pBindings	= bindings.data();

		VK_ASSERT(vkCreateDescriptorSetLayout(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mLayout), "Failed to create descriptor set layout");
	}

	void VkDescriptorSet::CreatePool()
	{
		std::vector<VkDescriptorPoolSize> sizes{};
		if (!mUniforms.empty())
		{
			VkDescriptorPoolSize& size = sizes.emplace_back();
			size.type				= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			size.descriptorCount	= static_cast<u32>(mUniforms.size());
		}
		if(!mSamplers.empty())
		{
			VkDescriptorPoolSize& size = sizes.emplace_back();
			size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			size.descriptorCount = static_cast<u32>(mSamplers.size());
		}

		VkDescriptorPoolCreateInfo createInfo{};
		createInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.maxSets			= VkSurface::BufferCount;
		createInfo.poolSizeCount	= static_cast<u32>(sizes.size());
		createInfo.pPoolSizes		= sizes.data();

		VK_ASSERT(vkCreateDescriptorPool(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mPool), "Failed to create descriptor pool");
	}

	void VkDescriptorSet::CreateDescriptorSet()
	{
		const std::vector<VkDescriptorSetLayout> layouts{ VkSurface::BufferCount, mLayout };

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool		= mPool;
		allocInfo.descriptorSetCount	= static_cast<u32>(layouts.size());
		allocInfo.pSetLayouts			= layouts.data();

		VK_ASSERT(vkAllocateDescriptorSets(VkContext::Get().LogicalDevice(), &allocInfo, mSets.data()), "Failed to allocate descriptor sets");
	}
}
