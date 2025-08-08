#pragma once
#include <string>

namespace DX3D {
    class UnitySceneImporter {
    public:
        static bool ImportUnityScene(const std::string& unityFilePath);
        
        static bool ConvertUnitySceneToJson(const std::string& unityFilePath, const std::string& outJsonPath);
    };
}
