#pragma once

#include "engine/Instance.hpp"

// libs
#include <vulkan/vulkan.h>

// std
#include <string>
#include <bitset>
#include <array>

namespace vk_engine{
	
	enum Family{
		FAMILY_GRAPHIC = 0,
		FAMILY_PRESENT = 1,
		FAMILY_COMPUTE = 2,
		FAMILY_TRANSFER = 3,
		FAMILY_PROTECTED = 4,
		FAMILY_SPARSE_BINDING = 5,
		FAMILY_NONE = -1
	};
	static constexpr int FAMILY_TYPE_COUNT = 6;

	class PhysicalDevice{
		public:
			enum Feature{
				FEATURE_ROBUST_BUFFER_ACCES = 0,
				FEATURE_FULL_DRAW_INDEX_UINT_32 = 1,
				FEATURE_IMAGE_CUBE_ARRAY = 2,
				FEATURE_INDEPENDENT_BLEND = 3,
				FEATURE_GEOMETRY_SHADER = 4,
				FEATURE_TESSELLATION_SHADER = 5,
				FEATURE_SAMPLE_RATE_SHADING = 6,
				FEATURE_DUAL_SRC_BLEND = 7,
				FEATURE_LOGIC_OP = 8,
				FEATURE_MULTI_DRAW_INDIRECT = 9,
				FEATURE_DRAW_INDIRECT_FISRT_INSTANCE = 10,
				FEATURE_DEPTH_CLAMP = 11,
				FEATURE_DEPTH_BIAS_CLAMP = 12,
				FEATURE_FILL_MODE_NON_SOLID = 13,
				FEATURE_DEPTH_BOUNDS = 14,
				FEATURE_WIDE_LINES = 15,
				FEATURE_LARGE_POINTS = 16,
				FEATURE_ALPHA_TO_ONE = 17,
				FEATURE_MULTI_VIEWPORT = 18,
				FEATURE_SAMPLER_ANISOTROPY = 19,
				FEATURE_TEXTURE_COMPRESSION_ETC2 = 20,
				FEATURE_TEXTURE_COMPRESSION_ASTC_LDR = 21,
				FEATURE_TEXTURE_COMPRESSION_BC = 22,
				FEATURE_OCCLUSION_QUERY_PRECISE = 23,
				FEATURE_PIPELINE_STATISTICS_QUERY = 24,
				FEATURE_VERTEX_PIPELINE_STORES_AND_ATOMICS = 25,
				FEATURE_FRAGMENT_STORES_AND_ATOMICS = 26,
				FEATURE_SHADER_TESSELLATION_AND_GEOMETRY_POINTS = 27,
				FEATURE_SHADER_IMAGE_GATHER_EXTENDED = 28,
				FEATURE_SHADER_STORAGE_IMAGE_EXTENDED_FORMAT = 29,
				FEATURE_SHADER_STORAGE_IMAGE_MULTISAMPLE = 30,
				FEATURE_SHADER_STORAGE_IMAGE_READ_WITHOUT_ANY_FORMAT = 31,
				FEATURE_SHADER_STORAGE_IMAGE_WRITE_WITHOUT_ANY_FORMAT = 32,
				FEATURE_SHADER_UNIFORM_BUFFER_ARRAY_DYNAMIC_INDEXING = 33,
				FEATURE_SHADER_SAMPLED_IMAGE_ARRAY_DYNAMIC_INDEXING = 34,
				FEATURE_SHADER_STORAGE_BUFFER_ARRAY_DUNAMIC_INDEXING = 35,
				FEATURE_SHADER_STORAGE_IMAGE_ARRAY_DYNAMIC_INDEXING = 36,
				FEATURE_SHADER_CLIP_DISTANCE = 37,
				FEATURE_SHADER_CULL_DISTANCE = 38,
				FEATURE_SHADER_FLOAT_64 = 39,
				FEATURE_SHADER_INT_64 = 40,
				FEATURE_SHADER_INT_16 = 41,
				FEATURE_SHADER_RESOURCE_RESIDENCY = 42,
				FEATURE_SHADER_RESOURCE_MIN_LOD = 43,
				FEATURE_SPARSE_BINDING = 44,
				FEATURE_SPARSE_RESIDENCY_BUFFER = 45,
				FEATURE_SPARSE_RESIDENCY_IMAGE_2D = 46,
				FEATURE_SPARSE_RESIDENCY_IMAGE_3D = 47,
				FEATURE_SPARSE_RESIDENCY_2_SAMPLES = 48,
				FEATURE_SPARSE_RESIDENCY_4_SAMPLES = 49,
				FEATURE_SPARSE_RESIDENCY_8_SAMPLED = 50,
				FEATURE_SPARSE_RESIDENCY_16_SAMPLES = 51,
				FEATURE_SPARSE_RESODENCY_ALIASED = 52,
				FEATURE_VARIABLE_MUTISAMPLE_RATE = 53,
				FEATURE_INHERIED_QUERIES = 54,
				FEATURE_NONE = -1
			};

			static constexpr int FEATURES_COUNT = 55;

			struct FamilyDetails{
				uint32_t family = VK_NULL_HANDLE;
				Family type = FAMILY_NONE;
			};

			struct SwapChainSupport{
				VkSurfaceCapabilitiesKHR capabilities;
				std::vector<VkSurfaceFormatKHR> formats;
				std::vector<VkPresentModeKHR> presentModes;
			};

			PhysicalDevice(Instance &instance);
			~PhysicalDevice();

			// avoid copy
			PhysicalDevice(const PhysicalDevice &) = delete;
			PhysicalDevice &operator=(const PhysicalDevice &) = delete;

			/**
			 * @brief get the physical device instance
			 * @warning do not copy the physical device until you are not sure that it's will be deleted before this
			 * @return VkPhysicalDevice 
			 */
			VkPhysicalDevice getPhysicalDevice() const noexcept {return physicalDevice;}

			/**
			 * @brief require the given extensions on the build
			 * @param extensions the extensions, must be strings
			 */
			template<typename... Args> void pushExtension(Args... extensions){
				requiredExtensions.push_back(std::forward<Args>(extensions)...);
			}

			void requireGraphicFamily() noexcept {requiredFamilies.set(FAMILY_GRAPHIC);}
			void requirePresentFamily() noexcept {requiredFamilies.set(FAMILY_PRESENT);}
			void requireComputeFamily() noexcept {requiredFamilies.set(FAMILY_COMPUTE);}
			void requireTransferFamily() noexcept {requiredFamilies.set(FAMILY_TRANSFER);}
			void requireProtectedFamily() noexcept {requiredFamilies.set(FAMILY_PROTECTED);}
			void requireSparseBindingFamily() noexcept {requiredFamilies.set(FAMILY_SPARSE_BINDING);}

			void requireFamily(Family family) noexcept {requiredFamilies.set(family);}

			/**
			 * @brief require the given feature on the build
			 * @param feature the feature to require
			 */
			void requireFeature(Feature feature) noexcept {requiredFeatures.set(static_cast<size_t>(feature));}
			
			/**
			 * @brief found the physical device from the given parameters
			 */
			void build();

			/**
			 * @brief get the families indices
			 * @return std::vector<uint32_t> 
			 */
			std::vector<FamilyDetails> getFamilies() const noexcept {return families;}
			
			/**
			 * @brief get the family details of the given family
			 * @return the family details, if the family is not found throw an exception
			 */
			FamilyDetails getFamily(Family family) const;

			/**
			 * @brief get the biset of required features
			 * @return std::bitset<FEATURES_COUNT> 
			 */
			std::bitset<FEATURES_COUNT> getRequiredFeatures() const noexcept {return requiredFeatures;}

			/**
			 * @brief get the required features as a VkPhysicalDeviceFeatures structure
			 * @return VkPhysicalDevice 
			 */
			VkPhysicalDeviceFeatures getRequiredFeaturesStruct() const noexcept;

			/**
			 * @brief get the device peropeties
			 * @return VkPhysicalDeviceProperties 
			 */
			VkPhysicalDeviceProperties getProperties() const noexcept {return properties;}

			/**
			 * @brief get the support of the swapChain of the physical device
			 * @return SwapChainSupport 
			 */
			SwapChainSupport getSwapChainSupport() const noexcept {return swapChainSupport;}

			/**
			 * @brief return the supported format between the given formats
			 * 
			 * @param candidates the formats to try
			 * @param tiling the tiling of the image
			 * @param features the features of the format
			 * @return VkFormat 
			 */
			VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

			/**
			 * @brief get the memory type from the given filter and properties
			 * 
			 * @param typeFilter the filter used to found the image
			 * @param properties the memory properties
			 * @return uint32_t 
			 */
			uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

			// operators
			operator VkPhysicalDevice() const noexcept {return physicalDevice;}
			void operator<<(const std::string &extention) {pushExtension(extention);}
			void operator<<(const Feature &feature) {requireFeature(feature);}

		private:
			void checkDeviceExtensionSupport(VkPhysicalDevice device);
			bool isSuitableDevice(VkPhysicalDevice device);
			std::array<uint32_t, FAMILY_TYPE_COUNT> getFamilies(VkPhysicalDevice physicalDevice);
			bool checkDeviceExtensions(VkPhysicalDevice device);
			SwapChainSupport getSwapChainSupport(VkPhysicalDevice device);
			bool checkFeatures(VkPhysicalDeviceFeatures features);

			Instance &instance;
			VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
			VkPhysicalDeviceProperties properties;
			SwapChainSupport swapChainSupport;

			std::vector<FamilyDetails> families;
			std::vector<std::string> requiredExtensions;
			std::bitset<FEATURES_COUNT> requiredFeatures;
			std::bitset<FAMILY_TYPE_COUNT> requiredFamilies;
	};	
}