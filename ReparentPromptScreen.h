#pragma once

#include <string>
#include "GameObject.h"

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui.h"

#include <vector>
class ReparentPromptScreen
{

	public:
		
		GameObject* invoker = nullptr;
		GameObject* selection = nullptr;

		bool aborted = false;
		bool invoked = false;


		void draw();
		void drawParents(GameObject* obj);

	private:
		std::vector<GameObject*> rendered;
};

