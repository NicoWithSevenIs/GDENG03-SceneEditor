#include "ECS/Systems/EntityManager.h"
//#include "ECS/Systems/PhysicsSystem.h"
#include "Utilities.h"

#include "ECS/Components/CubeRenderer.h"
//#include "ECS/Systems/PhysicsSystem.h"
//class PhysicsSystem;


namespace SE = SceneEditor;

void SE::EntityManager::Initialize()
{
	SceneEditor::Entity *e = new SceneEditor::Entity();
	e->AddComponent<CubeRenderer>();
	//e->AddComponent<>
	//Phy
	
	AddObject(e);
}

void SE::EntityManager::Update(Matrix4x4 view_mat, Matrix4x4 proj_mat)
{
	for (auto e : get().m_object_list) {
		//auto physics = Utilities::Where<Component*>(e->GetComponents(), [](Component* c) {return c->Type == ComponentType::PHYSICS; });
		//for (auto p : physics)
		//	p->Update(e->cc);

		e->Update(view_mat, proj_mat);
		auto scripts = Utilities::Where<Component*>(e->GetComponents(), [](Component* c) {return c->Type == ComponentType::SCRIPT; });
		for (auto s : scripts)
			s->Update(e->cc);
	}
}

void SE::EntityManager::Draw()
{

	for (auto e : get().m_object_list) {
		auto renderers = Utilities::Where<Component*>(e->GetComponents(), [](Component* c) {return c->Type == ComponentType::RENDERER; });
		for(auto r: renderers)
			r->Update(e->cc);
	}

}

void SE::EntityManager::AddObject(SceneEditor::Entity* object, SceneEditor::Entity* parent)
{
	for (auto i : get().m_object_list) {
		if (object->m_name == i->m_name)
			return;
	}

	get().m_object_list.push_back(object);
	ParentingManager::get().AddObject(object, parent);
}

void SE::EntityManager::Release()
{
	for (auto e : get().m_object_list)
		e->Release();
}

void SE::EntityManager::DoOnAll(std::function<void(SceneEditor::Entity*)> callback)
{
	for (auto e : get().m_object_list)
		callback(e);
}

std::vector<SE::Entity*> SE::EntityManager::get_all()
{
	return std::vector<SE::Entity*>(get().m_object_list);
}
