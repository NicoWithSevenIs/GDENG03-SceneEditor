#include "UI/HierarchyScreen.h"

HierarchyScreen::HierarchyScreen(float width, float height)
{
	this->width = width;
	this->height = height;
	this->m_name = "Hierarchy";
}

HierarchyScreen::~HierarchyScreen()
{
}

void HierarchyScreen::draw()
{
	ImGui::SetNextWindowSize(ImVec2(200, height));
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::Begin("Hierarchy");

	this->drawParents(nullptr);

	ImGui::End();
}

void HierarchyScreen::drawParents(GameObject* parent)
{
	
	auto root_objects = ParentingManager::get().GetChildren(parent);
	
	for (auto i : root_objects) {
		const char* p_name = i->m_name.c_str();
		if (ImGui::Button(p_name)) {
			auto inspector = (InspectorScreen*)m_screen->Retrieve("inspector");
			inspector->currTrackedObject = (GameObject*)i;
		}
		ImGui::Indent();
		this->drawParents((GameObject*) i);
		ImGui::Unindent();
	}
	
}

