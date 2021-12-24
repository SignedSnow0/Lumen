#include "VkDescriptorSet.h"

#include "VkBuffer.h"
#include "VkContext.h"
#include "VkShader.h"

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
		*this = shader->GetDescriptorSet(setIndex);
	}

	VkDescriptorSet::VkDescriptorSet(const VkDescriptorSet& right)
		: mUniforms{ right.mUniforms }, mSet{ right.mSet }
	{
	}

	VkDescriptorSet& VkDescriptorSet::operator=(const VkDescriptorSet& right)
	{
		this->mUniforms = right.mUniforms;
		this->mSet = right.mSet;
		return *this;
	}

	void VkDescriptorSet::Init()
	{
		CreateLayout();
		CreatePool();
		CreateDescriptorSet();

		for(auto& [binding, uniform] : mUniforms)
		{
			uniform.Buffer = new VkUniformBuffer{ uniform.Size };
		}

		Update();

		mCreated = true;
	}

	void VkDescriptorSet::Release()
	{
		if(mCreated)
		{
			vkDestroyDescriptorPool(VkContext::Get().LogicalDevice(), mPool, nullptr); //descriptors are automatically freed
			vkDestroyDescriptorSetLayout(VkContext::Get().LogicalDevice(), mLayout, nullptr);

			mCreated = false;
		}
	}

	void VkDescriptorSet::Update()
	{
		std::vector<VkWriteDescriptorSet> writes{ mUniforms.size() };

		for (uint32_t i{ 0 }; i < VkSurface::BufferCount; i++)
		{
			u32 j = 0;
			for(auto& [binding, uniform] : mUniforms)
			{
				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer	= uniform.Buffer->Buffer(i);
				bufferInfo.offset	= 0;
				bufferInfo.range	= uniform.Buffer->Size();

				VkWriteDescriptorSet uniformWrite{};
				uniformWrite.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				uniformWrite.dstSet				= mSets[i];
				uniformWrite.dstBinding			= binding;
				uniformWrite.dstArrayElement	= 0;
				uniformWrite.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				uniformWrite.descriptorCount	= 1;
				uniformWrite.pBufferInfo		= &bufferInfo;
				uniformWrite.pImageInfo			= nullptr;
				uniformWrite.pTexelBufferView	= nullptr;

				writes[j] = uniformWrite;
				j++;
			}

			vkUpdateDescriptorSets(VkContext::Get().LogicalDevice(), static_cast<u32>(writes.size()), writes.data(), 0, nullptr);
		}
	}

	void VkDescriptorSet::SetUniform(u32 binding, const void* data)
	{
		if (mUniforms.contains(binding))
		{
			mUniforms.at(binding).Buffer->Update(data, 0);//todo get current frame
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

		VkDescriptorSetLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		createInfo.pBindings = bindings.data();

		VK_ASSERT(vkCreateDescriptorSetLayout(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mLayout), "Failed to create descriptor set layout");
	}

	void VkDescriptorSet::CreatePool()
	{
		std::vector<VkDescriptorPoolSize> sizes{};
		if (!mUniforms.empty())
		{
			VkDescriptorPoolSize& size = sizes.emplace_back();
			size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			size.descriptorCount = static_cast<uint32_t>(mUniforms.size());
		}

		VkDescriptorPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.maxSets = VkSurface::BufferCount;
		createInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
		createInfo.pPoolSizes = sizes.data();

		VK_ASSERT(vkCreateDescriptorPool(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mPool), "Failed to create descriptor pool");
	}

	void VkDescriptorSet::CreateDescriptorSet()
	{
		const std::vector<VkDescriptorSetLayout> layouts{ VkSurface::BufferCount, mLayout };

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = mPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
		allocInfo.pSetLayouts = layouts.data();

		VK_ASSERT(vkAllocateDescriptorSets(VkContext::Get().LogicalDevice(), &allocInfo, mSets.data()), "Failed to allocate descriptor sets");
	}
}
