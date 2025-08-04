#pragma once
#include "Scene Saving/SceneManager.hpp"
#include "GameObject/GameObjectManager.h"
#include <iostream>

class SceneTest {
public:
    static void CreateTestScene() {
        std::cout << "=== Creating Test Scene ===" << std::endl;
        
        GameObjectManager::Release();
        
        GameObjectManager::MakeCube("TestCube1");
        GameObjectManager::MakeCube("TestCube2");
        GameObjectManager::MakeQuad("TestQuad1");
        
        auto objects = GameObjectManager::get_all();
        if (objects.size() >= 3) {
            objects[0]->m_transform.m_translation = Vector3D(1.0f, 0.0f, 0.0f);
            objects[0]->m_color = Vector3D(1.0f, 0.0f, 0.0f);
            
            objects[1]->m_transform.m_translation = Vector3D(-1.0f, 0.0f, 0.0f);
            objects[1]->m_color = Vector3D(0.0f, 1.0f, 0.0f);
            
            objects[2]->m_transform.m_translation = Vector3D(0.0f, 1.0f, 0.0f);
            objects[2]->m_transform.m_scale = Vector3D(2.0f, 2.0f, 1.0f);
            objects[2]->m_color = Vector3D(0.0f, 0.0f, 1.0f);
        }
        
        std::cout << "Created " << objects.size() << " test objects" << std::endl;
    }
    
    static void SaveTestScene() {
        std::cout << "=== Saving Test Scene ===" << std::endl;
        
        DX3D::SceneManager::CreateNewScene("Test Scene");
        
        if (DX3D::SceneManager::SaveScene("test_scene.json")) {
            std::cout << "Test scene saved successfully!" << std::endl;
        } else {
            std::cout << "Failed to save test scene!" << std::endl;
        }
    }
    
    static void LoadTestScene() {
        std::cout << "=== Loading Test Scene ===" << std::endl;
        
        if (DX3D::SceneManager::LoadScene("test_scene.json")) {
            std::cout << "Test scene loaded successfully!" << std::endl;
            
            auto objects = GameObjectManager::get_all();
            std::cout << "Loaded " << objects.size() << " objects" << std::endl;
            
            for (auto obj : objects) {
                std::cout << "Object: " << obj->m_name 
                         << " at (" << obj->m_transform.m_translation.m_x 
                         << ", " << obj->m_transform.m_translation.m_y 
                         << ", " << obj->m_transform.m_translation.m_z << ")" << std::endl;
            }
        } else {
            std::cout << "Failed to load test scene!" << std::endl;
        }
    }
    
    static void RunFullTest() {
        std::cout << "=== Running Full Scene Test ===" << std::endl;
        
        CreateTestScene();
        SaveTestScene();
        
        GameObjectManager::Release();
        std::cout << "Cleared scene, objects count: " << GameObjectManager::get_all().size() << std::endl;
        
        LoadTestScene();
        
        std::cout << "=== Test Complete ===" << std::endl;
    }
};
