#include "ECS/Entities/Entity.h"
#include "Math/Vector3D.h"

Json::Value Entity::SerializeToJson() const {
    Json::Value entityJson;
    
    // Basic entity info
    entityJson["name"] = m_name;
    entityJson["guid"] = m_name; // Since IGUID uses m_name
    
    // Transform data
    Json::Value transformJson;
    transformJson["position"]["x"] = m_transform.m_translation.m_x;
    transformJson["position"]["y"] = m_transform.m_translation.m_y;
    transformJson["position"]["z"] = m_transform.m_translation.m_z;
    
    transformJson["rotation"]["x"] = m_transform.m_rotation.m_x;
    transformJson["rotation"]["y"] = m_transform.m_rotation.m_y;
    transformJson["rotation"]["z"] = m_transform.m_rotation.m_z;
    
    transformJson["scale"]["x"] = m_transform.m_scale.m_x;
    transformJson["scale"]["y"] = m_transform.m_scale.m_y;
    transformJson["scale"]["z"] = m_transform.m_scale.m_z;
    
    entityJson["transform"] = transformJson;
    
    // Components
    Json::Value componentsArray(Json::arrayValue);
    for (const Component* component : components) {
        Json::Value componentJson;
        componentJson["type"] = static_cast<int>(component->Type);
        
        // TODO: Add component-specific serialization
        // You'll need to extend this based on your component types
        switch (component->Type) {
            case ComponentType::SCRIPT:
                componentJson["component_type"] = "SCRIPT";
                break;
            case ComponentType::RENDERER:
                componentJson["component_type"] = "RENDERER";
                break;
            default:
                componentJson["component_type"] = "UNKNOWN";
                break;
        }
        
        componentsArray.append(componentJson);
    }
    entityJson["components"] = componentsArray;
    
    return entityJson;
}

void Entity::DeserializeFromJson(const Json::Value& json) {
    // Basic entity info
    if (json.isMember("name")) {
        m_name = json["name"].asString();
    }
    
    // Transform data
    if (json.isMember("transform")) {
        const Json::Value& transformJson = json["transform"];
        
        if (transformJson.isMember("position")) {
            const Json::Value& pos = transformJson["position"];
            m_transform.m_translation.m_x = pos.get("x", 0.0f).asFloat();
            m_transform.m_translation.m_y = pos.get("y", 0.0f).asFloat();
            m_transform.m_translation.m_z = pos.get("z", 0.0f).asFloat();
        }
        
        if (transformJson.isMember("rotation")) {
            const Json::Value& rot = transformJson["rotation"];
            m_transform.m_rotation.m_x = rot.get("x", 0.0f).asFloat();
            m_transform.m_rotation.m_y = rot.get("y", 0.0f).asFloat();
            m_transform.m_rotation.m_z = rot.get("z", 0.0f).asFloat();
        }
        
        if (transformJson.isMember("scale")) {
            const Json::Value& scale = transformJson["scale"];
            m_transform.m_scale.m_x = scale.get("x", 1.0f).asFloat();
            m_transform.m_scale.m_y = scale.get("y", 1.0f).asFloat();
            m_transform.m_scale.m_z = scale.get("z", 1.0f).asFloat();
        }
    }
    
    // Components (basic implementation)
    if (json.isMember("components")) {
        const Json::Value& componentsArray = json["components"];
        
        // Clear existing components
        for (Component* comp : components) {
            comp->Release();
            delete comp;
        }
        components.clear();
        
        // TODO: Recreate components based on saved data
        // This will need to be expanded based on your component system
        for (const Json::Value& componentJson : componentsArray) {
            if (componentJson.isMember("type")) {
                ComponentType type = static_cast<ComponentType>(componentJson["type"].asInt());
                
                // You'll need to implement component creation based on type
                // This is a placeholder for component deserialization
                switch (type) {
                    case ComponentType::SCRIPT:
                        // Create script component
                        break;
                    case ComponentType::RENDERER:
                        // Create renderer component
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
