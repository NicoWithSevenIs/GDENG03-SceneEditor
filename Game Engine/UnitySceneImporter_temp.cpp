#include "Game Engine/UnitySceneImporter.h"
#include "ECS/Systems/EntityManager.h"
#include "ECS/Entities/Entity.h"
#include "ECS/Components/CubeRenderer.h"
#include "ECS/Components/QuadRenderer.h"
#include "ECS/Components/SphereRenderer.h"
#include "ECS/Components/CylinderRenderer.h"
#include "ECS/Components/MeshRenderer.h"
#include "ECS/Components/PhysicsComponent.h"
#include "Math/Vector3D.h"
#include "Math/Transform.h"
#include <json/json.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <unordered_map>
#include <string>

using namespace DX3D;

// Helper struct for holding parsed GameObject data
struct UnityGameObjectData {
    std::string name;
    int fileID = 0;
    int parentFileID = 0;
    Vector3D position, rotation, scale;
    std::string type;
};

// Legacy function: Very basic YAML parser for Unity .unity files (only GameObject/Transform hierarchy)
bool UnitySceneImporter::ImportUnityScene(const std::string& unityFilePath) {
    std::ifstream file(unityFilePath);
    if (!file.is_open()) {
        std::cerr << "[UnitySceneImporter] Failed to open file: " << unityFilePath << std::endl;
        return false;
    }

    std::string line;
    std::unordered_map<int, UnityGameObjectData> objects;
    std::unordered_map<int, int> childToParent;
    int currentFileID = 0;
    UnityGameObjectData* currentObj = nullptr;

    // Pass 1: Parse GameObjects and Transforms
    while (std::getline(file, line)) {
        // Find GameObject blocks
        std::smatch match;
        if (std::regex_search(line, match, std::regex("!u!1 &([0-9]+)"))) {
            currentFileID = std::stoi(match[1]);
            currentObj = &objects[currentFileID];
            currentObj->fileID = currentFileID;
            continue;
        }
        // Name
        if (currentObj && line.find("m_Name:") != std::string::npos) {
            size_t pos = line.find(":");
            if (pos != std::string::npos) currentObj->name = line.substr(pos + 1);
            currentObj->name.erase(0, currentObj->name.find_first_not_of(" \t"));
        }
        // Parent (Transform block)
        if (std::regex_search(line, match, std::regex("m_Father: \\{fileID: ([0-9]+)"))) {
            int parentID = std::stoi(match[1]);
            if (currentObj) currentObj->parentFileID = parentID;
            childToParent[currentFileID] = parentID;
        }
        // Position
        if (line.find("m_LocalPosition:") != std::string::npos) {
            float x = 0, y = 0, z = 0;
            std::getline(file, line); sscanf_s(line.c_str(), "  x: %f", &x);
            std::getline(file, line); sscanf_s(line.c_str(), "  y: %f", &y);
            std::getline(file, line); sscanf_s(line.c_str(), "  z: %f", &z);
            if (currentObj) currentObj->position = Vector3D(x, y, z);
        }
        // Rotation (Euler)
        if (line.find("m_LocalEulerAnglesHint:") != std::string::npos) {
            float x = 0, y = 0, z = 0;
            std::getline(file, line); sscanf_s(line.c_str(), "  x: %f", &x);
            std::getline(file, line); sscanf_s(line.c_str(), "  y: %f", &y);
            std::getline(file, line); sscanf_s(line.c_str(), "  z: %f", &z);
            if (currentObj) currentObj->rotation = Vector3D(x, y, z);
        }
        // Scale
        if (line.find("m_LocalScale:") != std::string::npos) {
            float x = 1, y = 1, z = 1;
            std::getline(file, line); sscanf_s(line.c_str(), "  x: %f", &x);
            std::getline(file, line); sscanf_s(line.c_str(), "  y: %f", &y);
            std::getline(file, line); sscanf_s(line.c_str(), "  z: %f", &z);
            if (currentObj) currentObj->scale = Vector3D(x, y, z);
        }
    }
    file.close();

    // Pass 2: Create entities
    std::unordered_map<int, Entity*> idToEntity;
    for (auto& [fileID, data] : objects) {
        auto* e = new Entity(data.name);
        e->m_transform.m_translation = data.position;
        e->m_transform.m_rotation = data.rotation;
        e->m_transform.m_scale = data.scale;
        // TODO: Add component mapping if needed
        EntityManager::AddObject(e);
        idToEntity[fileID] = e;
    }
    // Pass 3: Set up parent-child relationships
    for (auto& [childID, parentID] : childToParent) {
        if (parentID == 0) continue;
        auto childIt = idToEntity.find(childID);
        auto parentIt = idToEntity.find(parentID);
        if (childIt != idToEntity.end() && parentIt != idToEntity.end()) {
            // ParentingManager::get().SetParent(childIt->second, parentIt->second);
        }
    }
    std::cout << "[UnitySceneImporter] Imported " << objects.size() << " GameObjects from Unity scene." << std::endl;
    return true;
}

// New: Convert Unity .unity to engine .json scene file
bool UnitySceneImporter::ConvertUnitySceneToJson(const std::string& unityFilePath, const std::string& outJsonPath) {
    std::ifstream file(unityFilePath);
    if (!file.is_open()) {
        std::cerr << "[UnitySceneImporter] Failed to open file: " << unityFilePath << std::endl;
        return false;
    }

    std::string line;
    std::unordered_map<int, UnityGameObjectData> objects;
    std::unordered_map<int, int> childToParent;
    int currentFileID = 0;
    UnityGameObjectData* currentObj = nullptr;

    // Parse GameObjects and Transforms (same as before)
    while (std::getline(file, line)) {
        std::smatch match;
        if (std::regex_search(line, match, std::regex("!u!1 &([0-9]+)"))) {
            currentFileID = std::stoi(match[1]);
            currentObj = &objects[currentFileID];
            currentObj->fileID = currentFileID;
            continue;
        }
        if (currentObj && line.find("m_Name:") != std::string::npos) {
            size_t pos = line.find(":");
            if (pos != std::string::npos) currentObj->name = line.substr(pos + 1);
            currentObj->name.erase(0, currentObj->name.find_first_not_of(" \t"));
        }
        if (std::regex_search(line, match, std::regex("m_Father: \\{fileID: ([0-9]+)"))) {
            int parentID = std::stoi(match[1]);
            if (currentObj) currentObj->parentFileID = parentID;
            childToParent[currentFileID] = parentID;
        }
        if (line.find("m_LocalPosition:") != std::string::npos) {
            float x = 0, y = 0, z = 0;
            std::getline(file, line); sscanf_s(line.c_str(), "  x: %f", &x);
            std::getline(file, line); sscanf_s(line.c_str(), "  y: %f", &y);
            std::getline(file, line); sscanf_s(line.c_str(), "  z: %f", &z);
            if (currentObj) currentObj->position = Vector3D(x, y, z);
        }
        if (line.find("m_LocalEulerAnglesHint:") != std::string::npos) {
            float x = 0, y = 0, z = 0;
            std::getline(file, line); sscanf_s(line.c_str(), "  x: %f", &x);
            std::getline(file, line); sscanf_s(line.c_str(), "  y: %f", &y);
            std::getline(file, line); sscanf_s(line.c_str(), "  z: %f", &z);
            if (currentObj) currentObj->rotation = Vector3D(x, y, z);
        }
        if (line.find("m_LocalScale:") != std::string::npos) {
            float x = 1, y = 1, z = 1;
            std::getline(file, line); sscanf_s(line.c_str(), "  x: %f", &x);
            std::getline(file, line); sscanf_s(line.c_str(), "  y: %f", &y);
            std::getline(file, line); sscanf_s(line.c_str(), "  z: %f", &z);
            if (currentObj) currentObj->scale = Vector3D(x, y, z);
        }
    }
    file.close();

    // Compose engine scene JSON
    Json::Value root;
    root["scene_name"] = "ImportedUnityScene";
    root["version"] = "1.0";
    Json::Value entitiesArray(Json::arrayValue);
    for (const auto& [fileID, data] : objects) {
        Json::Value entityObj;
        entityObj["name"] = data.name;
        entityObj["enabled"] = true;
        // Transform
        Json::Value transformObj;
        transformObj["position"]["x"] = data.position.m_x;
        transformObj["position"]["y"] = data.position.m_y;
        transformObj["position"]["z"] = data.position.m_z;
        transformObj["rotation"]["x"] = data.rotation.m_x;
        transformObj["rotation"]["y"] = data.rotation.m_y;
        transformObj["rotation"]["z"] = data.rotation.m_z;
        transformObj["scale"]["x"] = data.scale.m_x;
        transformObj["scale"]["y"] = data.scale.m_y;
        transformObj["scale"]["z"] = data.scale.m_z;
        entityObj["transform"] = transformObj;
        // Parent
        if (data.parentFileID != 0 && objects.count(data.parentFileID))
            entityObj["parent"] = objects.at(data.parentFileID).name;
        else
            entityObj["parent"] = Json::nullValue;
        // Constant data (default values)
        Json::Value constantObj;
        constantObj["color"]["x"] = 1.0f;
        constantObj["color"]["y"] = 1.0f;
        constantObj["color"]["z"] = 1.0f;
        constantObj["angle"] = 0.0f;
        constantObj["isRandom"] = 0;
        constantObj["hasTex"] = 0;
        entityObj["constant"] = constantObj;
        // Components (empty for now)
        entityObj["components"] = Json::arrayValue;
        entitiesArray.append(entityObj);
    }
    root["entities"] = entitiesArray;

    // Write to .json file
    std::ofstream out(outJsonPath);
    if (!out.is_open()) {
        std::cerr << "[UnitySceneImporter] Failed to write json: " << outJsonPath << std::endl;
        return false;
    }
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "  ";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    writer->write(root, &out);
    out.close();
    std::cout << "[UnitySceneImporter] Converted Unity scene to: " << outJsonPath << std::endl;
    return true;
}
