#pragma once
#include "Math/Constant.h"
//#include "ECS/Entities/Entity.h"
//using namespace SceneEditor;


namespace SceneEditor {

enum class ComponentType {
	SCRIPT,
	RENDERER,
	PHYSICS
};

//class Entity;

class Entity;

class Component
{
public:
	SceneEditor::Entity* owner;

	public:
		const ComponentType Type;
		inline Component(ComponentType type) : Type(type) {}
		inline Component(ComponentType type, Entity* newOwner) : Type(type), owner(newOwner){}

		inline virtual Component* CreateSnapshot() = 0;

	virtual void Update(constant c) = 0;
	virtual void Release() = 0;
	virtual void Init() {};
};
}

