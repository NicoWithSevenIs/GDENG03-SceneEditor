#pragma once
#include "GameObject/Constant.h"

enum class ComponentType {
	SCRIPT,
	RENDERER
};

class Component
{
	public:
		const ComponentType Type;
		inline Component(ComponentType type): Type(type){}

		virtual void Update(constant c) = 0;
		virtual void Release() = 0;
};

