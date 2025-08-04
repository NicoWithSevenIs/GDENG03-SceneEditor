#pragma once
#include "UI/UIScreen.h"
#include <functional>
#include <string>

class MenuBarUI: public UIScreen
{
	private:
		std::function<void()> doOnPrompt = nullptr;
		char prompt_input[128];
		
		// Scene management
		char scene_name_input[128];
		char scene_path_input[256];
		bool show_save_dialog = false;
		bool show_load_dialog = false;

	public:
		MenuBarUI(float width, float height);
		void draw() override;
		void ShowPrompt();
		void ShowSaveDialog();
		void ShowLoadDialog();

};

