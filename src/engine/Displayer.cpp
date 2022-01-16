#include "engine/Displayer.hpp"

// std
#include <stdexcept>

namespace vk_engine{
	
	Displayer::Displayer(LogicalDevice &device, VkRenderPass renderPass) : device{device}, renderPass{renderPass}, pipeline{device}{
		Pipeline::defaultPipelineConfigInfo(pipeline.getConfig());
	}

	Displayer::~Displayer(){
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	}

	void Displayer::build(){
		createPipelineLayout();

		pipeline.getConfig().pipelineLayout = pipelineLayout;
		pipeline.getConfig().renderPass = renderPass;

		pipeline.build();
	}

	void Displayer::createPipelineLayout(){
		std::vector<VkPushConstantRange> pushConstantRanges(pushConstants.size());
	
		for (int i=0; i<static_cast<int>(pushConstantRanges.size()); i++){
			pushConstantRanges[i].offset = pushConstants[i].offset;
			pushConstantRanges[i].size = pushConstants[i].size;
			pushConstantRanges[i].stageFlags = pushConstants[i].stages;
		}

		VkPipelineLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
		createInfo.pPushConstantRanges = pushConstantRanges.data();

		// TODO
		createInfo.setLayoutCount = 0;
		createInfo.pSetLayouts = nullptr;

		createInfo.flags = 0;

		if (vkCreatePipelineLayout(device, &createInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create pipeline layout");
	}

	size_t Displayer::getPushConstantsSize() const noexcept{
		size_t sum = 0;
		for (auto &p : pushConstants){
			sum += p.size;
		}
		return sum;
	}

}