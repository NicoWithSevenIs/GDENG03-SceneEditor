#include "ECS/Systems/PhysicsSystem.h"
#include "UI/MenuBarUI.h"
//#include "ECS/Systems/EntityManager.h"
#include "UI/UIManager.h"
#include "UI/DebugWindow.h"
#include "ECS/Components/CubeRenderer.h"
#include "ECS/Components/QuadRenderer.h"
#include "ECS/Components/SphereRenderer.h"
#include "ECS/Components/CylinderRenderer.h"
#include "ECS/Components/MeshRenderer.h"
#include "ECS/Systems/TimelineManager.h"
#include "Scene Saving/SceneManager.hpp"
#include "Constants/AppConstants.h"
#include "Game Engine/UnitySceneImporter.h"
#include <random>
#include <sstream>
#include <filesystem>

void MenuBarUI::ShowImportUnityDialog() {
	if (!show_import_unity_dialog)
		return;
	std::cout << "[MenuBarUI] ShowImportUnityDialog called!" << std::endl;
	ImGui::SetNextWindowSize(ImVec2(400, 120));
	ImGui::Begin("Import Unity Scene", &show_import_unity_dialog);
	ImGui::Text("Unity .unity File Path:");
	ImGui::InputText("##UnityFilePath", unity_path_input, IM_ARRAYSIZE(unity_path_input));
	ImGui::Separator();

	ImGui::Text("Suggested files:");
	try {
		std::vector<std::string> directories = { "Saved Scenes" };
		for (const auto& dir : directories) {
			if (std::filesystem::exists(dir)) {
				for (const auto& entry : std::filesystem::directory_iterator(dir)) {
					if (entry.is_regular_file() && entry.path().extension() == ".unity") {
						std::string filename = entry.path().filename().string();
						std::string displayName = dir + "/" + filename;
						if (ImGui::Selectable(displayName.c_str())) {
							strcpy_s(unity_path_input, displayName.c_str());
						}
					}
				}
			}
		}
	} catch (...) {
		ImGui::Text("Could not read directory");
	}

	ImGui::Separator();
	if (ImGui::Button("Import")) {
		std::cout << "[MenuBarUI] Import button pressed! Path: " << unity_path_input << std::endl;
		if (strlen(unity_path_input) > 0) {
			std::string unityPath = std::string(unity_path_input);
			std::string jsonPath = unityPath.substr(0, unityPath.find_last_of('.')) + ".json";
			if (DX3D::UnitySceneImporter::ConvertUnitySceneToJson(unityPath, jsonPath)) {
				std::cout << "[MenuBarUI] Unity scene converted to: " << jsonPath << std::endl;
				std::cout << "[MenuBarUI] You can now load it using Load Scene!" << std::endl;
			}
		}
		show_import_unity_dialog = false;
		memset(unity_path_input, 0, sizeof(unity_path_input));
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		std::cout << "[MenuBarUI] Import dialog canceled!" << std::endl;
		show_import_unity_dialog = false;
		memset(unity_path_input, 0, sizeof(unity_path_input));
	}
	ImGui::End();
}

MenuBarUI::MenuBarUI(float width, float height)
{
	this->width = width;
	this->height = height;
	this->m_name = "Menu";

	memset(scene_name_input, 0, sizeof(scene_name_input));
	memset(scene_path_input, 0, sizeof(scene_path_input));
	strcpy_s(scene_path_input, "scene.json");
}

void MenuBarUI::draw()
{
	ShowPrompt();
	ShowSaveDialog();
	ShowLoadDialog();
	ShowImportUnityDialog();

	if (ImGui::BeginMainMenuBar() && !doOnPrompt && !show_save_dialog && !show_load_dialog) {
		
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New Scene")) {
				DX3D::SceneManager::CreateNewScene("New Scene");
			}

			if (ImGui::MenuItem("Save Scene")) {
				show_save_dialog = true;
			}

			if (ImGui::MenuItem("Save Scene As...")) {
				show_save_dialog = true;
				memset(scene_path_input, 0, sizeof(scene_path_input));
				strcpy_s(scene_path_input, "new_scene.json");
			}

			   if (ImGui::MenuItem("Load Scene")) {
				   show_load_dialog = true;
			   }


			   if (ImGui::MenuItem("Import Unity Scene (.unity)...")) {
				   std::cout << "[MenuBarUI] Import Unity Scene menu item clicked!" << std::endl;
				   show_import_unity_dialog = true;
			   }


			   ImGui::Separator();

			   if (ImGui::MenuItem("Clear Scene")) {
				   DX3D::SceneManager::ClearCurrentScene();
			   }

			   ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Create")) {
			if (ImGui::MenuItem("Create Cube")) {

				doOnPrompt = [this]() {
					if(prompt_input[0] == '\0' || prompt_input[0] == ' ' || prompt_input == nullptr) return;
					auto e = new Entity(prompt_input);
					e->cc.hasTex = false;
					e->AddComponent<CubeRenderer>();
					//PhysicsComponent* p6Component = e->AddComponent<PhysicsComponent>(reactphysics3d::BodyType::DYNAMIC);
					////e->AddComponent<PhysicsComponent>();
					//PhysicsSystem::AddPhysicsComponent(p6Component);
					EntityManager::AddObject(e);
					TimelineManager::get().SetDirty();
				};

			}
			if (ImGui::MenuItem("Create Quad")) {
				doOnPrompt = [this]() {
					if (prompt_input[0] == '\0' || prompt_input[0] == ' ' || prompt_input == nullptr) return;
					auto e = new Entity(prompt_input);
					e->cc.hasTex = false;
					e->AddComponent<QuadRenderer>();
					EntityManager::AddObject(e);
					TimelineManager::get().SetDirty();
					};
			}
			if (ImGui::MenuItem("Create Sphere")) {
				doOnPrompt = [this]() {
					if (prompt_input[0] == '\0' || prompt_input[0] == ' ' || prompt_input == nullptr) return;
					auto e = new Entity(prompt_input);
					e->cc.hasTex = false;
					e->AddComponent<SphereRenderer>();
					e->GetComponent<SphereRenderer>()->Load();
					EntityManager::AddObject(e);
					TimelineManager::get().SetDirty();
				};
			}
			if (ImGui::MenuItem("Create Cylinder")) {
				doOnPrompt = [this]() {
					if (prompt_input[0] == '\0' || prompt_input[0] == ' ' || prompt_input == nullptr) return;
					auto e = new Entity(prompt_input);
					e->cc.hasTex = false;
					e->AddComponent<CylinderRenderer>();
					e->GetComponent<CylinderRenderer>()->Load();
					EntityManager::AddObject(e);
					TimelineManager::get().SetDirty();
					};
			}
			if (ImGui::MenuItem("Create Bunny")) {
				doOnPrompt = [this]() {
					if (prompt_input[0] == '\0' || prompt_input[0] == ' ' || prompt_input == nullptr) return;
					auto e = new Entity(prompt_input);
					e->cc.hasTex = true;
					e->AddComponent<MeshRenderer>();
					e->GetComponent<MeshRenderer>()->Load("Assets/Meshes/bunny.obj", "");
					EntityManager::AddObject(e);
					TimelineManager::get().SetDirty();
					};
			}
			if (ImGui::MenuItem("Create Armadillo")) {
				doOnPrompt = [this]() {
					if (prompt_input[0] == '\0' || prompt_input[0] == ' ' || prompt_input == nullptr) return;
					auto e = new Entity(prompt_input);
					e->cc.hasTex = true;
					e->AddComponent<MeshRenderer>();
					e->GetComponent<MeshRenderer>()->Load("Assets/Meshes/armadillo.obj", "");
					EntityManager::AddObject(e);
					TimelineManager::get().SetDirty();
					};
			}
			if (ImGui::MenuItem("Create Lucy")) {
				doOnPrompt = [this]() {
					if (prompt_input[0] == '\0' || prompt_input[0] == ' ' || prompt_input == nullptr) return;
					auto e = new Entity(prompt_input);
					e->cc.hasTex = true;
					e->AddComponent<MeshRenderer>();
					e->GetComponent<MeshRenderer>()->Load("Assets/Meshes/lucy.obj", "");
					EntityManager::AddObject(e);
					TimelineManager::get().SetDirty();
					};
			}
			if (ImGui::MenuItem("Create Teapot")) {
				doOnPrompt = [this]() {
					if (prompt_input[0] == '\0' || prompt_input[0] == ' ' || prompt_input == nullptr) return;
					auto e = new Entity(prompt_input);
					e->cc.hasTex = false;
					e->AddComponent<MeshRenderer>();
					e->GetComponent<MeshRenderer>()->Load("Assets/Meshes/teapot.obj","Assets/Textures/brick.png");
					EntityManager::AddObject(e);
					TimelineManager::get().SetDirty();
					};
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Manage")) {
			if (ImGui::MenuItem("Destroy All Gameobjects")) {
					EntityManager::get().Release();
					
			}
			if (ImGui::MenuItem("Reset All Transforms")) {
				
				auto reset_transforms = [](Entity* go) {
					go->m_transform.m_translation = Vector3D();
					go->m_transform.m_scale = Vector3D(1,1,1);
					go->m_transform.m_rotation = Vector3D();
				};
				EntityManager::get().DoOnAll(reset_transforms);

			}
			if (ImGui::MenuItem("Spawn 100 Cubes")) {
				Spawn100Cubes();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window")) {
			if (ImGui::MenuItem("Debug Console")) {
				auto debug_window = (DebugWindow*)m_screen->Retrieve("debug");
				if (debug_window) {
					debug_window->isOpen = !debug_window->isOpen;
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Scene")) {
			if (DX3D::SceneManager::HasCurrentScene()) {
				DX3D::Scene* currentScene = DX3D::SceneManager::GetCurrentScene();
				std::string sceneName = currentScene ? currentScene->GetSceneName() : "No Scene";
				ImGui::Text("Current Scene: %s", sceneName.c_str());

				std::string scenePath = DX3D::SceneManager::GetCurrentScenePath();
				if (!scenePath.empty()) {
					ImGui::Text("Path: %s", scenePath.c_str());
				}
				else {
					ImGui::Text("Path: Unsaved");
				}
			}
			else {
				ImGui::Text("No scene loaded");
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo")) {
				TimelineManager::get().Undo();
			}
			if (ImGui::MenuItem("Redo")) {
				TimelineManager::get().Redo();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

}

void MenuBarUI::ShowPrompt()
{
	if(!doOnPrompt)
		return;

	ImGui::SetNextWindowSize(ImVec2(300, 80));
	ImGui::Begin("Input Name");
	ImGui::InputText("##Name:", this->prompt_input, IM_ARRAYSIZE(this->prompt_input));

	if(ImGui::Button("Submit"))
	{
		doOnPrompt();
		memset(prompt_input, 0, sizeof(prompt_input));
		doOnPrompt = nullptr;
	}
	ImGui::End();


}

void MenuBarUI::ShowSaveDialog() {
	if (!show_save_dialog)
		return;

	ImGui::SetNextWindowSize(ImVec2(400, 150));
	ImGui::Begin("Save Scene", &show_save_dialog);

	ImGui::Text("Scene Name:");
	ImGui::InputText("##SceneName", scene_name_input, IM_ARRAYSIZE(scene_name_input));

	ImGui::Text("File Path:");
	ImGui::InputText("##FilePath", scene_path_input, IM_ARRAYSIZE(scene_path_input));

	ImGui::Separator();

	if (ImGui::Button("Save")) {
		if (strlen(scene_name_input) > 0) {
			if (DX3D::SceneManager::HasCurrentScene()) {
				DX3D::SceneManager::GetCurrentScene()->SetSceneName(std::string(scene_name_input));
			}
			else {
				DX3D::SceneManager::CreateNewScene(std::string(scene_name_input));
			}
		}

		if (DX3D::SceneManager::SaveScene(std::string(scene_path_input))) {
			show_save_dialog = false;
			memset(scene_name_input, 0, sizeof(scene_name_input));
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Cancel")) {
		show_save_dialog = false;
		memset(scene_name_input, 0, sizeof(scene_name_input));
	}

	ImGui::End();
}

void MenuBarUI::ShowLoadDialog() {
	if (!show_load_dialog)
		return;

	ImGui::SetNextWindowSize(ImVec2(400, 120));
	ImGui::Begin("Load Scene", &show_load_dialog);

	ImGui::Text("File Path:");
	ImGui::InputText("##LoadFilePath", scene_path_input, IM_ARRAYSIZE(scene_path_input));

	ImGui::Text("Suggested files:");
	try {
		std::vector<std::string> directories = { ".", AppConstants::SCENE_SAVE_DIRECTORY };
		
		for (const auto& dir : directories) {
			if (std::filesystem::exists(dir)) {
				for (const auto& entry : std::filesystem::directory_iterator(dir)) {
					if (entry.is_regular_file() && entry.path().extension() == ".json") {
						std::string filename = entry.path().filename().string();
						std::string displayName = (dir == ".") ? filename : AppConstants::SCENE_SAVE_DIRECTORY + "/" + filename;
						
						if (ImGui::Selectable(displayName.c_str())) {
							strcpy_s(scene_path_input, filename.c_str());
						}
					}
				}
			}
		}
	}
	catch (...) {
		ImGui::Text("Could not read directory");
	}

	ImGui::Separator();

	if (ImGui::Button("Load")) {
		if (DX3D::SceneManager::LoadScene(std::string(scene_path_input))) {
			show_load_dialog = false;
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Cancel")) {
		show_load_dialog = false;
	}

	ImGui::End();
}

void MenuBarUI::Spawn100Cubes()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(-cubeSpawnArea, cubeSpawnArea);

	std::cout << "[INFO] Spawning 100 cubes at height " << cubeSpawnHeight
		<< " within area " << cubeSpawnArea << "x" << cubeSpawnArea << std::endl;

	for (int i = 0; i < 100; i++) {
		std::stringstream ss;
		ss << "SpawnedCube_" << i;

		auto cube = new Entity(ss.str());
		cube->cc.hasTex = false;
		
		cube->AddComponent<CubeRenderer>();

		if (cube->GetComponent<PhysicsComponent>() == nullptr) {
			PhysicsComponent* p6component = cube->AddComponent<PhysicsComponent>(reactphysics3d::BodyType::DYNAMIC);
			PhysicsSystem::AddPhysicsComponent(p6component);
		}

		float randomX = dis(gen);
		float randomZ = dis(gen);
		cube->m_transform.m_translation = Vector3D(randomX, cubeSpawnHeight, randomZ);

		EntityManager::AddObject(cube);
	}

	TimelineManager::get().SetDirty();

	std::cout << "[INFO] Successfully spawned 100 cubes!" << std::endl;
}
