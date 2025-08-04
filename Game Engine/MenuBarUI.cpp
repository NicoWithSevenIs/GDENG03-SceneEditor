#include "UI/MenuBarUI.h"
#include "GameObject/GameObjectManager.h"
#include "UI/UIManager.h"

MenuBarUI::MenuBarUI(float width, float height)
{
	this->width = width;
	this->height = height;
	this->m_name = "Menu";
}

void MenuBarUI::draw()
{
	ShowPrompt();

	if (ImGui::BeginMainMenuBar() && !doOnPrompt) {
		
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
					go->m_transform.m_scale = Vector3D(1,1,1);
					go->m_transform.m_rotation = Vector3D();
				};
				GameObjectManager::get().DoOnAll(reset_transforms);

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
