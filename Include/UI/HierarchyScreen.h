#pragma once

#include "vector"
#include "unordered_map"
#include "string"

#include "GameObject/GameObject.h"
#include "GameObject/GameObjectManager.h"
#include "UI/UIScreen.h"

#include <vector>


class HierarchyScreen : public UIScreen
{
	public:
		HierarchyScreen(float width, float height);
		~HierarchyScreen();

	public:
		virtual void draw() override;
	
	private:
		void drawParents(GameObject* parent);

	private:
		std::vector<GameObject*> rendered;
};

