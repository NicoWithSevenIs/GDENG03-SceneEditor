#include "UI/HierarchyScreen.h"
#include "UI/InspectorScreen.h"

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

void HierarchyScreen::drawParents(Entity* parent)
{
	
	auto root_objects = ParentingManager::get().GetChildren(parent);
	
	for (auto i : root_objects) {
		const char* p_name = i->m_name.c_str();

		bool enabled = ((Entity*)i)->updated;
		ImGui::Checkbox("", &enabled);
		ImGui::SameLine();
		if (ImGui::Button(p_name)) {
			auto inspector = (InspectorScreen*)m_screen->Retrieve("inspector");
			inspector->m_tracked_name = ((Entity*)i)->m_name;
		}
		ImGui::Indent();
		this->drawParents((Entity*) i);
		ImGui::Unindent();
	}
	
}

