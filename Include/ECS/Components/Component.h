#pragma once
#include "Math/Constant.h"

enum class ComponentType {
	SCRIPT,
	RENDERER
};

class Entity;

class Component
{
	public:
		Entity* owner;

	public:
		const ComponentType Type;
		inline Component(ComponentType type) : Type(type) {}

		inline virtual Component* CreateSnapshot() = 0;

		virtual void Update(constant c) = 0;
		virtual void Release() = 0;
};

