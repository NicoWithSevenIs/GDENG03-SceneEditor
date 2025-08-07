#pragma once
#include "reactphysics3d/reactphysics3d.h"

#include "ECS/Components/Component.h"

class PhysicsSystem;

class PhysicsComponent : public Component
{
	public:
		PhysicsComponent(reactphysics3d::BodyType type);
	
		void Update(constant cc) override;
		void Release() override;
		Component* CreateSnapshot() override;
		
		reactphysics3d::RigidBody* GetRigidBody();
		void SetBodyType(reactphysics3d::BodyType type);
		void Init() override;

         void ExportToUnity(std::string& output) override;


	private:
		reactphysics3d::Quaternion EulerToQuaternion(Vector3D euler);
		Vector3D QuaternionToEuler(reactphysics3d::Quaternion q);
	
	private:
		float mass = 1000.0f;
		reactphysics3d::RigidBody* rigidBody;
		reactphysics3d::BodyType bodyType = reactphysics3d::BodyType::STATIC;


	
};

