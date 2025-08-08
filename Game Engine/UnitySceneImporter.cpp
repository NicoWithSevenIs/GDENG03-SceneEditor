#include "Game Engine/UnitySceneImporter.h"
#include "ECS/Systems/EntityManager.h"
#include "ECS/Entities/Entity.h"
#include "ECS/Components/CubeRenderer.h"
#include "Math/Vector3D.h"
#include <json/json.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <unordered_map>
#include <functional>
#include <string>

using namespace DX3D;

// Helper struct for holding parsed GameObject data
struct UnityGameObjectData {
    std::string name;
    int fileID = 0;
    int parentFileID = 0;
    Vector3D position, rotation, scale;
    
    UnityGameObjectData() : scale(1.0f, 1.0f, 1.0f) {}
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
    std::unordered_map<int, int> transformToGameObject; // Map Transform fileID to GameObject fileID
    std::unordered_map<int, bool> isImportedSceneChild;
    int currentFileID = 0;
    UnityGameObjectData* currentObj = nullptr;
    int importedSceneRootID = -1;
    std::string currentComponentType = "";

    std::cout << "[UnitySceneImporter] Starting Unity scene import from: " << unityFilePath << std::endl;

    // First pass - collect all GameObjects and their basic info
    while (std::getline(file, line)) {
        std::smatch match;
        
        // Check for any Unity component using safer parsing
        if (line.find("--- !u!") == 0) {
            // Extract component type and fileID manually for safety
            size_t typeStart = line.find("!u!") + 3;
            size_t typeEnd = line.find(" ", typeStart);
            size_t idStart = line.find("&") + 1;
            
            if (typeStart != std::string::npos && typeEnd != std::string::npos && idStart != std::string::npos && idStart > 1) {
                try {
                    std::string typeStr = line.substr(typeStart, typeEnd - typeStart);
                    std::string idStr = line.substr(idStart);
                    
                    // Remove any trailing whitespace/newlines from idStr
                    idStr.erase(idStr.find_last_not_of(" \t\r\n") + 1);
                    
                    if (!typeStr.empty() && !idStr.empty()) {
                        int componentType = std::stoi(typeStr);
                        currentFileID = std::stoi(idStr);
                        
                        if (componentType == 1) { // GameObject
                            currentObj = &objects[currentFileID];
                            currentObj->fileID = currentFileID;
                            currentComponentType = "GameObject";
                            std::cout << "[UnitySceneImporter] Found GameObject with ID: " << currentFileID << std::endl;
                        } else if (componentType == 4) { // Transform
                            currentComponentType = "Transform";
                            // Find which GameObject this transform belongs to
                            for (auto& pair : objects) {
                                // Simple heuristic: the GameObject usually comes right before its Transform
                                if (pair.second.fileID < currentFileID && pair.second.fileID > currentFileID - 50) {
                                    transformToGameObject[currentFileID] = pair.second.fileID;
                                    currentObj = &pair.second;
                                    std::cout << "[UnitySceneImporter] Linking Transform " << currentFileID << " to GameObject " << pair.second.fileID << std::endl;
                                    break;
                                }
                            }
                        } else {
                            currentComponentType = "Other";
                            currentObj = nullptr;
                        }
                    }
                } catch (const std::exception& e) {
                    std::cout << "[UnitySceneImporter] Error parsing component line: " << line << " - " << e.what() << std::endl;
                    currentObj = nullptr;
                    currentComponentType = "Other";
                }
            }
            continue;
        }
        
        // Parse GameObject name
        if (currentObj && currentComponentType == "GameObject" && line.find("  m_Name: ") == 0) {
            std::string nameStr = line.substr(9);
            nameStr.erase(0, nameStr.find_first_not_of(" \t"));
            nameStr.erase(nameStr.find_last_not_of(" \t\r\n") + 1);
            currentObj->name = nameStr;
            
            std::cout << "[UnitySceneImporter] GameObject " << currentObj->fileID << " name: '" << currentObj->name << "'" << std::endl;
            
            // Check if this is an "Imported_Scene_" object
            if (currentObj->name.find("Imported_Scene_") == 0) {
                importedSceneRootID = currentObj->fileID;
                isImportedSceneChild[currentObj->fileID] = true;
                std::cout << "[UnitySceneImporter] Found imported scene root: " << currentObj->name << " (ID: " << currentObj->fileID << ")" << std::endl;
            }
        }
        
        // Parse Transform parent relationship
        if (currentObj && currentComponentType == "Transform" && line.find("  m_Father: {fileID: ") != std::string::npos) {
            size_t idStart = line.find("fileID: ") + 8;
            size_t idEnd = line.find("}", idStart);
            
            if (idStart != std::string::npos && idEnd != std::string::npos && idStart < idEnd) {
                try {
                    std::string parentIdStr = line.substr(idStart, idEnd - idStart);
                    int parentTransformID = std::stoi(parentIdStr);
                    
                    if (parentTransformID != 0 && transformToGameObject.find(parentTransformID) != transformToGameObject.end()) {
                        int parentGameObjectID = transformToGameObject[parentTransformID];
                        currentObj->parentFileID = parentGameObjectID;
                        childToParent[currentObj->fileID] = parentGameObjectID;
                        std::cout << "[UnitySceneImporter] GameObject " << currentObj->fileID << " parent: " << parentGameObjectID << std::endl;
                    }
                } catch (const std::exception& e) {
                    std::cout << "[UnitySceneImporter] Error parsing parent relationship: " << line << std::endl;
                }
            }
        }
        
        // Parse Transform position
        if (currentObj && currentComponentType == "Transform" && line.find("  m_LocalPosition:") != std::string::npos) {
            float x = 0, y = 0, z = 0;
            std::getline(file, line); 
            if (line.find("    x: ") != std::string::npos) sscanf_s(line.c_str(), "    x: %f", &x);
            std::getline(file, line); 
            if (line.find("    y: ") != std::string::npos) sscanf_s(line.c_str(), "    y: %f", &y);
            std::getline(file, line); 
            if (line.find("    z: ") != std::string::npos) sscanf_s(line.c_str(), "    z: %f", &z);
            currentObj->position = Vector3D(x, y, z);
        }
        
        // Parse Transform rotation
        if (currentObj && currentComponentType == "Transform" && line.find("  m_LocalEulerAnglesHint:") != std::string::npos) {
            float x = 0, y = 0, z = 0;
            std::getline(file, line); 
            if (line.find("    x: ") != std::string::npos) sscanf_s(line.c_str(), "    x: %f", &x);
            std::getline(file, line); 
            if (line.find("    y: ") != std::string::npos) sscanf_s(line.c_str(), "    y: %f", &y);
            std::getline(file, line); 
            if (line.find("    z: ") != std::string::npos) sscanf_s(line.c_str(), "    z: %f", &z);
            currentObj->rotation = Vector3D(x, y, z);
        }
        
        // Parse Transform scale
        if (currentObj && currentComponentType == "Transform" && line.find("  m_LocalScale:") != std::string::npos) {
            float x = 1, y = 1, z = 1;
            std::getline(file, line); 
            if (line.find("    x: ") != std::string::npos) sscanf_s(line.c_str(), "    x: %f", &x);
            std::getline(file, line); 
            if (line.find("    y: ") != std::string::npos) sscanf_s(line.c_str(), "    y: %f", &y);
            std::getline(file, line); 
            if (line.find("    z: ") != std::string::npos) sscanf_s(line.c_str(), "    z: %f", &z);
            currentObj->scale = Vector3D(x, y, z);
        }
    }
    file.close();

    std::cout << "[UnitySceneImporter] Parsing complete. Found " << objects.size() << " total GameObjects." << std::endl;

    // Mark all children of the imported scene root recursively
    if (importedSceneRootID != -1) {
        std::cout << "[UnitySceneImporter] Processing imported scene hierarchy..." << std::endl;
        
        std::function<void(int, int)> markChildren = [&](int parentID, int depth) {
            std::string indent(depth * 2, ' ');
            for (const auto& [childID, parentIDInMap] : childToParent) {
                if (parentIDInMap == parentID && !isImportedSceneChild[childID]) {
                    isImportedSceneChild[childID] = true;
                    std::cout << "[UnitySceneImporter] " << indent << "Marking child object: " << objects[childID].name << " (ID: " << childID << ")" << std::endl;
                    markChildren(childID, depth + 1);
                }
            }
        };
        markChildren(importedSceneRootID, 1);
    } else {
        // If no imported scene root found, include all objects for debugging
        std::cout << "[UnitySceneImporter] No 'Imported_Scene_' root found. Available objects:" << std::endl;
        for (const auto& [fileID, data] : objects) {
            std::cout << "[UnitySceneImporter] - GameObject ID " << fileID << ": '" << data.name << "'" << std::endl;
            if (data.name.find("Imported_Scene") != std::string::npos) {
                std::cout << "[UnitySceneImporter] Found potential match: " << data.name << std::endl;
                importedSceneRootID = fileID;
                isImportedSceneChild[fileID] = true;
                // Re-run the marking
                std::function<void(int, int)> markChildren = [&](int parentID, int depth) {
                    for (const auto& [childID, parentIDInMap] : childToParent) {
                        if (parentIDInMap == parentID && !isImportedSceneChild[childID]) {
                            isImportedSceneChild[childID] = true;
                            markChildren(childID, depth + 1);
                        }
                    }
                };
                markChildren(importedSceneRootID, 1);
                break;
            }
        }
    }

    // Create JSON output
    Json::Value root;
    
    // Extract scene name
    std::string sceneName = "ImportedUnityScene";
    if (importedSceneRootID != -1 && objects.count(importedSceneRootID)) {
        const std::string& rootName = objects[importedSceneRootID].name;
        if (rootName.find("Imported_Scene_") == 0) {
            sceneName = rootName.substr(15);
        } else if (rootName.find("Imported_Scene") != std::string::npos) {
            sceneName = rootName.substr(rootName.find("Imported_Scene") + 14);
        }
        std::cout << "[UnitySceneImporter] Extracted scene name: " << sceneName << std::endl;
    }
    
    root["scene_name"] = sceneName;
    root["version"] = "1.0";
    Json::Value entitiesArray(Json::arrayValue);
    
    int exportedCount = 0;
    for (const auto& [fileID, data] : objects) {
        // Skip if not part of imported scene hierarchy
        if (importedSceneRootID != -1 && !isImportedSceneChild[fileID]) {
            continue;
        }
        
        // Skip the root container itself
        if (fileID == importedSceneRootID) {
            continue;
        }
        
        // Skip objects with empty names
        if (data.name.empty()) {
            continue;
        }
        
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
        if (data.parentFileID != 0 && objects.count(data.parentFileID) && data.parentFileID != importedSceneRootID) {
            entityObj["parent"] = objects.at(data.parentFileID).name;
        } else {
            entityObj["parent"] = Json::nullValue;
        }
        
        // Constant data
        Json::Value constantObj;
        constantObj["color"]["x"] = 1.0f;
        constantObj["color"]["y"] = 1.0f;
        constantObj["color"]["z"] = 1.0f;
        constantObj["angle"] = 0.0f;
        constantObj["isRandom"] = 0;
        constantObj["hasTex"] = 0;
        entityObj["constant"] = constantObj;
        
        // Components
        Json::Value componentsArray(Json::arrayValue);
        Json::Value cubeRenderer;
        cubeRenderer["type"] = "CubeRenderer";
        cubeRenderer["isTextured"] = true;
        componentsArray.append(cubeRenderer);
        entityObj["components"] = componentsArray;
        
        entitiesArray.append(entityObj);
        exportedCount++;
        
        std::cout << "[UnitySceneImporter] Exported object: " << data.name << std::endl;
    }
    
    root["entities"] = entitiesArray;

    // Write to JSON file
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
    
    std::cout << "[UnitySceneImporter] Successfully converted Unity scene to: " << outJsonPath << std::endl;
    std::cout << "[UnitySceneImporter] Exported " << exportedCount << " objects" << std::endl;
    return true;
}
