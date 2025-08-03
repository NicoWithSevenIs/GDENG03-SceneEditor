#pragma once

#include <Windows.h>
#include "vector"
#include "Library/Interfaces/Singleton.h"
#include "HierarchyScreen.h"
#include "InspectorScreen.h"
#include "MenuBarUI.h"
#include "IMediator.h"

#include <unordered_map>

class UIManager : public Singleton<UIManager>, public IMediator<UIScreen>
{
	public:
		static void initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* device_context);
		static void draw();

	public:
		static void addUIScreen(std::string name, UIScreen* screen);
		void Notify(std::string notification, UIScreen* notifier) override;
		UIScreen* Retrieve(std::string name) override;

	private:
		std::unordered_map<std::string, UIScreen*> map_ui_screens;

		float width;
		float height;
};

