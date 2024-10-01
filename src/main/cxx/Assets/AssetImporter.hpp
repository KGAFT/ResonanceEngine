//
// Created by kgaft on 3/3/24.
//

#ifndef ASSETIMPORTER_HPP
#define ASSETIMPORTER_HPP

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <VulkanLib/Device/Image/Image.hpp>
#include <vector>
#include <map>
#include "RenderData/Model.hpp"
#include "RenderData/RenderObjectsDataManager.hpp"
#include <stb_image.h>

using namespace std;

struct Texture {
    std::shared_ptr<Image> image;
    uint32_t id;
    bool init = false;
};

class AssetImporter {
private:
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
private:
public:
    AssetImporter(std::shared_ptr<LogicalDevice> device, bool rtSupport) : device(device), dataManager(
        std::make_shared<RenderObjectsDataManager>(device, rtSupport)) {

    }

private:
    Assimp::Importer importer;
    std::shared_ptr<RenderObjectsDataManager> dataManager;
    std::shared_ptr<LogicalDevice> device;
    std::string directory;
    std::map<std::string, Texture> loadedTextures;
    std::vector<float> vertexBuffer;
    std::vector<uint32_t> indexBuffer;
    std::vector<vk::DrawIndexedIndirectCommand> drawCommands;
    std::vector<std::shared_ptr<Model>> loadedModels;
    SeriesObject<vk::BufferCreateInfo> bufferCreateInfos;
    Texture notInited = { nullptr, 0, false };
public:
    std::shared_ptr<Model> loadModel(std::string path) {
        auto scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            throw std::runtime_error("Failed to load model");
            return std::shared_ptr<Model>();
        }
        directory = path.substr(0, path.find_last_of('/'));
        auto outModel = std::make_shared<Model>(scene->mName.C_Str());
        processNode(device, outModel, scene->mRootNode, scene);
        loadedModels.push_back(outModel);
        importer.FreeScene();
        return outModel;
    }

    std::shared_ptr<RenderObjectsDataManager> makeBatchData() {
        auto resVertexBuffer = std::make_shared<VertexBuffer>(device, vertexBuffer.data(), vertexBuffer.size() / 9,
            9 * sizeof(float), vk::Format::eR32G32B32Sfloat, false);
        auto resIndexBuffer = std::make_shared<IndexBuffer>(device, indexBuffer.data(), indexBuffer.size(),
            vk::IndexType::eUint32, false);
        auto indirectBuffer = std::make_shared<IndirectBuffer>(device, drawCommands.size(),
            sizeof(vk::DrawIndexedIndirectCommand));
        memcpy(indirectBuffer->getMapPoint(), drawCommands.data(),
            drawCommands.size() * sizeof(vk::DrawIndexedIndirectCommand));
        vertexBuffer.clear();
        indexBuffer.clear();
        drawCommands.clear();
        dataManager->setBatchData(resIndexBuffer, resVertexBuffer, indirectBuffer, loadedModels);
        dataManager->makeMaterialBuffer();
        return dataManager;
    }

    void processNode(std::shared_ptr<LogicalDevice> device, std::shared_ptr<Model>& outModel, aiNode* node,
        const aiScene* scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(device, outModel, mesh, scene);
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(device, outModel, node->mChildren[i], scene);
        }
    }

    void processMesh(std::shared_ptr<LogicalDevice> device, std::shared_ptr<Model>& outModel, aiMesh* mesh,
        const aiScene* scene) {

        Material material{};
        AssetImporter::loadTextures(device, scene, mesh, material);
        uint32_t materialId = dataManager->pushMaterial(material);
        drawCommands.push_back({});
        drawCommands[drawCommands.size() - 1].vertexOffset = vertexBuffer.size() / 9;
        drawCommands[drawCommands.size() - 1].firstIndex = indexBuffer.size();
        drawCommands[drawCommands.size() - 1].indexCount = 0;
        for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
            vertexBuffer.push_back(mesh->mVertices[i].x);
            vertexBuffer.push_back(mesh->mVertices[i].y);
            vertexBuffer.push_back(mesh->mVertices[i].z);
            glm::vec3 normals(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            normals = glm::normalize(normals);
            vertexBuffer.push_back(normals.x);
            vertexBuffer.push_back(normals.y);
            vertexBuffer.push_back(normals.z);

            vertexBuffer.push_back(mesh->mTextureCoords[0][i].x);
            vertexBuffer.push_back(mesh->mTextureCoords[0][i].y);

            vertexBuffer.push_back(materialId);
        }

        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++) {
                indexBuffer.push_back(face.mIndices[j]);
                drawCommands[drawCommands.size() - 1].indexCount++;
            }
        }
        drawCommands[drawCommands.size() - 1].instanceCount = 1;
        drawCommands[drawCommands.size() - 1].firstInstance = 0;
        auto out = std::make_shared<Mesh>(mesh->mName.C_Str(), materialId);
        out->indirectOffset = (drawCommands.size() - 1) * sizeof(vk::DrawIndexedIndirectCommand);
        outModel->meshes.push_back(out);
    }


    void loadTextures(std::shared_ptr<LogicalDevice> device, const aiScene* scene, aiMesh* mesh, Material& material) {


        Texture& currentTexture = notInited;
        aiString path;
        scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &path, nullptr, nullptr, nullptr,
            nullptr,
            nullptr);

        currentTexture = tryGetLoadedMaterial(path.C_Str());
        if (!currentTexture.init) {
            if (path.length)
                loadTexture(device, path.C_Str());
        }
        currentTexture = tryGetLoadedMaterial(path.C_Str());
        if (!currentTexture.init) {
            scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_BASE_COLOR, 0, &path, nullptr, nullptr,
                nullptr,
                nullptr,
                nullptr);
            currentTexture = tryGetLoadedMaterial(path.C_Str());
            if (!currentTexture.init) {
                if (path.length)
                    loadTexture(device, path.C_Str());
            }
        }


        material.albedoIndex = currentTexture.id;

        scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_NORMALS, 0, &path, nullptr, nullptr, nullptr,
            nullptr,
            nullptr);
        currentTexture = tryGetLoadedMaterial(path.C_Str());
        if (!currentTexture.init) {
            if (path.length)
                loadTexture(device, path.C_Str());
        }
        currentTexture = tryGetLoadedMaterial(path.C_Str());


        material.normalMapIndex = currentTexture.id;

        scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_EMISSIVE, 0, &path, nullptr, nullptr, nullptr,
            nullptr,
            nullptr);

        currentTexture = tryGetLoadedMaterial(path.C_Str());
        if (!currentTexture.init) {
            if (path.length)
                loadTexture(device, path.C_Str());
        }
        currentTexture = tryGetLoadedMaterial(path.C_Str());

        material.emissiveMapIndex = currentTexture.id;

        scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &path, nullptr, nullptr,
            nullptr,
            nullptr,
            nullptr);
        currentTexture = tryGetLoadedMaterial(path.C_Str());
        if (!currentTexture.init) {
            if (path.length)
                loadTexture(device, path.C_Str());
        }
        currentTexture = tryGetLoadedMaterial(path.C_Str());
        material.aoMapIndex = currentTexture.id;

        scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_METALNESS, 0, &path, nullptr, nullptr,
            nullptr, nullptr,
            nullptr);
        currentTexture = tryGetLoadedMaterial(path.C_Str());
        if (!currentTexture.init) {
            if (path.length)
                loadTexture(device, path.C_Str());
        }
        currentTexture = tryGetLoadedMaterial(path.C_Str());
        material.metallicMapIndex = currentTexture.id;
        material.roughnessMapIndex = currentTexture.id;

        scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_OPACITY, 0, &path, nullptr, nullptr, nullptr,
            nullptr,
            nullptr);
        currentTexture = tryGetLoadedMaterial(path.C_Str());
        if (!currentTexture.init) {
            if (path.length)
                loadTexture(device, path.C_Str());
        }
        currentTexture = tryGetLoadedMaterial(path.C_Str());
        material.opacityMapIndex = currentTexture.id;

    }


    Texture& tryGetLoadedMaterial(const char* path) {

        for (auto& item : loadedTextures) {
            if (!item.first.compare(path)) {
                return item.second;
            }
        }
        notInited.init = false;
        return notInited;
    }

    void loadTexture(std::shared_ptr<LogicalDevice> device, const char* path) {
        try {
            string fullPath = (directory + '/' + path);
            int imageWidth, imageHeight, imageChannels;
            stbi_uc* imageData = stbi_load(fullPath.c_str(), &imageWidth, &imageHeight, &imageChannels, STBI_rgb_alpha);

            uint32_t queueIndices[] = { device->getQueueByType(vk::QueueFlagBits::eGraphics)->getIndex() };

            vk::BufferCreateInfo* createInfo = bufferCreateInfos.getObjectInstance();
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

            bufferCreateInfos.releaseObjectInstance(createInfo);

            defaultColorViewCreateInfo.image = image->getBase();
            image->createImageView(defaultColorViewCreateInfo);
            stageBuffer.destroy();
            stbi_image_free(imageData);
            image->transitionImageLayout(device, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral, vk::ImageAspectFlagBits::eColor);
            image->getImageInfo().initialLayout = vk::ImageLayout::eGeneral;
            loadedTextures.insert(
                std::pair<string, Texture>(string(path), { image, dataManager->pushImage(image), true }));
        }
        catch (std::runtime_error& ex) {
        }
    }

};


#endif //MENGINE_ASSETIMPORTER_HXX