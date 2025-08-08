#pragma once
#include <string>

namespace DX3D {
    class UnitySceneImporter {
    public:
        // Import a Unity .unity YAML scene file and convert to engine entities
        static bool ImportUnityScene(const std::string& unityFilePath);
        
        // Convert Unity .unity scene to engine .json format
        static bool ConvertUnitySceneToJson(const std::string& unityFilePath, const std::string& outJsonPath);
    };
}
