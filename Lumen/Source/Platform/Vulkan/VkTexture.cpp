#include "VkTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <utility>

#include "VkBuffer.h"
#include "VkContext.h"

namespace Lumen::Graphics::Vulkan
{
	namespace 
	{
		void TransitionLayout(VkImage image, u32 mipLevels, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, u32 layerCount)
		{
			VkCommandBuffer cmd = VkContext::Get().StartCommand();

			VkImageMemoryBarrier barrier{};
			barrier.sType							= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout						= oldLayout;
			barrier.newLayout						= newLayout;
			barrier.srcQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
			barrier.image							= image;
			barrier.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseMipLevel	= 0;
			barrier.subresourceRange.levelCount		= mipLevels;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount		= layerCount;
			barrier.srcAccessMask					= 0;
			barrier.dstAccessMask					= 0;

			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;

			if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				sourceStage			= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage	= VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage			= VK_PIPELINE_STAGE_TRANSFER_BIT;
				destinationStage	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage			= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				destinationStage	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage			= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage	= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

				sourceStage			= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage	= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			}
			else
			{
				assert(false && "Unsupported transition requested");
			}

			vkCmdPipelineBarrier(cmd, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

			VkContext::Get().EndCommand(cmd);
		}

		void GenerateMipmaps(VkImage image, VkFormat imageFormat, u32 texWidth, u32 texHeight, uint32_t mipLevels)
		{
			VkFormatProperties formatProperties;
			vkGetPhysicalDeviceFormatProperties(VkContext::Get().PhysicalDevice(), imageFormat, &formatProperties);

			if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
				assert(false && "Texture image format does not support linear blitting!");

			VkCommandBuffer cmd = VkContext::Get().StartCommand();

			VkImageMemoryBarrier barrier{};
			barrier.sType							= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image							= image;
			barrier.srcQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount		= 1;
			barrier.subresourceRange.levelCount		= 1;

			u32 mipWidth = texWidth;
			u32 mipHeight = texHeight;

			for (uint32_t i = 1; i < mipLevels; i++)
			{
				barrier.subresourceRange.baseMipLevel = i - 1;
				barrier.oldLayout		= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout		= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.srcAccessMask	= VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask	= VK_ACCESS_TRANSFER_READ_BIT;

				vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

				VkImageBlit blit{};
				blit.srcOffsets[0]					= { 0, 0, 0 };
				blit.srcOffsets[1]					= { static_cast<s32>(mipWidth), static_cast<s32>(mipHeight), 1 };
				blit.srcSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
				blit.srcSubresource.mipLevel		= i - 1;
				blit.srcSubresource.baseArrayLayer	= 0;
				blit.srcSubresource.layerCount		= 1;
				blit.dstOffsets[0]					= { 0, 0, 0 };
				blit.dstOffsets[1]					= { mipWidth > 1 ? static_cast<s32>(mipWidth) / 2 : 1, mipHeight > 1 ? static_cast<s32>(mipHeight) / 2 : 1, 1 };
				blit.dstSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
				blit.dstSubresource.mipLevel		= i;
				blit.dstSubresource.baseArrayLayer	= 0;
				blit.dstSubresource.layerCount		= 1;

				vkCmdBlitImage(cmd, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

				barrier.oldLayout		= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout		= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask	= VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask	= VK_ACCESS_SHADER_READ_BIT;

				vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

				if (mipWidth > 1)
					mipWidth /= 2;
				if (mipHeight > 1)
					mipHeight /= 2;
			}

			barrier.subresourceRange.baseMipLevel = mipLevels - 1;
			barrier.oldLayout		= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout		= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask	= VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask	= VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

			VkContext::Get().EndCommand(cmd);
		}

		Texture* CreateFuncVulkan(std::filesystem::path source)
		{
			return new VkTexture{ std::move(source) };
		}
	}

	VkTexture::VkTexture(std::filesystem::path source)
		: mSource{ std::move(source) }
	{
		LoadImage();
		CreateView();
		CreateSampler();
	}

	void VkTexture::Release()
	{
		vkDestroySampler(VkContext::Get().LogicalDevice(), mSampler, nullptr);
		vkDestroyImageView(VkContext::Get().LogicalDevice(), mView, nullptr);

		vmaDestroyImage(VkContext::Get().Device().Allocator(), mImage, mAllocation);
	}

	void VkTexture::SetInterface()
	{
		sCreateFunc = CreateFuncVulkan;
	}

	void VkTexture::LoadImage()
	{
		s32 width, height, channels;
		stbi_uc* pixels = stbi_load(mSource.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
		assert(pixels && "Failed to load texture image");

		mMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

		const u64 imageSize = static_cast<u64>(width) * height * 4;
		const VkBuffer stagingBuffer{ imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY };
		stagingBuffer.InsertData(pixels);
		stbi_image_free(pixels);

		CreateImage(stagingBuffer, width, height);
	}

	void VkTexture::CreateImage(const VkBuffer& buffer, u32 width, u32 height)
	{
		VkExtent3D extent
		{
			width,
			height,
			1
		};

		VkImageCreateInfo createInfo{};
		createInfo.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.imageType		= VK_IMAGE_TYPE_2D;
		createInfo.extent			= extent;
		createInfo.mipLevels		= mMipLevels;
		createInfo.arrayLayers		= 1;
		createInfo.format			= VK_FORMAT_R8G8B8A8_SRGB;
		createInfo.tiling			= VK_IMAGE_TILING_OPTIMAL;
		createInfo.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		createInfo.usage			= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;//src for mipmapping
		createInfo.sharingMode		= VK_SHARING_MODE_EXCLUSIVE;
		createInfo.samples			= VK_SAMPLE_COUNT_1_BIT;
		createInfo.flags			= 0;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		VmaAllocationInfo resultInfo{};
		vmaCreateImage(VkContext::Get().Device().Allocator(), &createInfo, &allocInfo, &mImage, &mAllocation, &resultInfo);

		TransitionLayout(mImage, mMipLevels, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);

		VkCommandBuffer cmd = VkContext::Get().StartCommand();

		VkBufferImageCopy region{};
		region.bufferOffset						= 0;
		region.bufferRowLength					= 0;
		region.bufferImageHeight				= 0;
		region.imageSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel		= 0;
		region.imageSubresource.baseArrayLayer	= 0;
		region.imageSubresource.layerCount		= 1;
		region.imageOffset						= { 0, 0, 0 };
		region.imageExtent						= extent;
		vkCmdCopyBufferToImage(cmd, buffer.Buffer(), mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		VkContext::Get().EndCommand(cmd);

		GenerateMipmaps(mImage, VK_FORMAT_R8G8B8A8_SRGB, width, height, mMipLevels);
		mLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//layout changed from dst by mipmapping

		mWidth = width;
		mHeight = height;
	}

	void VkTexture::CreateView()
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType							= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image							= mImage;
		createInfo.viewType							= VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format							= VK_FORMAT_R8G8B8A8_SRGB;
		createInfo.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel	= 0;
		createInfo.subresourceRange.levelCount		= mMipLevels;
		createInfo.subresourceRange.baseArrayLayer	= 0;
		createInfo.subresourceRange.layerCount		= 1;

		VK_ASSERT(vkCreateImageView(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mView), "Failed to create image view!");
	}

	void VkTexture::CreateSampler()
	{
		VkSamplerCreateInfo createInfo{};
		createInfo.sType					= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		createInfo.magFilter				= VK_FILTER_LINEAR;
		createInfo.minFilter				= VK_FILTER_LINEAR;
		createInfo.addressModeU				= VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeV				= VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeW				= VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.anisotropyEnable			= VK_TRUE;
		createInfo.maxAnisotropy			= VkContext::Get().Device().Properties().limits.maxSamplerAnisotropy;
		createInfo.borderColor				= VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		createInfo.unnormalizedCoordinates	= VK_FALSE;
		createInfo.compareEnable			= VK_FALSE;
		createInfo.compareOp				= VK_COMPARE_OP_ALWAYS;
		createInfo.mipmapMode				= VK_SAMPLER_MIPMAP_MODE_LINEAR;
		createInfo.mipLodBias				= 0.0f;
		createInfo.minLod					= 0.0f;
		createInfo.maxLod					= static_cast<float>(mMipLevels);

		VK_ASSERT(vkCreateSampler(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mSampler), "Failed to create image sampler!");
	}
}
