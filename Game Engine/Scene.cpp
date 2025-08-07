#include "Scene Saving/Scene.hpp"
#include "GameObject/GameObject.h"
#include "GameObject/Cube.h"
#include "GameObject/Quad.h"
#include "GameObject/ParentingManager.h"
#include "ECS/Entities/Entity.h"
#include "ECS/Components/Component.h"
#include "ECS/Components/CubeRenderer.h"
#include "ECS/Components/QuadRenderer.h"
#include "ECS/Components/SphereRenderer.h"
#include "ECS/Components/CylinderRenderer.h"
#include "ECS/Components/MeshRenderer.h"
#include "ECS/Components/PhysicsComponent.h"
#include "ECS/Systems/EntityManager.h"
#include "ECS/Systems/PhysicsSystem.h"
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

    std::vector<Entity*> entities = EntityManager::get_all();
    for (Entity* entity : entities) {
        m_entities.push_back(entity);
    }
    std::cout << "Captured " << entities.size() << " ECS Entities" << std::endl;
}

void Scene::LoadIntoScene() {
    std::vector<Entity*> loadedEntities = EntityManager::get_all();
    std::cout << "Scene loaded with " << loadedEntities.size() << " ECS Entities" << std::endl;
}

void Scene::Clear() {
    m_entities.clear();
}

Json::Value Scene::SerializeToJson() {
    Json::Value root;
    root["scene_name"] = m_sceneName;
    root["version"] = "1.0";

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
        for (const Json::Value& objJson : gameObjects) {
            ConvertGameObjectToEntity(objJson);
        }

        for (const Json::Value& objJson : gameObjects) {
            RestoreEntityParentRelationship(objJson);
        }
    }

    if (json.isMember("entities")) {
        const Json::Value& entities = json["entities"];
        for (const Json::Value& entityJson : entities) {
            DeserializeEntity(entityJson);
        }

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

    obj["transform"] = SerializeTransform(entity->m_transform);

    Json::Value constantObj;
    constantObj["color"]["x"] = entity->cc.m_color.m_x;
    constantObj["color"]["y"] = entity->cc.m_color.m_y;
    constantObj["color"]["z"] = entity->cc.m_color.m_z;
    constantObj["angle"] = entity->cc.m_angle;
    constantObj["isRandom"] = static_cast<int>(entity->cc.isRandom);
    constantObj["hasTex"] = static_cast<int>(entity->cc.hasTex);
    obj["constant"] = constantObj;

    Entity* parent = static_cast<Entity*>(ParentingManager::get().GetParent(entity));
    if (parent != nullptr) {
        obj["parent"] = parent->m_name;
    }
    else {
        obj["parent"] = Json::nullValue;
    }

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

    if (CubeRenderer* cubeRenderer = dynamic_cast<CubeRenderer*>(component)) {
        compObj["type"] = "CubeRenderer";
        compObj["isTextured"] = cubeRenderer->isTextured;
    }
    else if (QuadRenderer* quadRenderer = dynamic_cast<QuadRenderer*>(component)) {
        compObj["type"] = "QuadRenderer";
        compObj["isTextured"] = quadRenderer->isTextured;
    }
    else if (SphereRenderer* sphereRenderer = dynamic_cast<SphereRenderer*>(component)) {
        compObj["type"] = "SphereRenderer";
        compObj["isTextured"] = sphereRenderer->isTextured;
    }
    else if (CylinderRenderer* cylinderRenderer = dynamic_cast<CylinderRenderer*>(component)) {
        compObj["type"] = "CylinderRenderer";
        compObj["isTextured"] = cylinderRenderer->isTextured;
    }
    else if (MeshRenderer* meshRenderer = dynamic_cast<MeshRenderer*>(component)) {
        compObj["type"] = "MeshRenderer";
        compObj["isTextured"] = meshRenderer->isTextured;
        compObj["modelPath"] = meshRenderer->GetModelPath();
        compObj["texturePath"] = meshRenderer->GetTexturePath();
    }
    else if (PhysicsComponent* physicsComponent = dynamic_cast<PhysicsComponent*>(component)) {
        compObj["type"] = "PhysicsComponent";
        reactphysics3d::RigidBody* rigidBody = physicsComponent->GetRigidBody();
        if (rigidBody) {
            compObj["bodyType"] = static_cast<int>(rigidBody->getType());
            compObj["mass"] = rigidBody->getMass();
        }
        else {
            compObj["bodyType"] = static_cast<int>(reactphysics3d::BodyType::STATIC);
            compObj["mass"] = 1.0f;
        }
    }
    else {
        compObj["type"] = "Unknown";
    }

    return compObj;
}

void Scene::DeserializeEntity(const Json::Value& json) {
    std::string name = json["name"].asString();
    Entity* entity = new Entity();
    entity->m_name = name;

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

    if (json.isMember("constant")) {
        const Json::Value& constantJson = json["constant"];

        if (constantJson.isMember("color")) {
            entity->cc.m_color = DeserializeVector3D(constantJson["color"]);
        }
        if (constantJson.isMember("angle")) {
            entity->cc.m_angle = constantJson["angle"].asFloat();
        }
        if (constantJson.isMember("isRandom")) {
            entity->cc.isRandom = static_cast<UINT>(constantJson["isRandom"].asInt());
        }
        if (constantJson.isMember("hasTex")) {
            entity->cc.hasTex = static_cast<UINT>(constantJson["hasTex"].asInt());
        }
    }

    if (json.isMember("components")) {
        const Json::Value& components = json["components"];
        for (const Json::Value& compJson : components) {
            DeserializeComponent(compJson, entity);
        }
    }

    EntityManager::AddObject(entity);

    std::cout << "Loaded Entity: " << name << std::endl;
}

void Scene::DeserializeComponent(const Json::Value& json, Entity* entity) {
    if (!json.isMember("type")) {
        return;
    }

    std::string componentType = json["type"].asString();

    if (componentType == "CubeRenderer") {
        entity->AddComponent<CubeRenderer>();
        if (json.isMember("isTextured")) {
            entity->GetComponent<CubeRenderer>()->isTextured = json["isTextured"].asBool();
        }
        std::cout << "Added CubeRenderer component to entity: " << entity->m_name << std::endl;
    }
    else if (componentType == "QuadRenderer") {
        entity->AddComponent<QuadRenderer>();
        if (json.isMember("isTextured")) {
            entity->GetComponent<QuadRenderer>()->isTextured = json["isTextured"].asBool();
        }
        std::cout << "Added QuadRenderer component to entity: " << entity->m_name << std::endl;
    }
    else if (componentType == "SphereRenderer") {
        entity->AddComponent<SphereRenderer>();
        entity->GetComponent<SphereRenderer>()->Load();
        if (json.isMember("isTextured")) {
            entity->GetComponent<SphereRenderer>()->isTextured = json["isTextured"].asBool();
        }
        std::cout << "Added SphereRenderer component to entity: " << entity->m_name << std::endl;
    }
    else if (componentType == "CylinderRenderer") {
        entity->AddComponent<CylinderRenderer>();
        entity->GetComponent<CylinderRenderer>()->Load();
        if (json.isMember("isTextured")) {
            entity->GetComponent<CylinderRenderer>()->isTextured = json["isTextured"].asBool();
        }
        std::cout << "Added CylinderRenderer component to entity: " << entity->m_name << std::endl;
    }
    else if (componentType == "MeshRenderer") {
        entity->AddComponent<MeshRenderer>();

        std::string modelPath = json.get("modelPath", "").asString();
        std::string texturePath = json.get("texturePath", "").asString();

        if (!modelPath.empty()) {
            entity->GetComponent<MeshRenderer>()->Load(modelPath, texturePath);
        }

        if (json.isMember("isTextured")) {
            entity->GetComponent<MeshRenderer>()->isTextured = json["isTextured"].asBool();
        }
        std::cout << "Added MeshRenderer component to entity: " << entity->m_name << std::endl;
    }
    else if (componentType == "PhysicsComponent") {
        reactphysics3d::BodyType bodyType = reactphysics3d::BodyType::STATIC;
        if (json.isMember("bodyType")) {
            bodyType = static_cast<reactphysics3d::BodyType>(json["bodyType"].asInt());
        }

        PhysicsComponent* physicsComp = entity->AddComponent<PhysicsComponent>(bodyType);

        if (json.isMember("mass") && physicsComp->GetRigidBody()) {
            float mass = json["mass"].asFloat();
            physicsComp->GetRigidBody()->setMass(mass);
        }

        std::cout << "Added PhysicsComponent to entity: " << entity->m_name << std::endl;
    }
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

void Scene::ConvertGameObjectToEntity(const Json::Value& json) {
    std::string name = json["name"].asString();
    std::string type = json.get("type", "GameObject").asString();

    Entity* entity = new Entity();
    entity->m_name = name;

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

    if (json.isMember("color")) {
        entity->cc.m_color = DeserializeVector3D(json["color"]);
    }

    if (json.isMember("isRainbow")) {
        entity->cc.isRandom = json["isRainbow"].asBool() ? 1 : 0;
    }

    if (type == "Cube") {
        entity->AddComponent<CubeRenderer>();
    }
    else if (type == "Quad") {
        entity->AddComponent<QuadRenderer>();
    }
    else {
        entity->AddComponent<CubeRenderer>();
    }

    EntityManager::AddObject(entity);

    std::cout << "Converted legacy GameObject to Entity: " << name << " (type: " << type << ")" << std::endl;
}

void Scene::AddEntity(Entity* entity) {
    m_entities.push_back(entity);
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

        if (childEntity && parentEntity) {
            ParentingManager::get().AddObject(childEntity, parentEntity);
            std::cout << "Restored entity parent relationship: " << childName << " -> " << parentName << std::endl;
        }
        else {
            std::cerr << "Failed to restore entity parent relationship for " << childName
                << " (parent: " << parentName << ")" << std::endl;
        }
    }
}

