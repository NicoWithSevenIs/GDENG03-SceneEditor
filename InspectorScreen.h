#pragma once
#include "GameObject.h"
#include "string"
#include "vector"
#include "UIScreen.h"
#include "ParentingManager.h"
#include "ReparentPromptScreen.h"

class InspectorScreen : public UIScreen
{
	public:
		InspectorScreen(float width, float height);
		~InspectorScreen();

	public:
		virtual void draw() override;
		void drawInspector();
		void drawTranslateFields();
		void drawScaleFields();
		void drawRotFields();

		void getTrackedTransform();
		void applyChanges();
		//void applyChanges(std::string key);


		bool checkParentValid(std::string name);

		IGUID* getParentGUID(std::string name);

	public:
		GameObject* currTrackedObject;
		ReparentPromptScreen prompt;

	public:
		float m_translate_x = 0.0f;
		float m_translate_y = 0.0f;
		float m_translate_z = 0.0f;

		float m_scale_x = 0.0f;
		float m_scale_y = 0.0f;
		float m_scale_z = 0.0f;

		float m_rot_x = 0.0f;
		float m_rot_y = 0.0f;
		float m_rot_z = 0.0f;

		std::string m_tracked_name;
};

