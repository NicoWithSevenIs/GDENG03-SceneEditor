#pragma once
#include "UI/UIScreen.h"
#include <functional>
class MenuBarUI: public UIScreen
{
	private:
		std::function<void()> doOnPrompt = nullptr;
		char prompt_input[128];

		float cubeSpawnHeight = 1.5f;
		float cubeSpawnArea = 1.0f;

		// Scene management
		char scene_name_input[128];
		char scene_path_input[256];
		bool show_save_dialog = false;
		bool show_load_dialog = false;
		bool show_import_unity_dialog = false;
		char unity_path_input[256] = "";

	public:
		MenuBarUI(float width, float height);
		void draw() override;
		void ShowPrompt();
		void Spawn50Cubes();
		void ShowSaveDialog();
		void ShowLoadDialog();
		void ShowImportUnityDialog();


};

