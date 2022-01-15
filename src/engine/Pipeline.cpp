#include "engine/Pipeline.hpp"

// std
#include <stdexcept>
#include <vector>

namespace vk_engine{
	Pipeline::Pipeline(LogicalDevice &device, SwapChain &swapChain) : device{device}{}
	Pipeline::~Pipeline(){
		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
		vkDestroyRenderPass(device, renderPass, nullptr);
	}

	void Pipeline::createRenderPass(SwapChain &swapChain){
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChain.getSwapChainImageFormat();

		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		
		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// create the subpass and link it to the color attachement
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		// create the _renderPass
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;

		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;

		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
			throw std::runtime_error("failed to create the renderPass");
	}

	void Pipeline::createDescriptorSetLayout(){

		std::vector<VkDescriptorSetLayoutBinding> bindings(descriptors.size());

		int i=0;
		for (auto &descriptor : descriptors){
			bindings[i].binding = descriptor.first;
			bindings[i].descriptorCount = descriptor.second.count;
			bindings[i].descriptorType = static_cast<VkDescriptorType>(descriptor.second.type);
			bindings[i].stageFlags = static_cast<VkShaderStageFlags>(descriptor.second.stage);
			bindings[i].pImmutableSamplers = nullptr;
			i++;
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create the descriptor set layour");
	}

}