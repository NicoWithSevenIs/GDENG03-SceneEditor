#include "ECS/Systems/EntityManager.h"
#include ""
void EntityManager::Initialize()
{
}

void EntityManager::Update(Matrix4x4 view_mat, Matrix4x4 proj_mat)
{
}

void EntityManager::Draw()
{

}

void EntityManager::AddObject(Entity* object, Entity* parent)
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

void EntityManager::DoOnAll(std::function<void(Entity*)> callback)
{
	for (auto e : get().m_object_list)
		callback(e);
}

std::vector<Entity*> EntityManager::get_all()
{
	return std::vector<Entity*>(get().m_object_list);
}
