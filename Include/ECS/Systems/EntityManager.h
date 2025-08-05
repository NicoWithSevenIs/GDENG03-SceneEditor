#pragma once

#include "Interfaces/Singleton.h"

#include "ECS/Entities/Entity.h"
#include "GameObject/ParentingManager.h"

#include "Game Engine/EngineTime.h"

#include "Rendering/Mesh.h"

#include <functional>
#include <vector>
#include <unordered_map>

namespace SceneEditor {
	class EntityManager : public Singleton<EntityManager>
	{
	private:
		std::vector <SceneEditor::Entity* > m_object_list;

	public:
		static void Initialize();
		static void Update(Matrix4x4 view_mat, Matrix4x4 proj_mat);
		static void Draw();
		static void AddObject(SceneEditor::Entity* object, SceneEditor::Entity* parent = nullptr);
		static void Release();

	static void DoOnAll(std::function<void(Entity*)> callback);
	static std::vector<Entity*> get_all();

	inline static void ResetUpdatedFlags() {
		for (auto obj : get().m_object_list) {
			obj->updated = false;
		}
	}

	static void ReplaceHierarchy(std::vector<Entity*> entities);
};

}

