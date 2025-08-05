#pragma once

#include "reactphysics3d/reactphysics3d.h"
#include "ECS/Components/PhysicsComponent.h"
#include "Interfaces/Singleton.h"

#include "vector"
#include "Math/Vector3D.h"
#include "Game Engine/EngineTime.h"
#include <algorithm>

using namespace reactphysics3d;

class PhysicsComponent;
class PhysicsSystem : public Singleton<PhysicsSystem>
{
	public: 
		static void Initialize();
		static void AddPhysicsComponent(PhysicsComponent* component);
		static void RemovePhysicsComponent(PhysicsComponent* component);
		static std::vector<PhysicsComponent*> GetAllPhysicsComponents();
		static void Release();
	
		static void UpdateAllPhysicsComponents();
		static PhysicsWorld* GetPhysicsWorld();
		static PhysicsCommon* GetPhysicsCommon();
	
	private:
		std::vector<PhysicsComponent*> physicsComponentList;
		PhysicsWorld* physicsWorld = nullptr;
		PhysicsCommon* physicsCommon = nullptr;
};

