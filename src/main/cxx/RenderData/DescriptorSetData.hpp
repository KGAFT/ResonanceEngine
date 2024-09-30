//
// Created by kgaft on 3/13/24.
//

#ifndef DESCRIPTORSETDATA_HPP
#define DESCRIPTORSETDATA_HPP


#include <cstdint>
#include <memory>
#include "VulkanLib/Device/Buffer/StorageBuffer.hpp"
#include "VulkanLib/Device/Descriptors/DescriptorSet.hpp"
#include "VulkanLib/Device/Buffer/UniformBuffer.hpp"
#include "VulkanLib/Device/Image/Sampler.hpp"
#include "VulkanLib/Device/Image/Image.hpp"

struct SamplerDataInfo {
    std::shared_ptr<Sampler> sampler;
    vk::ImageView imageView;
    vk::ImageLayout layout;
};

class DescriptorSetData : public IDestroyableObject {
public:
    explicit DescriptorSetData(std::shared_ptr<DescriptorSet> descriptorSet) : descriptorSet(descriptorSet) {
    }

private:
    std::shared_ptr<DescriptorSet> descriptorSet;
    std::vector<std::map<uint32_t, std::shared_ptr<UniformBuffer> > > uniformBuffers;
    std::vector<std::map<uint32_t, std::shared_ptr<SamplerDataInfo> > > samplers;
    std::vector<std::map<uint32_t, std::shared_ptr<StorageBuffer>>> storageBuffers;
    uint32_t instanceCount = 1;

public:
    void destroy() override;


    void createUniformBuffer(std::shared_ptr<LogicalDevice> device, uint32_t binding, size_t size);

    void createUniformBuffer(std::shared_ptr<LogicalDevice> device, uint32_t binding, size_t size, uint32_t instance);

    void createSampler(std::shared_ptr<LogicalDevice> device, uint32_t binding);

    void createSampler(std::shared_ptr<LogicalDevice> device, uint32_t binding, uint32_t instance);

    void setStorageBuffer(std::shared_ptr<StorageBuffer> storageBuffer, uint32_t binding, uint32_t instance);

    void setStorageBuffer(std::shared_ptr<StorageBuffer> storageBuffer, uint32_t binding);
    
    void confirmAndWriteDataToDescriptorSet() const;

    std::shared_ptr<SamplerDataInfo> getSamplerByBinding(uint32_t binding);

    std::shared_ptr<UniformBuffer> getUniformBufferByBinding(uint32_t binding);

    std::shared_ptr<SamplerDataInfo> getSamplerByBinding(uint32_t binding, uint32_t instance);

    std::shared_ptr<UniformBuffer> getUniformBufferByBinding(uint32_t binding, uint32_t instance);

    std::shared_ptr<DescriptorSet> &getDescriptorSet();

    void setInstanceCount(uint32_t instanceCount);
};


#endif //MENGINE_DESCRIPTORSETDATA_HPP
