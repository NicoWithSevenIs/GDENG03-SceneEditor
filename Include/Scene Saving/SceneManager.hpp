#pragma once
#include <string>
#include <memory>
#include <json/json.h>
#include "Interfaces/Singleton.h"
#include "Scene Saving/Scene.hpp"

namespace DX3D {
    class SceneManager : public Singleton<SceneManager> {
    private:
        std::unique_ptr<Scene> m_currentScene;
        std::string m_currentScenePath;

    public:

    public:
        SceneManager();
        ~SceneManager();

    private:
        bool WriteJsonToFile(const Json::Value& json, const std::string& filepath);
        Json::Value ReadJsonFromFile(const std::string& filepath);
        std::string GetSaveFilePath(const std::string& filename);

    public:
        static void CreateNewScene(const std::string& name = "NewScene");
        static bool SaveScene(const std::string& filepath);
        static bool SaveCurrentScene();
        static bool LoadScene(const std::string& filepath);
        static void ClearCurrentScene();

        static void CaptureCurrentGameState();
        static bool HasCurrentScene();
        static Scene* GetCurrentScene();

    protected:
    public:

    public:
        static std::string GetCurrentScenePath();
        static void SetCurrentScenePath(const std::string& path);
    };
}

