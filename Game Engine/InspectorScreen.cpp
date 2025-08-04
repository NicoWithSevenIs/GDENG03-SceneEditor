#include "UI/InspectorScreen.h"

InspectorScreen::InspectorScreen(float width, float height)
{
	this->width = width;
	this->height = height;
	this->m_name = "Inspector";
}

InspectorScreen::~InspectorScreen()
{
}

void InspectorScreen::draw()
{
	this->getTrackedTransform();

	float windowWidth = 300;
	ImGui::SetNextWindowSize(ImVec2(windowWidth, 500));
	ImGui::SetNextWindowPos(ImVec2(width - windowWidth, 20));
	ImGui::Begin("Inspector");

	if (this->currTrackedObject != nullptr) {
		this->drawInspector();
		if (ImGui::Button("Reparent")) {
			prompt.invoker = currTrackedObject;
		}
		if (prompt.selection != nullptr) {
			ParentingManager::get().SetParent(currTrackedObject, prompt.selection);
			prompt.invoker = nullptr;
			prompt.selection = nullptr;
		}
		if (ParentingManager::get().hasChild(currTrackedObject) && ImGui::Button("Unparent")) {
			ParentingManager::get().Unparent(currTrackedObject);
		}
	}

	ImGui::End();

	prompt.draw();
}

void InspectorScreen::drawInspector()
{
	ImGui::Text("%s", m_tracked_name.c_str());

	ImGui::Text("Translation");
	this->drawTranslateFields();

	ImGui::Text("Scale");
	this->drawScaleFields();

	ImGui::Text("Rotation");
	this->drawRotFields();

	if (ImGui::Checkbox("Enabled", &this->enabled)) {
		this->currTrackedObject->enabled = this->enabled;
	}
}

void InspectorScreen::drawTranslateFields()
{
	float inputWidth = 60.0f;
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##Translate_X", &this->m_translate_x)) {
		this->applyChanges();
	}
	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	if (ImGui::InputFloat("##Translate_Y", &this->m_translate_y)) {
		this->applyChanges();
	}
	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	if (ImGui::InputFloat("##Translate_Z", &this->m_translate_z)) {
		this->applyChanges();
	}
	ImGui::PopItemWidth();
}

void InspectorScreen::drawScaleFields()
{
	float inputWidth = 60.0f;

	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##Scale_X", &this->m_scale_x)) {
		this->applyChanges();
	}
	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	if (ImGui::InputFloat("##Scale_Y", &this->m_scale_y)) {
		this->applyChanges();
	}
	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	if (ImGui::InputFloat("##Scale_Z", &this->m_scale_z)) {
		this->applyChanges();
	}
	ImGui::PopItemWidth();
}

void InspectorScreen::drawRotFields()
{
	float inputWidth = 60.0f;
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::PushItemWidth(inputWidth);
	if (ImGui::InputFloat("##Rot_X", &this->m_rot_x)) {
		this->applyChanges();
	}
	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	if (ImGui::InputFloat("##Rot_Y", &this->m_rot_y)) {
		this->applyChanges();
	}
	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	if (ImGui::InputFloat("##Rot_Z", &this->m_rot_z)) {
		this->applyChanges();
	}
	ImGui::PopItemWidth();
}

void InspectorScreen::getTrackedTransform()
{
	if (this->currTrackedObject != nullptr) {
		this->m_translate_x = this->currTrackedObject->m_transform.m_translation.m_x;
		this->m_translate_y = this->currTrackedObject->m_transform.m_translation.m_y;
		this->m_translate_z = this->currTrackedObject->m_transform.m_translation.m_z;

		this->m_scale_x = this->currTrackedObject->m_transform.m_scale.m_x;
		this->m_scale_y = this->currTrackedObject->m_transform.m_scale.m_y;
		this->m_scale_z = this->currTrackedObject->m_transform.m_scale.m_z;

		this->m_rot_x = this->currTrackedObject->m_transform.m_rotation.m_x;
		this->m_rot_y = this->currTrackedObject->m_transform.m_rotation.m_y;
		this->m_rot_z = this->currTrackedObject->m_transform.m_rotation.m_z;

		this->m_tracked_name = this->currTrackedObject->m_name.c_str();
		this->enabled =  this->currTrackedObject->enabled;
	}
	else {
		std::string name = "NONE";
		this->m_tracked_name = name;
	}
}

void InspectorScreen::applyChanges()
{
	if (this->currTrackedObject == nullptr) return;

	this->currTrackedObject->m_transform.m_translation.m_x = this->m_translate_x;
	this->currTrackedObject->m_transform.m_translation.m_y = this->m_translate_y;
	this->currTrackedObject->m_transform.m_translation.m_z = this->m_translate_z;

	this->currTrackedObject->m_transform.m_scale.m_x = this->m_scale_x;
	this->currTrackedObject->m_transform.m_scale.m_y = this->m_scale_y;
	this->currTrackedObject->m_transform.m_scale.m_z = this->m_scale_z;

	this->currTrackedObject->m_transform.m_rotation.m_x = this->m_rot_x;
	this->currTrackedObject->m_transform.m_rotation.m_y = this->m_rot_y;
	this->currTrackedObject->m_transform.m_rotation.m_z = this->m_rot_z;


}


bool InspectorScreen::checkParentValid(std::string name)
{
	std::vector<IGUID*> parents;
	for (const auto& pair : ParentingManager::get().GetHierarchy()) {
		parents.push_back(pair.first);
	}

	for (IGUID* target : parents) {
		if (target->m_name == name) {
			return true;
		}
	}

	return false;
}

IGUID* InspectorScreen::getParentGUID(std::string name)
{
	std::vector<IGUID*> parents;
	for (const auto& pair : ParentingManager::get().GetHierarchy()) {
		parents.push_back(pair.first);
	}

	for (IGUID* target : parents) {
		if (target->m_name == name) {
			return target;
		}
	}

	return nullptr;
}




