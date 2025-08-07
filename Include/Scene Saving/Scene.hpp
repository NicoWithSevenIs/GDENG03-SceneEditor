#pragma once
#include <string>
#include <vector>
#include <json/json.h>

class GameObject;
class Entity;
class Transform;
class Vector3D;
class Component;

namespace DX3D {
    class Scene {
    private:
        std::string m_sceneName;
        std::vector<Entity*> m_entities;

    public:

    public:
        Scene();
        Scene(const std::string& name);
        ~Scene();

    private:
        Json::Value SerializeGameObject(GameObject* gameObject);
        Json::Value SerializeEntity(Entity* entity);
        Json::Value SerializeTransform(const Transform& transform);
        Json::Value SerializeVector3D(const Vector3D& vector);
        Json::Value SerializeComponent(Component* component);

        void DeserializeEntity(const Json::Value& json);
        void DeserializeComponent(const Json::Value& json, Entity* entity);
        void ConvertGameObjectToEntity(const Json::Value& json);
        void RestoreEntityParentRelationship(const Json::Value& json);
        Vector3D DeserializeVector3D(const Json::Value& json);

    public:
        void CaptureCurrentScene();
        void LoadIntoScene();
        void Clear();

        Json::Value SerializeToJson();
        void DeserializeFromJson(const Json::Value& json);

        void AddEntity(Entity* entity);
        void RemoveEntity(Entity* entity);

    protected:
    public:

    public:
        const std::string& GetSceneName() const { return m_sceneName; }
        void SetSceneName(const std::string& name) { m_sceneName = name; }

        const std::vector<Entity*>& GetEntities() const { return m_entities; }
    };
}

