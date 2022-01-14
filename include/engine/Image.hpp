#pragma once

#include "engine/LogicalDevice.hpp"
#include "engine/CommandPool.hpp"

// libs
#include <vulkan/vulkan.h>

// std
#include <string>

namespace vk_engine{
	class Image{
		public:
			Image(LogicalDevice &device, CommandPool &commandPool, const std::string &filepath);
			~Image();

			enum Format{
				FORMAT_RGBA = VK_FORMAT_R8G8B8A8_SRGB,
				FORMAT_RGB = VK_FORMAT_R8G8B8_SRGB,
				FORMAT_RG = VK_FORMAT_R8G8_SRGB,
				FORMAT_R = VK_FORMAT_R8_SRGB
			};

			enum Filter{
				FILTER_NEARTEST = VK_FILTER_NEAREST,
				FILTER_LINEAR = VK_FILTER_LINEAR
			};

			/**
			 * @brief build the image from the given properties
			 * 
			 */
			void build();

			// void copy
			Image(const Image &image) = delete;
			Image &operator=(const Image &image) = delete;

			/**
			 * @brief get if the image is loaded
			 * @return true if loaded, false if not
			 */
			bool isLoaded() const noexcept {return image != VK_NULL_HANDLE;}

			/**
			 * @brief get the vulkan image
			 * @return VkImage 
			 */
			VkImage get() const noexcept {return image;}

			/**
			 * @brief get the memory of the image
			 * @return VkDeviceMemory 
			 */
			VkDeviceMemory getMemory() const noexcept {return memory;}

			/**
			 * @brief get the image view of the image
			 * @return VkImageView 
			 */
			VkImageView getView() const noexcept {return imageView;}

			/**
			 * @brief get the extent of the image
			 * @return VkExtent2D 
			 */
			VkExtent2D getExtent() const noexcept {return extent;}

			/**
			 * @brief get the width of the image
			 * @return int 
			 */
			int width() const noexcept {return extent.width;}

			/**
			 * @brief get the height of the image
			 * @return int 
			 */
			int height() const noexcept {return extent.height;}

			/**
			 * @brief get the information for a descriptor
			 * @return VkDescriptorImageInfo 
			 */
			VkDescriptorImageInfo getDescriptorInfo() const noexcept {return {sampler, imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};}

			/**
			 * @brief set the format of the image
			 * @param format the new format of the image
			 */
			void setFormat(Format format) noexcept {this->format = format;}

			/**
			 * @brief set the format of the source image
			 * @param format 
			 */
			void setSourceFormat(Format format) noexcept {srcFormat = format;}

			/**
			 * @brief set the filter of the image
			 * @param filter the new filter
			 */
			void setFilter(Filter filter) noexcept {this->filter = filter;}

			/**
			 * @brief normalize or not the image's coordonates
			 * @param normalize 
			 */
			void setNomalizedCoordonates(const bool normalize = true) noexcept {normalizeCoordonates = normalize;}

			// operator
			operator bool() const noexcept {return isLoaded();}
			operator VkImage() const noexcept {return image;}

		private:
			void load(const std::string &filepath);
			void createImage(void *pixels, uint32_t width, uint32_t height, uint32_t layerCount);
			void createImageView();
			void createSampler();
			static uint32_t formatToLayerCount(Format format) noexcept;

			LogicalDevice &device;
			CommandPool &commandPool;
			const std::string filepath;

			VkImage image = VK_NULL_HANDLE;
			VkDeviceMemory memory = VK_NULL_HANDLE;
			VkImageView imageView = VK_NULL_HANDLE;
			VkSampler sampler = VK_NULL_HANDLE;
			VkExtent2D extent;
			
			Format format = FORMAT_RGB;
			Format srcFormat = FORMAT_RGB;
			Filter filter = FILTER_LINEAR;
			bool normalizeCoordonates = true;
	};
}