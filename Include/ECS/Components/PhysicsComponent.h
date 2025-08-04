#pragma once
#include "reactphysics3d/reactphysics3d.h"
#include "ECS/Systems/PhysicsSystem.h"
#include "ECS/Components/Component.h"
//#include "ECS/Entities/Entity.h"

using namespace reactphysics3d;
class PhysicsComponent : public SceneEditor::Component
{
	public:
		PhysicsComponent();
	
		void Update(constant cc) override;
		//void Perform(float deltaTime);
		void Release() override;
		RigidBody* getRigidBody();

	private:
		Quaternion EulerToQuaternion(Vector3D euler);
		Vector3D QuaternionToEuler(Quaternion q);
		auto ConvertPosition(auto pos);
		//auto Position(auto pos);
	
	private:
		float mass = 1000.0f;
		RigidBody* rigidBody;
	
};

