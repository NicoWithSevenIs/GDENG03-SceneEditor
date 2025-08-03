#pragma once


#include <d3d11.h>
#include <string>
#include <functional>
#include <DirectXMath.h>
#include <cmath>


#include "Math/Vector3D.h"
#include "Math/Matrix4x4.h"
#include "Math/Transform.h"

#include "Rendering/GraphicsEngine.h"
#include "Rendering/DeviceContext.h"

#include "GameObject/IGUID.h"
#include "GameObject/ParentingManager.h"
#include "GameObject/Constant.h"

#include "ECS/Components/Component.h"
#include "ECS/Components/SampleComponent.h"

class GameObject: public IGUID
{
	protected:
		std::vector<Component*> components;
		constant cc;

	public:
		Transform m_transform;

	public:

		inline GameObject(std::string name): IGUID(name) {}

		inline virtual void Update(float delta_time, Matrix4x4 view_matrix, Matrix4x4 projection_matrix) {
			cc.m_world = this->m_transform.GetTransformationMatrix();
			auto parent = (GameObject*)ParentingManager::get().GetParent(this);
			auto child = this;

			while (parent != nullptr) {
				cc.m_world *= parent->m_transform.GetTransformationMatrix();
				child = parent;
				parent = parent = (GameObject*)ParentingManager::get().GetParent(child);
			}	
		}
		inline void OnUnparent() override {
	
			auto descendant = this;
			auto ancestry = (GameObject*)ParentingManager::get().GetParent(descendant);
			Matrix4x4 ancestry_matrix = m_transform.GetTransformationMatrix();

			while (ancestry != nullptr) {
				ancestry_matrix *= ancestry->m_transform.GetTransformationMatrix();
				descendant = ancestry;
				ancestry = (GameObject*)ParentingManager::get().GetParent(descendant);
			}

			auto mat = ancestry_matrix.m_mat;

			m_transform.m_translation = Vector3D(mat[3][0], mat[3][1], mat[3][2]);

			auto row_1 = Vector3D(mat[0][0], mat[0][1], mat[0][2]);
			auto row_2 = Vector3D(mat[1][0], mat[1][1], mat[1][2]);
			auto row_3 = Vector3D(mat[2][0], mat[2][1], mat[2][2]);

			auto scale = Vector3D(
				Vector3D::magnitude(row_1),
				Vector3D::magnitude(row_2),
				Vector3D::magnitude(row_3)
			);

			m_transform.m_scale = scale;
		

			float pitch, yaw, roll;
		
			auto parent = (GameObject*)ParentingManager::get().GetParent(this);
			GameObject* child = this;

			pitch = m_transform.m_rotation.m_x;
			yaw = m_transform.m_rotation.m_y;
			roll = m_transform.m_rotation.m_z;
			
			while (parent != nullptr) {
				pitch += parent->m_transform.m_rotation.m_x;
				yaw += parent->m_transform.m_rotation.m_y;
				roll += parent->m_transform.m_rotation.m_z;

				child = parent;
				parent = parent = (GameObject*)ParentingManager::get().GetParent(child);
			}


			m_transform.m_rotation = Vector3D(pitch, yaw, roll);
			
		}

		inline std::vector<Component*> GetComponents(){return std::vector<Component*>(components);}
		inline constant GetConstant(){return cc;}

	public:
		
		template <typename T> inline
		typename std::enable_if<std::is_base_of<Component, T>::value, T*>::type
		AddComponent() 
		{
			for (auto c : components) {
				if(typeid(*c) == typeid(T))
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

