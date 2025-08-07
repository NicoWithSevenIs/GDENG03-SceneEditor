#include "Scene Saving/SceneManager.hpp"
#include "ECS/Systems/EntityManager.h"
#include "Constants/AppConstants.h"
#include <fstream>
#include <iostream>
#include <filesystem>

using namespace DX3D;

SceneManager::SceneManager() : m_currentScene(nullptr) {}

SceneManager::~SceneManager() {
    m_currentScene.reset();
}


void SceneManager::CreateNewScene(const std::string& name) {
    get().m_currentScene = std::make_unique<Scene>(name);
    get().m_currentScenePath = "";
    std::cout << "Created new scene: " << name << std::endl;
}

std::string SceneManager::GetSaveFilePath(const std::string& filename) {
    if (!std::filesystem::exists(AppConstants::SCENE_SAVE_DIRECTORY)) {
        std::filesystem::create_directories(AppConstants::SCENE_SAVE_DIRECTORY);
    }

    if (filename.find(AppConstants::SCENE_SAVE_DIRECTORY) != std::string::npos) {
        return filename;
    }

    return AppConstants::SCENE_SAVE_DIRECTORY + "/" + filename;
}

bool SceneManager::SaveScene(const std::string& filepath) {
    if (!get().m_currentScene) {
        std::cerr << "No current scene to save!" << std::endl;
        return false;
    }

    CaptureCurrentGameState();

    std::string fullPath = get().GetSaveFilePath(filepath);
    Json::Value sceneJson = get().m_currentScene->SerializeToJson();

    if (get().WriteJsonToFile(sceneJson, fullPath)) {
        get().m_currentScenePath = fullPath;
        std::cout << "Scene saved successfully to: " << fullPath << std::endl;
        return true;
    }
    else {
        std::cerr << "Failed to save scene to: " << fullPath << std::endl;
        return false;
    }
}

bool SceneManager::SaveCurrentScene() {
    if (get().m_currentScenePath.empty()) {
        std::cerr << "No current scene path set. Use SaveScene(filepath) instead." << std::endl;
        return false;
    }

    return SaveScene(get().m_currentScenePath);
}

bool SceneManager::LoadScene(const std::string& filepath) {
    std::string fullPath = filepath;

    if (!std::filesystem::exists(filepath)) {
        fullPath = get().GetSaveFilePath(filepath);
    }

    if (!std::filesystem::exists(fullPath)) {
        std::cerr << "Scene file does not exist: " << filepath << std::endl;
        return false;
    }

    Json::Value sceneJson = get().ReadJsonFromFile(fullPath);
    if (sceneJson.isNull()) {
        std::cerr << "Failed to read scene file: " << fullPath << std::endl;
        return false;
    }

    // Clear all entities before loading new scene
    EntityManager::Release();

    get().m_currentScene = std::make_unique<Scene>();
    get().m_currentScene->DeserializeFromJson(sceneJson);
    get().m_currentScenePath = fullPath;

    get().m_currentScene->LoadIntoScene();

    std::cout << "Scene loaded successfully from: " << fullPath << std::endl;
    return true;
}

void SceneManager::ClearCurrentScene() {
    if (get().m_currentScene) {
        get().m_currentScene->Clear();
    }

    // Clear all entities
    EntityManager::Release();

    std::cout << "Current scene cleared" << std::endl;
}

void SceneManager::CaptureCurrentGameState() {
    if (!get().m_currentScene) {
        CreateNewScene("Captured Scene");
    }

    get().m_currentScene->CaptureCurrentScene();
    std::cout << "Current game state captured" << std::endl;
}

bool SceneManager::HasCurrentScene() {
    return get().m_currentScene != nullptr;
}

Scene* SceneManager::GetCurrentScene() {
    return get().m_currentScene.get();
}

std::string SceneManager::GetCurrentScenePath() {
    return get().m_currentScenePath;
}

void SceneManager::SetCurrentScenePath(const std::string& path) {
    get().m_currentScenePath = path;
}

bool SceneManager::WriteJsonToFile(const Json::Value& json, const std::string& filepath) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filepath << std::endl;
            return false;
        }

        Json::StreamWriterBuilder builder;
        builder["indentation"] = "  ";
        std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        writer->write(json, &file);

        file.close();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception while writing JSON file: " << e.what() << std::endl;
        return false;
    }
}

Json::Value SceneManager::ReadJsonFromFile(const std::string& filepath) {
    Json::Value root;

    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for reading: " << filepath << std::endl;
            return Json::nullValue;
        }

        Json::CharReaderBuilder builder;
        Json::String errors;

        if (!Json::parseFromStream(builder, file, &root, &errors)) {
            std::cerr << "Failed to parse JSON: " << errors << std::endl;
            return Json::nullValue;
        }

        file.close();
        return root;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception while reading JSON file: " << e.what() << std::endl;
        return Json::nullValue;
    }
}

