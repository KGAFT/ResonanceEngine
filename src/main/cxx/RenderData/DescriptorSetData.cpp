//
// Created by kgaft on 3/13/24.
//

#include "DescriptorSetData.hpp"

void DescriptorSetData::destroy() {
    destroyed = true;
    for (auto &item: uniformBuffers) {
        for (auto &citem: item) {
            citem.second->destroy();
        }
    }
    for (auto &item: samplers) {
        for (auto &citem: item) {
            citem.second->sampler->destroy();
        }
    }
}

void DescriptorSetData::createUniformBuffer(std::shared_ptr<LogicalDevice> device, uint32_t binding, size_t size) {
    if (instanceCount <= 1) {
        if (uniformBuffers.empty())
            uniformBuffers.push_back(std::map<uint32_t, std::shared_ptr<UniformBuffer> >());
        uniformBuffers[0].insert(std::pair<uint32_t, std::shared_ptr<UniformBuffer> >(binding,
            std::make_shared<UniformBuffer>(
                device, size)));
    } else {
        throw std::runtime_error(
            "Error: you set instance count more than 1, in this order you need to use function with instance number specified");
    }
}

void DescriptorSetData::createUniformBuffer(std::shared_ptr<LogicalDevice> device, uint32_t binding, size_t size,
    uint32_t instance) {
    if (uniformBuffers.size() <= instance) {
        throw std::runtime_error("Error: you specified not existing instance");
    }
    uniformBuffers[instance].insert(std::pair<uint32_t, std::shared_ptr<UniformBuffer> >(binding,
        std::make_shared<UniformBuffer>(
            device, size)));
}

void DescriptorSetData::createSampler(std::shared_ptr<LogicalDevice> device, uint32_t binding) {
    if (instanceCount <= 1) {
        if (samplers.empty())
            samplers.push_back(std::map<uint32_t, std::shared_ptr<SamplerDataInfo> >());
        samplers[0].insert(std::pair<uint32_t, std::shared_ptr<SamplerDataInfo> >(binding,
            std::make_shared<SamplerDataInfo>(SamplerDataInfo{
                std::make_shared<Sampler>(device), vk::ImageView(), vk::ImageLayout::eUndefined
            })));
    } else {
        throw std::runtime_error(
            "Error: you set instance count more than 1, in this order you need to use function with instance number specified");
    }
}

void DescriptorSetData::createSampler(std::shared_ptr<LogicalDevice> device, uint32_t binding, uint32_t instance) {
    if (samplers.size() <= instance) {
        throw std::runtime_error("Error: you specified not existing instance");
    }
    samplers[instance].insert(std::pair<uint32_t, std::shared_ptr<SamplerDataInfo> >(
        binding, std::make_shared<SamplerDataInfo>(SamplerDataInfo{
            std::make_shared<Sampler>(device), vk::ImageView()
        })));
}

void DescriptorSetData::confirmAndWriteDataToDescriptorSet() const {
    DescriptorBufferInfo bufferInfo = {};
    DescriptorImageInfo imageInfo = {};
    for (uint32_t i = 0; i < instanceCount; i++) {
        for (const auto &item: uniformBuffers[i]) {
            bufferInfo = {
                {{item.second->getBuffer(), 0, VK_WHOLE_SIZE}}, item.first, vk::DescriptorType::eUniformBuffer
            };
            descriptorSet->addBufferInfo(bufferInfo);
        }
        for (const auto &item: samplers[i]) {
            imageInfo = {
                {{item.second->sampler->getSampler(), item.second->imageView, item.second->layout}}, item.first,
                vk::DescriptorType::eCombinedImageSampler
            };
            descriptorSet->addImageInfo(imageInfo);
        }
    }
    if (instanceCount > 1) {
        descriptorSet->setBufferInfoPerInstanceAmount((uint32_t)uniformBuffers[0].size());
        descriptorSet->setImageInfoPerInstanceAmount((uint32_t)samplers[0].size());
    }
    descriptorSet->updateDescriptors();
    descriptorSet->clearObjectsInfos();
}

std::shared_ptr<SamplerDataInfo> DescriptorSetData::getSamplerByBinding(uint32_t binding) {
    return samplers[0][binding];
}

std::shared_ptr<UniformBuffer> DescriptorSetData::getUniformBufferByBinding(uint32_t binding) {
    return uniformBuffers[0][binding];
}

std::shared_ptr<SamplerDataInfo> DescriptorSetData::getSamplerByBinding(uint32_t binding, uint32_t instance) {
    return samplers[instance][binding];
}

std::shared_ptr<UniformBuffer> DescriptorSetData::getUniformBufferByBinding(uint32_t binding, uint32_t instance) {
    return uniformBuffers[instance][binding];
}

std::shared_ptr<DescriptorSet> & DescriptorSetData::getDescriptorSet() {
    return descriptorSet;
}

void DescriptorSetData::setInstanceCount(uint32_t instanceCount) {
    DescriptorSetData::instanceCount = instanceCount;
    DescriptorSetData::samplers.resize(instanceCount);
    DescriptorSetData::uniformBuffers.resize(instanceCount);
}
