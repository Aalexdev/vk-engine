#pragma once

#include "engine/Pipeline.hpp"
#include "engine/LogicalDevice.hpp"

// libs
#include <vulkan/vulkan.h>

// std
#include <vector>
#include <cassert>

namespace vk_engine{
	class Displayer{
		public:
			struct PushConstant{
				std::size_t size;
				std::size_t offset;
				VkShaderStageFlags stages;
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

				assert(getPushConstantsSize() + pushConstant.size < device.getPhysicalDevice().getProperties().limits.maxPushConstantsSize && "the size of the push constant cannot be greater than the max push constant size");

				pushConstant.offset = getPushConstantsSize();
				pushConstant.stages = stages;
				pushConstants.push_back(pushConstant);
				pushConstantsData.push_back(static_cast<void*>(&instance));
			}

			/**
			 * @brief get the pipeline
			 * @return Pipeline& 
			 */
			Pipeline &getPipeline() noexcept {return pipeline;}

		protected:
			void createPipelineLayout();
			size_t getPushConstantsSize() const noexcept;

			LogicalDevice &device;
			VkRenderPass renderPass;
			Pipeline pipeline;
			VkPipelineLayout pipelineLayout;

			std::vector<PushConstant> pushConstants;
			std::vector<void *> pushConstantsData;
	};
}