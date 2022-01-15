#pragma once

#include "engine/LogicalDevice.hpp"
#include "engine/SwapChain.hpp"

// libs
#include <vulkan/vulkan.h>

// std
#include <unordered_map>
#include <cassert>
#include <stdexcept>

namespace vk_engine{
	
	class Pipeline{
		public:
			enum descriptorType{
				DESCRIPTOR_SAMPLER = VK_DESCRIPTOR_TYPE_SAMPLER,
				DESCRIPTOR_COMBINED_IMAGE_SAMPLER = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				DESCRIPTOR_SAMPLED_IMAGE = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
				DESCRIPTOR_STORAGE_IMAGE = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
				DESCRIPTOR_UNIFORM_TEXEL_BUFFER = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
				DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
				DESCRIPTOR_UNIFORM_BUFFER = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				DESCRIPTOR_STORAGE_BUFFER = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				DESCRIPTOR_UNIFORM_BUFFER_DYNAMIC = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
				DESCRIPTOR_STORAGE_BUFFER_DYNAMIC = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
				DESCRIPTOR_INPUT_ATTACHMENT = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
				DESCRIPTOR_INLINE_UNIFORM_BLOCK_EXT = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT,
				DESCRIPTOR_ACCELERATION_STRUCTURE_KHR = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
				DESCRIPTOR_ACCELERATION_STRUCTURE_NV = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV,
				DESCRIPTOR_MUTABLE_VALVE = VK_DESCRIPTOR_TYPE_MUTABLE_VALVE
			};	

			enum Stage{
				STAGE_VERTEX = VK_SHADER_STAGE_VERTEX_BIT,
				STAGE_TESSELLATION_CONTROL = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
				STAGE_TESSELLATION_EVALuATION = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
				STAGE_GEOMETRY = VK_SHADER_STAGE_GEOMETRY_BIT,
				STAGE_FRAGMENT = VK_SHADER_STAGE_FRAGMENT_BIT,
				STAGE_COMPUTE_ = VK_SHADER_STAGE_COMPUTE_BIT,
				STAGE_ALL_GRAPHICS = VK_SHADER_STAGE_ALL_GRAPHICS,
				STAGE_ALL = VK_SHADER_STAGE_ALL,
				STAGE_RAYGEN = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
				STAGE_ANY_HIT = VK_SHADER_STAGE_ANY_HIT_BIT_KHR,
				STAGE_CLOSEST_HIT = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,
				STAGE_MISS_HIT = VK_SHADER_STAGE_MISS_BIT_KHR,
				STAGE_INTERSECTION = VK_SHADER_STAGE_INTERSECTION_BIT_KHR,
				STAGE_CALLABLE = VK_SHADER_STAGE_CALLABLE_BIT_KHR,
				STAGE_TASK = VK_SHADER_STAGE_TASK_BIT_NV,
				STAGE_MESH = VK_SHADER_STAGE_MESH_BIT_NV,
				STAGE_SUBPASS_SHADING = VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI
			};

			struct Descriptor{
				uint32_t count = 1;
				descriptorType type = DESCRIPTOR_UNIFORM_BUFFER;
				Stage stage = STAGE_VERTEX;
				size_t offset = 0;
				const size_t range = 0;
			};
			
			Pipeline(LogicalDevice &device, SwapChain &swapChain);
			~Pipeline();

			/**
			 * @brief build the pipeline from the given parameters
			 */
			void build();

			/**
			 * @brief register the given type at the binding
			 * 
			 * @tparam T the type to use a descriptor 
			 * @param binding the binding location of the descriptor
			 */
			template<typename T> void registerDescriptor(uint32_t binding = 0){
				assert(descriptors.find(binding) == descriptors.end() && "registering two descriptors at the same binding");
				descriptors.insert({binding, (Descriptor){.range = sizeof(T)}});
			}

			/**
			 * @brief get the descriptor at the given binding
			 * 
			 * @param binding 
			 * @return Descriptor& 
			 */
			Descriptor& get(uint32_t binding){
				assert(descriptors.find(binding) != descriptors.end() && "cannot get a non registered descriptor");
				return descriptors.find(binding)->second;
			}

			/**
			 * @brief set the count of the given descriptor
			 * 
			 * @param binding the binding of the desccriptor
			 * @param count 
			 */
			void setDescriptorCount(uint32_t binding, uint32_t count) noexcept {get(binding).count = count;}
		
		private:
			void createRenderPass(SwapChain &swapChain);
			void createDescriptorSetLayout();

			LogicalDevice &device;

			VkRenderPass renderPass;
			VkPipeline pipeline;
            VkDescriptorSetLayout descriptorSetLayout;
			
			std::unordered_map<uint32_t, Descriptor> descriptors{};
	};
}