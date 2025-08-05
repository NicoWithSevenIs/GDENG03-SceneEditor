#include "UI/MenuBarUI.h"
#include "ECS/Systems/EntityManager.h"
#include "UI/UIManager.h"
#include "ECS/Components/CubeRenderer.h"
#include "ECS/Components/QuadRenderer.h"
#include "ECS/Components/SphereRenderer.h"
#include "ECS/Systems/TimelineManager.h"
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
					if(prompt_input[0] == '\0' || prompt_input[0] == ' ' || prompt_input == nullptr) return;
					auto e = new Entity(prompt_input);
					e->AddComponent<CubeRenderer>();
					EntityManager::AddObject(e);
					TimelineManager::get().SetDirty();
				};

			}
			if (ImGui::MenuItem("Create Sphere")) {
				doOnPrompt = [this]() {
					if (prompt_input[0] == '\0' || prompt_input[0] == ' ' || prompt_input == nullptr) return;
					auto e = new Entity(prompt_input);
					e->AddComponent<SphereRenderer>();
					EntityManager::AddObject(e);
					TimelineManager::get().SetDirty();
				};
			}
			if (ImGui::MenuItem("Create Quad")) {
				doOnPrompt = [this]() {
					if (prompt_input[0] == '\0' || prompt_input[0] == ' ' || prompt_input == nullptr) return;
					auto e = new Entity(prompt_input);
					e->AddComponent<QuadRenderer>();
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
			ImGui::EndMenu();
		}
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
