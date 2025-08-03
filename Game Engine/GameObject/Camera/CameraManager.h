#pragma once
#include "Library/Interfaces/Singleton.h"
#include "Camera.h"
#include "Game Engine/Math/Matrix4x4.h"
#include <vector>
#include <string>
#include <unordered_map>

#include "PerspectiveCamera.h"

class CameraManager: public Singleton<CameraManager>
{
	public:
		static std::string PERSPECTIVE_CAMERA;

	private:
		std::unordered_map<std::string, Camera*> m_camera_map;
		Camera* current_camera;

	public:


		inline CameraManager() : Singleton() {
			//default camera
			//m_camera_map[PERSPECTIVE_CAMERA] = (Camera*) new PerspectiveCamera();
			//current_camera = m_camera_map[PERSPECTIVE_CAMERA];
		}

		inline static void GetCameraMatrices(Matrix4x4* view_mat, Matrix4x4** proj_mat) {
			//get().current_camera->GetViewProjMat(view_mat, proj_mat);
		}

		inline static void SwitchCamera(std::string name) {
			if (get().m_camera_map.contains(name)) {
				get().current_camera = get().m_camera_map[name];
			}
		}
};

