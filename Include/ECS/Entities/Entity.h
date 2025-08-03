#pragma once
#include <vector>

#include "ECS/Components/Component.h"
#include "GameObject/IGUID.h"
#include "Game Engine/EngineTime.h"
#include "Math/Constant.h"
#include "Math/Transform.h"
class Entity: public IGUID{

	public:
		Transform m_transform;
		constant cc;

	private:
		std::vector<Component*> components;
		

	public:
		inline std::vector<Component*> GetComponents() { return std::vector<Component*>(components); }
		inline constant GetConstant() { return cc; }

	public:
		inline Entity(): IGUID("Entity"), m_transform(this){}

		
		template <typename T> inline
			typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type
			AddComponent()
		{
			for (auto c : components) {
				if (typeid(*c) == typeid(T))
					return nullptr;
			}
			T* component = new T();
			components.push_back(component);
			return component;
		}

		template <typename T> inline
			typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type
			GetComponent()
		{
			for (auto c : components) {
				if (typeid(*c) == typeid(T))
					return static_cast<T*>(c);
			}
			return nullptr;
		}

		template <typename T> inline
			typename std::enable_if<std::is_base_of<Component, T>::value, void>::type
			RemoveComponent()
		{
			std::erase_if(components, [](Component* c) {
				return typeid(*c) == typeid(T);
				});
		}

};