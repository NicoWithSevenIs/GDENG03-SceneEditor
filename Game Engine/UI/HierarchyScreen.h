#pragma once
#include "GameObject.h"

#include "vector"
#include "unordered_map"
#include "string"

#include "UIScreen.h"

#include "GameObjectManager.h"
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

