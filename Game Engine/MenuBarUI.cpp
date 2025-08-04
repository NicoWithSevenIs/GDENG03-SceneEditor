#include "UI/MenuBarUI.h"
#include "GameObject/GameObjectManager.h"
#include "UI/UIManager.h"
#include "Scene Saving/SceneManager.hpp"
#include "Scene Saving/SceneTest.h"
#include <filesystem>

MenuBarUI::MenuBarUI(float width, float height) {
	this->width = width;
	this->height = height;
	this->m_name = "Menu";

	memset(scene_name_input, 0, sizeof(scene_name_input));
	memset(scene_path_input, 0, sizeof(scene_path_input));
	strcpy_s(scene_path_input, "scene.json");
}

void MenuBarUI::draw() {
	ShowPrompt();
	ShowSaveDialog();
	ShowLoadDialog();

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

			ImGui::Separator();

			if (ImGui::MenuItem("Clear Scene")) {
				DX3D::SceneManager::ClearCurrentScene();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Create")) {
			if (ImGui::MenuItem("Create Cube")) {

				doOnPrompt = [this]() {
					GameObjectManager::get().MakeCube(std::string(prompt_input));
					};

			}
			if (ImGui::MenuItem("Create Quad")) {
				doOnPrompt = [this]() {
					GameObjectManager::get().MakeQuad(std::string(prompt_input));
					};
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Manage")) {
			if (ImGui::MenuItem("Destroy All Gameobjects")) {
				GameObjectManager::get().Release();

			}
			if (ImGui::MenuItem("Reset All Transforms")) {

				auto reset_transforms = [](GameObject* go) {
					go->m_transform.m_translation = Vector3D();
					go->m_transform.m_scale = Vector3D(1, 1, 1);
					go->m_transform.m_rotation = Vector3D();
					};
				GameObjectManager::get().DoOnAll(reset_transforms);

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

		if (ImGui::BeginMenu("Test")) {
			if (ImGui::MenuItem("Create Test Scene")) {
				SceneTest::CreateTestScene();
			}

			if (ImGui::MenuItem("Save Test Scene")) {
				SceneTest::SaveTestScene();
			}

			if (ImGui::MenuItem("Load Test Scene")) {
				SceneTest::LoadTestScene();
			}

			if (ImGui::MenuItem("Run Full Test")) {
				SceneTest::RunFullTest();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void MenuBarUI::ShowPrompt() {
	if (!doOnPrompt)
		return;

	ImGui::SetNextWindowSize(ImVec2(300, 80));
	ImGui::Begin("Input Name");
	ImGui::InputText("##Name:", this->prompt_input, IM_ARRAYSIZE(this->prompt_input));

	if (ImGui::Button("Submit")) {
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
		for (const auto& entry : std::filesystem::directory_iterator(".")) {
			if (entry.is_regular_file() && entry.path().extension() == ".json") {
				std::string filename = entry.path().filename().string();
				if (ImGui::Selectable(filename.c_str())) {
					strcpy_s(scene_path_input, filename.c_str());
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
