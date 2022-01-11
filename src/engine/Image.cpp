#include "engine/Image.hpp"
#include "engine/Buffer.hpp"
#include "engine/SingleTimeCommands.hpp"

// libs
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// std
#include <stdexcept>

namespace vk_engine{
	
	Image::Image(LogicalDevice &device, CommandPool &commandPool, const std::string &filepath) : device{device}, commandPool{commandPool}, filepath{filepath}{}

	Image::~Image(){
		vkDestroySampler(device, sampler, nullptr);
		vkDestroyImageView(device, imageView, nullptr);
		vkDestroyImage(device, image, nullptr);
		vkFreeMemory(device, memory, nullptr);
	}

	void Image::build(){
		load(filepath);
	}

	void Image::load(const std::string &filepath){
		int texWidth, texHeight, texChannels;

		stbi_uc *pixels = stbi_load(filepath.c_str(), &texWidth, &texHeight, &texChannels, formatToLayerCount(srcFormat));

		if (!pixels)
			throw std::runtime_error("failed to open image : " + filepath);
		
		try {
			createImage(pixels, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), texChannels);
		} catch (const std::exception &e){
			stbi_image_free(pixels);
			throw e;
		}

		// free the image
		stbi_image_free(pixels);
	}

	void Image::createImageView(){
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = static_cast<VkFormat>(format);
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &createInfo, nullptr, &imageView) != VK_SUCCESS)
			throw std::runtime_error("failed to create image view");

	}

	void Image::createSampler(){
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = static_cast<VkFilter>(filter);
		samplerInfo.minFilter = static_cast<VkFilter>(filter);

		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = device.getPhysicalDevice().getProperties().limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = static_cast<VkBool32>(!normalizeCoordonates);
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		samplerInfo.mipLodBias = 0.f;
		samplerInfo.minLod = 0.f;
		samplerInfo.maxLod = 0.f;

		if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
			throw std::runtime_error("failed to create sampler");
	}

	void Image::createImage(void *pixels, uint32_t width, uint32_t height, uint32_t layerCount){
		VkDeviceSize imageSize = width * height * layerCount;

		Buffer stagingBuffer(device, imageSize, 1, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		stagingBuffer.map();
		stagingBuffer.write(pixels, imageSize);
		stagingBuffer.unmap();

		extent = {width, height};

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = extent.width;
		imageInfo.extent.height = extent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = static_cast<VkFormat>(format);
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0;

		if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
			throw std::runtime_error("failed to create image");
		
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = device.getPhysicalDevice().findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate image memory!");
		

		vkBindImageMemory(device, image, memory, 0);

		transitionImageLayout(commandPool, device, image, static_cast<VkFormat>(format), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(commandPool, device, stagingBuffer, image, width, height, layerCount);
		transitionImageLayout(commandPool, device, image, static_cast<VkFormat>(format), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		createImageView();
		createSampler();
	}

	uint32_t Image::formatToLayerCount(Format format) noexcept{
		switch (format){
			case FORMAT_ARGB: return 4;
			case FORMAT_RGB: return 3;
			case FORMAT_RG: return 2;
			case FORMAT_R: return 1;
			default: return 0;
		}
	}
}