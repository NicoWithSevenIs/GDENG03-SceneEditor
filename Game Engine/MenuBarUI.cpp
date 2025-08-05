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
					e->cc.hasTex = false;
					e->AddComponent<CubeRenderer>();
					PhysicsComponent* p6Component = e->AddComponent<PhysicsComponent>(reactphysics3d::BodyType::DYNAMIC);
					//e->AddComponent<PhysicsComponent>();
					PhysicsSystem::AddPhysicsComponent(p6Component);
					EntityManager::AddObject(e);
					TimelineManager::get().SetDirty();
				};

			}
			if (ImGui::MenuItem("Create Quad")) {
				doOnPrompt = [this]() {
					if (prompt_input[0] == '\0' || prompt_input[0] == ' ' || prompt_input == nullptr) return;
					auto e = new Entity(prompt_input);
					e->cc.hasTex = true;
					e->AddComponent<QuadRenderer>();
					EntityManager::AddObject(e);
					TimelineManager::get().SetDirty();
					};
			}
			if (ImGui::MenuItem("Create Sphere")) {
				doOnPrompt = [this]() {
					if (prompt_input[0] == '\0' || prompt_input[0] == ' ' || prompt_input == nullptr) return;
					auto e = new Entity(prompt_input);
					e->cc.hasTex = true;
					e->AddComponent<SphereRenderer>();
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
					e->cc.hasTex = true;
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
