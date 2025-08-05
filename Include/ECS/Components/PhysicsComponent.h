#pragma once
#include "reactphysics3d/reactphysics3d.h"
#include "ECS/Systems/PhysicsSystem.h"
#include "ECS/Components/Component.h"
//#include "ECS/Entities/Entity.h"

using namespace reactphysics3d;
class PhysicsComponent : public SceneEditor::Component
{
	public:
		PhysicsComponent(BodyType type);
	
		void Update(constant cc) override;
		void Release() override;
		void Init() override;
		RigidBody* GetRigidBody();
		void SetBodyType(BodyType type);

	private:
		Quaternion EulerToQuaternion(Vector3D euler);
		Vector3D QuaternionToEuler(Quaternion q);
	
	private:
		float mass = 1000.0f;
		RigidBody* rigidBody;
		BodyType bodyType = BodyType::STATIC;
	
};

