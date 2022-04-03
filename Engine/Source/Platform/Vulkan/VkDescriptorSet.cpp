#include "VkDescriptorSet.h"

#include "VkBuffer.h"
#include "VkContext.h"
#include "VkShader.h"
#include "VkTexture.h"

namespace Lumen::Graphics::Vulkan
{
	namespace 
	{
		DescriptorSet* CreateFuncVulkan(Shader* shader, const u32 setIndex)
		{
			return new VkDescriptorSet{ static_cast<VkShader*>(shader), setIndex};
		}
	}

	VkUniform::~VkUniform()
	{
		delete Buffer;
	}

	/**
	 * \brief Creates a new DescriptorSet from a shader
	 * \param shader The shader to create the DescriptorSet from
	 * \param setIndex The index of the shaders DescriptorSet as in shaders code
	 */
	VkDescriptorSet::VkDescriptorSet(VkShader* shader, const u32 setIndex)
		: mShader(shader)
	{
		*this = shader->DescriptorSet(setIndex);
	}

	/**
	 * \brief Creates a new DescriptorSet copied from another DescriptorSet
	 * \param right The DescriptorSet to copy
	 */
	VkDescriptorSet::VkDescriptorSet(const VkDescriptorSet& right)
		: mUniforms{ right.mUniforms }, mSamplers{ right.mSamplers }, mSet{ right.mSet }, mShader{ right.mShader }
	{
	}

	/**
	 * \brief Creates a new DescriptorSet from a copied DescriptorSet
	 * \param right The DescriptorSet to copy
	 * \return The copied DescriptorSet
	 */
	VkDescriptorSet& VkDescriptorSet::operator=(const VkDescriptorSet& right)
	{
		if(this == &right)
			return *this;

		mUniforms = right.mUniforms;
		mSamplers = right.mSamplers;
		mSet = right.mSet;
		return *this;
	}

	/**
	 * \brief Returns the DescriptorSet's index in the shader
	 */
	u32 VkDescriptorSet::SetIndex() const { return mSet; }

	/**
	 * \brief Returns the native DescriptorSet
	 * \param frame The frame to get the DescriptorSet from
	 */
	::VkDescriptorSet VkDescriptorSet::Set(const u32 frame) const { return mSets.at(frame); }

	/**
	 * \brief Initializes the DescriptorSets resources
	 */
	void VkDescriptorSet::Init()
	{
		CreateLayout();
		CreatePool();
		CreateDescriptorSet();

		// Create the uniform buffers for every uniform in the shader
		for (auto& [binding, uniform] : mUniforms)
		{
			uniform.Buffer = new VkUniformBuffer{ uniform.Size };
		}

		//Binds the uniform buffers to the descriptor set
		for(u32 i{ 0 }; i < VkSurface::BufferCount; i++)
		{
			mToUpdate[i] = true;
			Update(i);
		}

		mCreated = true;
	}

	/**
	 * \brief Returns the DescriptorSets resources
	 */
	void VkDescriptorSet::Release()
	{
		if (mCreated)
		{
			vkDestroyDescriptorPool(VkContext::Get().LogicalDevice(), mPool, nullptr); //descriptors are automatically freed
			vkDestroyDescriptorSetLayout(VkContext::Get().LogicalDevice(), mLayout, nullptr);

			mCreated = false;
		}
	}

	/**
	 * \brief Updates the DescriptorSets buffers attached to it
	 * \param frame The frame to update the DescriptorSets from
	 */
	void VkDescriptorSet::Update(const u32 frame)
	{
		if (mToUpdate[frame])
		{
			std::vector<VkWriteDescriptorSet> writes{};

			//Creates a uniform write for every uniform
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

			//Create a sampler write for every sampler
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

	/**
	 * \brief Updates the value of a uniform if it exists
	 * \param binding The binding of the uniform as in shader code
	 * \param data The data to upload to the uniform
	 * \param frame The frame to update the uniform from
	 */
	void VkDescriptorSet::UpdateUniform(const u32 binding, const void* data, const u32 frame)
	{
		if (mUniforms.contains(binding))
		{
			mUniforms.at(binding).Buffer->Update(data, frame);
		}
	}

	/**
	 * \brief Sets a texture to a sampler if it exists
	 * \param binding The binding of the sampler as in shader code
	 * \param texture The texture to set to the sampler
	 */
	void VkDescriptorSet::SetTexture(const u32 binding, Texture* texture)
	{
		if (mSamplers.contains(binding))
		{
			mSamplers.at(binding).Texture = reinterpret_cast<VkTexture*>(texture);

			//Calls the update function to update the descriptor set on the next frame
			//Every frame contains the same texture because it is supposed to be immutable
			for (u32 i{ 0 }; i < VkSurface::BufferCount; i++)
				mToUpdate[i] = true;
		}
	}

	void VkDescriptorSet::SetInterface()
	{
		sCreateFunc = CreateFuncVulkan;
	}

	/**
	 * \brief Creates the DescriptorSetLayout from the shader
	 */
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

	/**
	 * \brief Allocates the descriptor sets from the pool
	 */
	void VkDescriptorSet::CreateDescriptorSet()
	{
		//creates a descriptor set for every frame
		const std::vector<VkDescriptorSetLayout> layouts{ VkSurface::BufferCount, mLayout };

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool		= mPool;
		allocInfo.descriptorSetCount	= static_cast<u32>(layouts.size());
		allocInfo.pSetLayouts			= layouts.data();

		VK_ASSERT(vkAllocateDescriptorSets(VkContext::Get().LogicalDevice(), &allocInfo, mSets.data()), "Failed to allocate descriptor sets");
	}
}
