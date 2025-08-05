#include "Scene Saving/Scene.hpp"
#include "GameObject/GameObject.h"
#include "GameObject/Cube.h"
#include "GameObject/Quad.h"
#include "GameObject/GameObjectManager.h"
#include "GameObject/ParentingManager.h"
#include "ECS/Entities/Entity.h"
#include "ECS/Components/Component.h"
#include "ECS/Components/CubeRenderer.h"
#include "ECS/Systems/EntityManager.h"
#include "Math/Vector3D.h"
#include "Math/Transform.h"
#include <iostream>
#include <algorithm>
#include <typeinfo>

using namespace DX3D;

Scene::Scene() : m_sceneName("Untitled Scene") {}

Scene::Scene(const std::string& name) : m_sceneName(name) {}

Scene::~Scene() {
    Clear();
}


void Scene::CaptureCurrentScene() {
    Clear();

    // Capture GameObjects
    std::vector<GameObject*> gameObjects = GameObjectManager::get_all();
    std::cout << "Captured " << gameObjects.size() << " GameObjects" << std::endl;

    // Capture ECS Entities
    std::vector<Entity*> entities = EntityManager::get_all();
    for (Entity* entity : entities) {
        m_entities.push_back(entity);
    }
    std::cout << "Captured " << entities.size() << " ECS Entities" << std::endl;
}

void Scene::LoadIntoScene() {
    std::vector<GameObject*> loadedObjects = GameObjectManager::get_all();
    std::vector<Entity*> loadedEntities = EntityManager::get_all();
    std::cout << "Scene loaded with " << loadedObjects.size() << " GameObjects and " 
              << loadedEntities.size() << " ECS Entities" << std::endl;
}

void Scene::Clear() {
    m_gameObjects.clear();
    m_entities.clear();
}

Json::Value Scene::SerializeToJson() {
    Json::Value root;
    root["scene_name"] = m_sceneName;
    root["version"] = "1.0";

    // Serialize GameObjects
    Json::Value gameObjectsArray(Json::arrayValue);
    std::vector<GameObject*> currentObjects = GameObjectManager::get_all();
    for (GameObject* obj : currentObjects) {
        gameObjectsArray.append(SerializeGameObject(obj));
    }
    root["gameobjects"] = gameObjectsArray;

    // Serialize ECS Entities
    Json::Value entitiesArray(Json::arrayValue);
    std::vector<Entity*> currentEntities = EntityManager::get_all();
    for (Entity* entity : currentEntities) {
        entitiesArray.append(SerializeEntity(entity));
    }
    root["entities"] = entitiesArray;

    return root;
}

void Scene::DeserializeFromJson(const Json::Value& json) {
    Clear();

    if (json.isMember("scene_name")) {
        m_sceneName = json["scene_name"].asString();
    }

    if (json.isMember("gameobjects")) {
        const Json::Value& gameObjects = json["gameobjects"];
        // First pass: Create all objects
        for (const Json::Value& objJson : gameObjects) {
            DeserializeGameObject(objJson);
        }
        
        // Second pass: Restore parent relationships
        for (const Json::Value& objJson : gameObjects) {
            RestoreParentRelationship(objJson);
        }
    }

    if (json.isMember("entities")) {
        const Json::Value& entities = json["entities"];
        // First pass: Create all entities
        for (const Json::Value& entityJson : entities) {
            DeserializeEntity(entityJson);
        }
        
        // Second pass: Restore entity parent relationships
        for (const Json::Value& entityJson : entities) {
            RestoreEntityParentRelationship(entityJson);
        }
    }
}

Json::Value Scene::SerializeGameObject(GameObject* gameObject) {
    Json::Value obj;
    obj["name"] = gameObject->m_name;
    obj["transform"] = SerializeTransform(gameObject->m_transform);
    obj["color"] = SerializeVector3D(gameObject->m_color);
    obj["isRainbow"] = gameObject->isRainbow;

    GameObject* parent = static_cast<GameObject*>(ParentingManager::get().GetParent(gameObject));
    if (parent != nullptr) {
        obj["parent"] = parent->m_name;
    }
    else {
        obj["parent"] = Json::nullValue;
    }

    if (dynamic_cast<Cube*>(gameObject)) {
        obj["type"] = "Cube";
    }
    else if (dynamic_cast<Quad*>(gameObject)) {
        obj["type"] = "Quad";
    }
    else {
        obj["type"] = "GameObject";
    }

    return obj;
}

Json::Value Scene::SerializeEntity(Entity* entity) {
    Json::Value obj;
    obj["name"] = entity->m_name;
    obj["enabled"] = entity->enabled;
    
    // Transform data
    obj["transform"] = SerializeTransform(entity->m_transform);
    
    // Parent information
    Entity* parent = static_cast<Entity*>(ParentingManager::get().GetParent(entity));
    if (parent != nullptr) {
        obj["parent"] = parent->m_name;
    } else {
        obj["parent"] = Json::nullValue;
    }

    // Components
    Json::Value componentsArray(Json::arrayValue);
    std::vector<Component*> components = entity->GetComponents();
    for (Component* component : components) {
        componentsArray.append(SerializeComponent(component));
    }
    obj["components"] = componentsArray;

    return obj;
}

Json::Value Scene::SerializeTransform(const Transform& transform) {
    Json::Value transformObj;
    transformObj["position"] = SerializeVector3D(transform.m_translation);
    transformObj["rotation"] = SerializeVector3D(transform.m_rotation);
    transformObj["scale"] = SerializeVector3D(transform.m_scale);
    return transformObj;
}

Json::Value Scene::SerializeVector3D(const Vector3D& vector) {
    Json::Value vecObj;
    vecObj["x"] = vector.m_x;
    vecObj["y"] = vector.m_y;
    vecObj["z"] = vector.m_z;
    return vecObj;
}

Json::Value Scene::SerializeComponent(Component* component) {
    Json::Value compObj;
    
    // Check component type and serialize accordingly
    if (CubeRenderer* cubeRenderer = dynamic_cast<CubeRenderer*>(component)) {
        compObj["type"] = "CubeRenderer";
        // Add any CubeRenderer-specific properties here
        // For now, just store the type as CubeRenderer may not have additional serializable data
    }
    // Add other component types as needed
    else {
        compObj["type"] = "Unknown";
    }
    
    return compObj;
}

void Scene::DeserializeGameObject(const Json::Value& json) {
    std::string name = json["name"].asString();
    std::string type = json.get("type", "GameObject").asString();

    if (type == "Cube") {
        GameObjectManager::MakeCube(name);
    }
    else if (type == "Quad") {
        GameObjectManager::MakeQuad(name);
    }
    else if (type == "GameObject") {
        if (name.find("Quad") != std::string::npos) {
            GameObjectManager::MakeQuad(name);
        } else {
            GameObjectManager::MakeCube(name);
        }
    }
    else {
        std::cout << "Unknown object type: " << type << ", defaulting to Cube" << std::endl;
        GameObjectManager::MakeCube(name);
    }

    std::vector<GameObject*> allObjects = GameObjectManager::get_all();
    GameObject* createdObject = nullptr;
    for (GameObject* obj : allObjects) {
        if (obj->m_name == name) {
            createdObject = obj;
            break;
        }
    }

    if (createdObject != nullptr) {
        if (json.isMember("transform")) {
            const Json::Value& transformJson = json["transform"];

            if (transformJson.isMember("position")) {
                createdObject->m_transform.m_translation = DeserializeVector3D(transformJson["position"]);
            }
            if (transformJson.isMember("rotation")) {
                createdObject->m_transform.m_rotation = DeserializeVector3D(transformJson["rotation"]);
            }
            if (transformJson.isMember("scale")) {
                createdObject->m_transform.m_scale = DeserializeVector3D(transformJson["scale"]);
            }
        }

        if (json.isMember("color")) {
            createdObject->m_color = DeserializeVector3D(json["color"]);
        }

        if (json.isMember("isRainbow")) {
            createdObject->isRainbow = json["isRainbow"].asBool();
        }

        std::cout << "Loaded GameObject: " << name << " (type: " << type << ")" << std::endl;
    }
    else {
        std::cerr << "Failed to find created object: " << name << std::endl;
    }
}

void Scene::DeserializeEntity(const Json::Value& json) {
    std::string name = json["name"].asString();
    Entity* entity = new Entity();
    entity->m_name = name;

    // Restore enabled state
    if (json.isMember("enabled")) {
        entity->enabled = json["enabled"].asBool();
    }

    if (json.isMember("transform")) {
        const Json::Value& transformJson = json["transform"];

        if (transformJson.isMember("position")) {
            entity->m_transform.m_translation = DeserializeVector3D(transformJson["position"]);
        }
        if (transformJson.isMember("rotation")) {
            entity->m_transform.m_rotation = DeserializeVector3D(transformJson["rotation"]);
        }
        if (transformJson.isMember("scale")) {
            entity->m_transform.m_scale = DeserializeVector3D(transformJson["scale"]);
        }
    }

    // Deserialize components
    if (json.isMember("components")) {
        const Json::Value& components = json["components"];
        for (const Json::Value& compJson : components) {
            DeserializeComponent(compJson, entity);
        }
    }

    // Register entity with EntityManager
    EntityManager::AddObject(entity);
    
    std::cout << "Loaded Entity: " << name << std::endl;
}

void Scene::RestoreParentRelationship(const Json::Value& json) {
    std::string childName = json["name"].asString();
    
    if (json.isMember("parent") && !json["parent"].isNull()) {
        std::string parentName = json["parent"].asString();
        
        // Find the child and parent objects in GameObjectManager
        std::vector<GameObject*> allObjects = GameObjectManager::get_all();
        GameObject* childObject = nullptr;
        GameObject* parentObject = nullptr;
        
        for (GameObject* obj : allObjects) {
            if (obj->m_name == childName) {
                childObject = obj;
            }
            if (obj->m_name == parentName) {
                parentObject = obj;
            }
        }
        
        // Establish the parent-child relationship if both objects exist
        if (childObject && parentObject) {
            ParentingManager::get().AddObject(childObject, parentObject);
            std::cout << "Restored parent relationship: " << childName << " -> " << parentName << std::endl;
        }
        else {
            std::cerr << "Failed to restore parent relationship for " << childName 
                      << " (parent: " << parentName << ")" << std::endl;
        }
    }
}

void Scene::DeserializeComponent(const Json::Value& json, Entity* entity) {
    if (!json.isMember("type")) {
        return;
    }
    
    std::string componentType = json["type"].asString();
    
    if (componentType == "CubeRenderer") {
        // Create and add CubeRenderer component
        entity->AddComponent<CubeRenderer>();
        std::cout << "Added CubeRenderer component to entity: " << entity->m_name << std::endl;
    }
    // Add other component types as needed
    else {
        std::cout << "Unknown component type: " << componentType << std::endl;
    }
}

Vector3D Scene::DeserializeVector3D(const Json::Value& json) {
    Vector3D vector;
    vector.m_x = json.get("x", 0.0f).asFloat();
    vector.m_y = json.get("y", 0.0f).asFloat();
    vector.m_z = json.get("z", 0.0f).asFloat();
    return vector;
}

void Scene::AddGameObject(GameObject* gameObject) {
    m_gameObjects.push_back(gameObject);
}

void Scene::AddEntity(Entity* entity) {
    m_entities.push_back(entity);
}

void Scene::RemoveGameObject(GameObject* gameObject) {
    auto it = std::find(m_gameObjects.begin(), m_gameObjects.end(), gameObject);
    if (it != m_gameObjects.end()) {
        m_gameObjects.erase(it);
    }
}

void Scene::RemoveEntity(Entity* entity) {
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it != m_entities.end()) {
        m_entities.erase(it);
    }
}

void Scene::RestoreEntityParentRelationship(const Json::Value& json) {
    std::string childName = json["name"].asString();
    
    if (json.isMember("parent") && !json["parent"].isNull()) {
        std::string parentName = json["parent"].asString();
        
        // Find the child and parent entities in EntityManager
        std::vector<Entity*> allEntities = EntityManager::get_all();
        Entity* childEntity = nullptr;
        Entity* parentEntity = nullptr;
        
        for (Entity* entity : allEntities) {
            if (entity->m_name == childName) {
                childEntity = entity;
            }
            if (entity->m_name == parentName) {
                parentEntity = entity;
            }
            
            if (childEntity && parentEntity) {
                break;
            }
        }
        
        // Establish the parent-child relationship if both entities exist
        if (childEntity && parentEntity) {
            // Use ParentingManager to establish relationship
            ParentingManager::get().AddObject(childEntity, parentEntity);
            std::cout << "Restored entity parent relationship: " << childName << " -> " << parentName << std::endl;
        }
        else {
            std::cerr << "Failed to restore entity parent relationship for " << childName 
                      << " (parent: " << parentName << ")" << std::endl;
        }
    }
}

