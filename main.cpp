#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "Window/AppWindow.h"

#include "Math/Matrix4x4.h"
#include "Constants/AppConstants.h"

namespace AppConstants {
	const std::string SCENE_SAVE_DIRECTORY = "Saved Scenes";
}


int main() {


	AppWindow app;

	if (app.init()) {
		while (app.isRunning()) {
			app.broadcast();

		}
	}


	return 0;
}