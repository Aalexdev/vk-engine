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

				VkPipelineViewportStateCreateInfo viewportInfo;
				VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
				VkPipelineRasterizationStateCreateInfo rasterizationInfo;
				VkPipelineMultisampleStateCreateInfo multisampleInfo;
				VkPipelineColorBlendAttachmentState colorBlendAttachment;
				VkPipelineColorBlendStateCreateInfo colorBlendInfo;
				VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
				std::vector<VkDynamicState> dynamicStateEnables;
				VkPipelineDynamicStateCreateInfo dynamicStateInfo;
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				VkRenderPass renderPass = VK_NULL_HANDLE;
				uint32_t subpass = 0;
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
		
		private:
			void createGraphicPipeline();
			void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
			static std::vector<char> readFile(const std::string &filepath);

			LogicalDevice &device;

			VkPipeline pipeline = VK_NULL_HANDLE;
			VkShaderModule vertShaderModule;
			VkShaderModule fragShaderModule;
			
			std::unique_ptr<ConfigInfo> config;
			std::string vertPath, fragPath;

			bool builded = false;
	};
}