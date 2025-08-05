#include "ECS/Systems/EntityManager.h"
#include "Utilities.h"

#include "ECS/Components/CubeRenderer.h"

void EntityManager::Initialize()
{
	SceneEditor::Entity *e = new SceneEditor::Entity();
	e->AddComponent<CubeRenderer>();
	AddObject(e);
}

void EntityManager::Update(Matrix4x4 view_mat, Matrix4x4 proj_mat)
{
	for (auto e : get().m_object_list) {
		e->Update(view_mat, proj_mat);
		auto scripts = Utilities::Where<Component*>(e->GetComponents(), [](Component* c) {return c->Type == ComponentType::SCRIPT; });
		for (auto s : scripts)
			s->Update(e->cc);
	}
}

void EntityManager::Draw()
{

	for (auto e : get().m_object_list) {
		auto renderers = Utilities::Where<Component*>(e->GetComponents(), [](Component* c) {return c->Type == ComponentType::RENDERER; });
		for(auto r: renderers)
			r->Update(e->cc);
	}

}

void EntityManager::AddObject(SceneEditor::Entity* object, SceneEditor::Entity* parent)
{
	for (auto i : get().m_object_list) {
		if (object->m_name == i->m_name)
			return;
	}

	get().m_object_list.push_back(object);
	ParentingManager::get().AddObject(object, parent);
}

void EntityManager::Release()
{
	for (auto e : get().m_object_list)
		e->Release();
}

void EntityManager::DoOnAll(std::function<void(SceneEditor::Entity*)> callback)
{
	for (auto e : get().m_object_list)
		callback(e);
}

std::vector<Entity*> EntityManager::get_all()
{
	return std::vector<Entity*>(get().m_object_list);
}
