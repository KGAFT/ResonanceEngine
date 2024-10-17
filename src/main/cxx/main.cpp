#include <iostream>

#include "Pipelines/Standart/OutputPipeline.hpp"
#include "Pipelines/PipelineManager.hpp"
#include "Pipelines/Standart/PBRPipeline.h"
#include "Settings/SettingsManager.hxx"
#include "Settings/SettingsResizeCallback.hxx"
#include "Vulkan/VulkanContext.hpp"
#include "VulkanLib/Device/DeviceBuilder.hpp"
#include "Window/Window.hxx"
#include "Verbose/EngineLogger.hxx"
#include "Verbose/DevicePicker/DevicePickerConsole.hxx"
#include "VulkanLib/Shader/ShaderLoader.hpp"
#include <Pipelines/Standart/GBufferPipeline.hpp>
#include <Assets/AssetImporter.hpp>
#include <memory>

#include "Camera/CameraManager.hxx"
#include "VulkanLib/RenderPipeline/OmniRenderingGraphicsPipeline.hpp"

struct TPCS {
    glm::mat4 viewProjMatrix;
};

static inline vk::ImageCreateInfo defaultColorCreateInfo = { vk::ImageCreateFlags(),
                                                                vk::ImageType::e2D, vk::Format::eR8G8B8A8Srgb,
                                                                vk::Extent3D{800, 600, 1},
                                                                1, 1, vk::SampleCountFlagBits::e1,
                                                                vk::ImageTiling::eOptimal,
                                                                vk::ImageUsageFlagBits::eSampled |
                                                                vk::ImageUsageFlagBits::eTransferDst |
                                                                vk::ImageUsageFlagBits::eTransferSrc,
                                                                vk::SharingMode::eExclusive,
                                                                0, nullptr,
                                                                vk::ImageLayout::eUndefined, nullptr

    };
static inline vk::ImageViewCreateInfo defaultColorViewCreateInfo = { vk::ImageViewCreateFlags(),
                                                                    nullptr, vk::ImageViewType::e2D,
                                                                    vk::Format::eR8G8B8A8Srgb, {},
                                                                    vk::ImageSubresourceRange{
                                                                        vk::ImageAspectFlagBits::eColor, 0, 1,
                                                                        0, 1} };

static inline float quadVerticesXPos[]
{
    0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, 0.5f
};
static inline uint32_t quadIndicesXPos[]
{
    3, 2, 1, 0, 3, 1
};

static inline float quadVerticesXNeg[]{
    -0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f
};
static inline uint32_t quadIndicesXNeg[]{
    0, 1, 2, 3, 0, 2
};
static inline float quadVerticesYPos[]{
    -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, 0.5f
};
static inline uint32_t quadIndicesYPos[]{
    3, 0, 2, 1, 3, 2
};
static inline float quadVerticesYNeg[]
{
    -0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, 0.5f
};
static inline uint32_t quadIndicesYNeg[]
{
    3, 1, 2, 0, 3, 2
};
static inline float quadVerticesZPos[]
{
    -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f
};
static inline uint32_t quadIndicesZPos[]
{
    0, 1, 2, 3, 0, 2
};
static inline float quadVerticesZNeg[]
{
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, -0.5f
};
static inline uint32_t quadIndicesZNeg[]{
    1, 3, 2, 0, 1, 2
};

std::shared_ptr<Image> loadTexture(std::shared_ptr<LogicalDevice> device, std::string path) {
    try {
            string fullPath = (path);
            int imageWidth, imageHeight, imageChannels;
            stbi_uc* imageData = stbi_load(fullPath.c_str(), &imageWidth, &imageHeight, &imageChannels, STBI_rgb_alpha);

            uint32_t queueIndices[] = { device->getQueueByType(vk::QueueFlagBits::eGraphics)->getIndex() };

            vk::BufferCreateInfo* createInfo = new vk::BufferCreateInfo();
            createInfo->sType = vk::StructureType::eBufferCreateInfo;
            createInfo->size = imageWidth * imageHeight * 4;
            createInfo->usage = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo->sharingMode = vk::SharingMode::eExclusive;
            createInfo->pQueueFamilyIndices = queueIndices;
            createInfo->queueFamilyIndexCount = 1;

            void* mapPoint;
            Buffer stageBuffer(device, createInfo, vk::MemoryPropertyFlagBits::eHostVisible);
            stageBuffer.map(&mapPoint, 0, vk::MemoryMapFlags());
            memcpy(mapPoint, imageData, imageWidth * imageHeight * 4);
            stageBuffer.unMap();


            defaultColorCreateInfo.extent = vk::Extent3D{ static_cast<uint32_t>(imageWidth),
                                                         static_cast<uint32_t>(imageHeight), 1 };
            auto image = std::make_shared<Image>(device, defaultColorCreateInfo);
            image->copyFromBuffer(stageBuffer, 1, *device->getQueueByType(vk::QueueFlagBits::eGraphics));

            delete createInfo;

            defaultColorViewCreateInfo.image = image->getBase();
            image->createImageView(defaultColorViewCreateInfo);
            stageBuffer.destroy();
            stbi_image_free(imageData);
            image->transitionImageLayout(device, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral, vk::ImageAspectFlagBits::eColor);
            image->getImageInfo().initialLayout = vk::ImageLayout::eGeneral;
            return image;
        }
        catch (std::runtime_error& ex) {
        }
    return nullptr;
}

void testCube() {
    auto renderBuilder = std::make_shared<RenderPipelineBuilder>();
    renderBuilder->addSamplerInfo({0, 1, vk::ShaderStageFlagBits::eFragment});
    renderBuilder->addVertexInput({0, 3, sizeof(float), vk::Format::eR32G32B32Sfloat});
    renderBuilder->addPushConstantInfo({vk::ShaderStageFlagBits::eVertex, sizeof(TPCS)});
    std::vector<ShaderCreateInfo> createInfos;
    createInfos.push_back({"shaders/CubemapGen/cubeside.vert", "cubeside.vert", SRC_FILE, vk::ShaderStageFlagBits::eVertex});
    createInfos.push_back({"shaders/CubemapGen/HDRToCube.frag", "HDRToCube.frag", SRC_FILE, vk::ShaderStageFlagBits::eFragment});

    auto shader = ShaderLoader::getInstance()->createShader(*VulkanContext::getDevice(), createInfos);

    OmniRenderingGraphicsPipeline omnPip(VulkanContext::getVulkanInstance(), VulkanContext::getDevice(), renderBuilder.get(), shader, vk::Extent2D(4096, 4096));
    auto dsc = DescriptorPool::getInstance(VulkanContext::getDevice(), false)->allocateDescriptorSet(1, omnPip.getGraphicsPipeline()->getDescriptorLayout());

    DescriptorSetData dscData(dsc);
    dscData.createSampler(VulkanContext::getDevice(), 0);
    PushConstant pcs(sizeof(TPCS), omnPip.getGraphicsPipeline()->getPipelineLayout());

    auto texture = loadTexture(VulkanContext::getDevice(), "assets/textures/cubetest.hdr");
    dscData.getSamplerByBinding(0)->imageView = texture->getImageViews()[0]->getBase();
    dscData.confirmAndWriteDataToDescriptorSet();
    auto q = VulkanContext::getDevice()->getQueueByType(vk::QueueFlagBits::eGraphics);
    auto cmd = q->beginSingleTimeCommands();
    omnPip.begin(cmd, 0);
    dscData.getDescriptorSet()->bindDescriptor(vk::PipelineBindPoint::eGraphics, 0, cmd, omnPip.getGraphicsPipeline()->getPipelineLayout());
    omnPip.endRender(cmd, 0);
    q->endSingleTimeCommands(cmd);
}

int main() {
    auto window = Window::createWindow(1920, 1080, "Hello world");
    SettingsManager::getInstance()->storeData<RESOLUTION_INFO_T>(RESOLUTION_INFO, glm::vec2(1920, 1080));
    SettingsManager::getInstance()->storeData<RENDER_SCALE_INFO_T>(RENDER_SCALE_INFO, 1.0f);
    window->addResizeCallback(new SettingsResizeCallback(SettingsManager::getInstance()));
    auto logger = EngineLogger::getLoggerInstance();
    DevicePickerConsole picker;
    uint32_t deviceCount;
    DeviceBuilder builder;
    builder.requestGraphicsSupport();
    //builder.requestRayTracingSupport();
    builder.requestPresentSupport(window->getWindowSurface(VulkanContext::getVulkanInstance(false).getInstance()));
    auto devices = VulkanContext::enumerateSupportedDevices(
        builder, window->getWindowSurface(VulkanContext::getVulkanInstance().getInstance()), &deviceCount);

    auto result = picker.pickDevice(devices, deviceCount);
    VulkanContext::pickDevice(builder, result,
                              window->getWindowSurface(VulkanContext::getVulkanInstance().getInstance()),
                              window->getWidth(), window->getHeight());

    AssetImporter importer(VulkanContext::getDevice(), false);
    auto model = importer.loadModel("assets/DamagedHelmet/glTF/DamagedHelmet.gltf");
    auto renderData = importer.makeBatchData();
    std::shared_ptr<OutputPipeline> pipeline = std::make_shared<OutputPipeline>(VulkanContext::getDevice());
    std::shared_ptr<GBufferPipeline> gPipeline = std::make_shared<GBufferPipeline>(VulkanContext::getDevice(), renderData);
    auto pbrPipeline = std::make_shared<PBRPipeline>(VulkanContext::getDevice(), gPipeline, 128, 128);


    pipeline->setMaxFramesInFlight(VulkanContext::getMaxFramesInFlight());

    PipelineManager pipelineManager;
    pipelineManager.addGraphicsPipeline(gPipeline);
    pipelineManager.addGraphicsPipeline(pbrPipeline);
    for(uint32_t i = 0; i < VulkanContext::getMaxFramesInFlight(); i++) {
        pipeline->addAttachment(pbrPipeline->getRenderPipeline()->getBaseRenderImages()[0]->getImageViews()[0]);
        pipeline->addAttachment(pbrPipeline->getRenderPipeline()->getBaseRenderImages()[0]->getImageViews()[0]);
        pipeline->addAttachment(pbrPipeline->getRenderPipeline()->getBaseRenderImages()[0]->getImageViews()[0]);
    }
    gPipeline->setupGlobalDescriptor(renderData);
    pipelineManager.setPresentationPipeline(pipeline);
    window->addResizeCallback(VulkanContext::getSyncManager().get());
    window->enableRefreshRateInfo();
    window->getInputSystem().registerKeyCallback(pipeline.get());
    pbrPipeline->getLightConfiguration()->enabledPoints = 1;
    pbrPipeline->getLightConfiguration()->enabledDirects = 0;
    pbrPipeline->getLightConfiguration()->ambientIntensity = 0.03;
    pbrPipeline->getPointLightBlock(0)->position = glm::vec3(2, 3, 2);
    pbrPipeline->getPointLightBlock(0)->color = glm::vec3(1,0,0);
    pbrPipeline->getPointLightBlock(0)->intensity = 100;
    CameraManager cameraManager(window);
    cameraManager.getCurrentCamera()->getPosition(pbrPipeline->getLightConfiguration()->cameraPosition);\
    while (!window->needToClose()) {
        window->preRenderEvents();
        glm::mat4 camMatrix = cameraManager.getCurrentCamera()->calculateCameraMatrix( 75, 0.05f, 2600000, (float)window->getWidth()/(float)window->getHeight());
        glm::vec3 camPos;
        cameraManager.getCurrentCamera()->getPosition(camPos);

        gPipeline->setCameraMatrix(camMatrix, camPos);
        cameraManager.getCurrentCamera()->getPosition(pbrPipeline->getLightConfiguration()->cameraPosition);
        pipelineManager.draw(renderData, model);
        
        window->postRenderEvents();
    }
    gPipeline->destroy();
    pbrPipeline->destroy();
    pipeline->destroy();
    VulkanContext::shutDown();
    return 0;
}
