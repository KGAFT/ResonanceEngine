//
// Created by kgaft on 8/30/24.
//

#ifndef MODEL_HPP
#define MODEL_HPP
#include <memory>
#include <string>
#include <vector>

#include "Mesh.hpp"


class Model {
    friend class AssetImporter;

public:
    Model(const std::string &name) : name(name) {}

private:
    std::string name;

    std::vector<std::shared_ptr<Mesh>> meshes;
public:
    const std::string &getName() const {
        return name;
    }

    const std::vector<std::shared_ptr<Mesh>> &getMeshes() const {
        return meshes;
    }
};



#endif //MODEL_HPP
