#pragma once

#include "IPipelineGraphics.hxx"

struct OutputPipelineConfig {
    int enableInput[3] = {0,0,0};
    int dep;
};

class PresentationPipeline : public IPipelineGraphics {
public:
  void
  populateBuilder(std::shared_ptr<RenderPipelineBuilder> builder) override {
    builder->addVertexInput(
        {0, 3, sizeof(float), vk::Format::eR32G32B32Sfloat});
    builder->addVertexInput({1, 2, sizeof(float), vk::Format::eR32G32Sfloat});
    builder->addSamplerInfo({0, 1, vk::ShaderStageFlagBits::eFragment});
    builder->addSamplerInfo({1, 1, vk::ShaderStageFlagBits::eFragment});
    builder->addSamplerInfo({2, 1, vk::ShaderStageFlagBits::eFragment});
    builder->addPushConstantInfo(
        {vk::ShaderStageFlagBits::eFragment, sizeof(OutputPipelineConfig)});
    builder->setAttachmentsPerStepAmount(1);
  }
  std::shared_ptr<std::vector<ShaderCreateInfo>> getShadersInfos() {
    std::shared_ptr<std::vector<ShaderCreateInfo>> res =
        std::make_shared<std::vector<ShaderCreateInfo>>();
    res->push_back({"shaders/OutputPipeline/main.vert", "main.vert", SRC_FILE,
                    vk::ShaderStageFlagBits::eVertex});
    res->push_back({"shaders/OutputPipeline/main.frag", "main.frag", SRC_FILE,
                    vk::ShaderStageFlagBits::eFragment});
    return res;
  }
};