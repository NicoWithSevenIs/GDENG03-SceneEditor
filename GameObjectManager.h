#pragma once
#include "Library/Interfaces/Singleton.h"
#include <unordered_map>
#include "GameObject.h"
#include "ParentingManager.h"
#include "Time.h"

#include "../Game Engine/Graphics Engine/Cube/Cube.h"
#include "Quad.h"
#include "Mesh.h"

#include <functional>
#include <vector>

class GameObjectManager: public Singleton<GameObjectManager>
{
	private:
		std::unordered_map<std::string, GameObject*> m_map_objects;

	public:
		static void Initialize();
		static void Update(Matrix4x4 view_mat, Matrix4x4 proj_mat);
		static void Draw();
		static void AddObject(GameObject* object, GameObject* parent = nullptr);
		static void Release();

		static void MakeCube(std::string name);
		static void MakeQuad(std::string name);

		static void DoOnAll(std::function<void(GameObject*)> callback);
		static std::vector<GameObject*> get_all();
};

