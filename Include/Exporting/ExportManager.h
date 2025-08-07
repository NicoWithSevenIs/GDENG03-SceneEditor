#pragma once

#include "Interfaces/Singleton.h"
#include <fstream>

#include "ECS/Systems/EntityManager.h"
#include <limits>
class ExportManager : public Singleton<ExportManager> {
	
		
	public:
		inline static void ExportToUnity(std::string file_name) {
			std::ofstream file(file_name +  ".unity");
			file << "%YAML 1.1\n";
			file << "%TAG !u! tag:unity3d.com,2011:\n";
			auto all = EntityManager::get_all();
			for (auto e : all) {
				std::string output;
				e->ExportToUnity(output);
				file << output;
			}
			file << "--- !u!1660057539 &9223372036854775807\n";
			file << "SceneRoots:\n";
			file << "  m_ObjectHideFlags: 0\n";
			file << "  m_Roots:\n";
			for (auto e : all) {
				file << "  - {fileID: " << e->GUID << "}\n";
			}
			file.close();
		}

};