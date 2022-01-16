#pragma once

#include "engine/Pipeline.hpp"
#include "engine/LogicalDevice.hpp"

// libs
#include <vulkan/vulkan.h>

// std
#include <vector>
#include <cassert>
#include <typeinfo>
#include <typeindex>
#include <iterator>
#include <iostream>

namespace vk_engine{
	class Displayer{
		public:
			struct PushConstant{
				std::size_t size;
				std::size_t offset;
				VkShaderStageFlags stages;
				std::uint8_t binding = 0;
			};

			Displayer(LogicalDevice &device, VkRenderPass renderPass);
			~Displayer();

			// avoid copy
			Displayer(const Displayer &) = delete;
			Displayer &operator=(const Displayer &) = delete;

			/**
			 * @brief build the displayer from the given properties
			 */
			void build();

			/**
			 * @brief add the input push constant to the displayer pushConstants 
			 * 
			 * @tparam T the type to add 
			 * @param stages the stages used by the pushConstant
			 */
			template<typename T>
			void setPushConstant(T instance = {}, VkShaderStageFlags stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT){
				PushConstant pushConstant;
				pushConstant.size = sizeof(T);

				assert(getPushConstantsSize() + pushConstant.size <= device.getPhysicalDevice().getProperties().limits.maxPushConstantsSize && "the size of the push constant cannot be greater than the max push constant size");

				pushConstant.offset = getPushConstantsSize();
				pushConstant.stages = stages;
				pushConstant.binding = static_cast<std::uint8_t>(pushConstants.size());

				pushConstants.push_back(pushConstant);
				pushConstantsData.push_back(std::make_unique<void*[]>(sizeof(T)));
				setPushConstantData(instance, pushConstant.binding);
			}

			/**
			 * @brief get the pipeline
			 * @return Pipeline& 
			 */
			Pipeline &getPipeline() noexcept {return pipeline;}

			/**
			 * @brief bind the displayer to the commandBuffer
			 * @param commandBuffer the CommandBuffer
			 */
			void bind(VkCommandBuffer commandBuffer);

			/**
			 * @brief set the data at the binding | create a copy of the given instance
			 * 
			 * @tparam T the type to transfert
			 * @param data the instance of the dype
			 * @param binding the binding where to send the data
			 */
			template<typename T>
			void setPushConstantData(const T &data, int binding){
				assert(binding < static_cast<int>(pushConstants.size()) && "the binding is greater than the count of push constants");
				assert(pushConstants[binding].size == sizeof(T) && "the size of the data to send is different than the size of the declared push constant");
				memcpy(pushConstantsData[binding].get(), &data, sizeof(T));
			}

		protected:
			void createPipelineLayout();
			size_t getPushConstantsSize() const noexcept;

			LogicalDevice &device;
			VkRenderPass renderPass;
			Pipeline pipeline;
			VkPipelineLayout pipelineLayout;

			std::vector<PushConstant> pushConstants;
			std::vector<std::unique_ptr<void*[]>> pushConstantsData;
			uint32_t pushConstantsSize;
	};
}