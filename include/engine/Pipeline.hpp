#pragma once

#include "engine/LogicalDevice.hpp"

// libs
#include <vulkan/vulkan.h>

// std
#include <unordered_map>
#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>

namespace vk_engine{
	class Pipeline{
		public:
			struct ConfigInfo {
				ConfigInfo(){defaultPipelineConfigInfo(*this);}

				ConfigInfo(const ConfigInfo&) = delete;
				ConfigInfo &operator=(const ConfigInfo&) = delete;

				VkPipelineViewportStateCreateInfo viewportInfo{};
				VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
				VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
				VkPipelineMultisampleStateCreateInfo multisampleInfo{};
				VkPipelineColorBlendAttachmentState colorBlendAttachment{};
				VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
				VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
				std::vector<VkDynamicState> dynamicStateEnables{};
				VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
				VkRenderPass renderPass = VK_NULL_HANDLE;
				uint32_t subpass = 0;
			};

			struct PushConstant{
				std::size_t size;
				std::size_t offset;
				VkShaderStageFlags stages;
				std::uint8_t binding = 0;
			};
			
			Pipeline(LogicalDevice &device);
			~Pipeline();

			// void vopy
			Pipeline(const Pipeline &) = delete;
			Pipeline &operator=(const Pipeline &) = delete;

			/**
			 * @brief build the pipeline from the given parameters
			 */
			void build();

			/**
			 * @brief return true if the pipeline is builded, false if not
			 */
			bool isBuilded() const noexcept {return builded;}

			/**
			 * @brief get the default pipeline configuration
			 * @param configInfo a reference to a ConfigInfo instance
			 */
			static void defaultPipelineConfigInfo(ConfigInfo &configInfo);

			/**
			 * @brief get a reference to teh current configuration of the pipeline
			 * @return ConfigInfo& 
			 */
			ConfigInfo &getConfig() noexcept {return *config;}

			/**
			 * @brief set the path to the fragment and vertex files
			 * 
			 * @param frag the fragment file
			 * @param vert the vertex file
			 */
			void setShaderFiles(const std::string &frag, const std::string &vert){fragPath = frag; vertPath = vert;}

			/**
			 * @brief set the path to the vertex shader
			 * @param filepath the path to the file
			 */
			void setVertex(const std::string &filepath) {vertPath = filepath;}

			/**
			 * @brief set the path to the fgragment shader
			 * @param filepath the path to the file
			 */
			void setFragment(const std::string &filepath) {fragPath = filepath;}

			/**
			 * @brief bind the pipeline to the commandBuffer
			 * @param commandBuffer 
			 */
			void bind(VkCommandBuffer commandBuffer);

			/**
			 * @brief add the input push constant to the pipeline pushConstants 
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

			/**
			 * @brief bind the pushConstants into the commandBuffer to be used by a pipeline
			 * @param commandBuffer 
			 * @param stages the stages where the push constant will be accesible
			 */
			void bindPushConstants(VkCommandBuffer commandBuffer, VkShaderStageFlags stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT) const noexcept;

			/**
			 * @brief bind the given push constant to the pipeline
			 * @param commandBuffer the commandBuffer
			 * @param binding the binding of the push constants
			 * @param stages the stages whgere the push constants will be accessible
			 */
			void bindPushConstant(VkCommandBuffer commandBuffer, int binding, VkShaderStageFlags stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT) const noexcept;

			/**
			 * @brief set the render pass of the pipeline
			 * @param renderPass 
			 */
			void setRenderPass(VkRenderPass renderPass) {
				assert(!builded && "cannot set the renderPass of the pipeline after builded it");
				config->renderPass = renderPass;
			}
		
		private:
			void createGraphicPipeline();
			void createPipelineLayout();
			void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
			size_t getPushConstantsSize() const noexcept;
			static std::vector<char> readFile(const std::string &filepath);

			LogicalDevice &device;

			VkPipeline pipeline = VK_NULL_HANDLE;
			VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
			VkShaderModule vertShaderModule;
			VkShaderModule fragShaderModule;
			
			std::unique_ptr<ConfigInfo> config;
			std::string vertPath, fragPath;

			std::vector<PushConstant> pushConstants;
			std::vector<std::unique_ptr<void*[]>> pushConstantsData;
			uint32_t pushConstantsSize;

			bool builded = false;
	};
}