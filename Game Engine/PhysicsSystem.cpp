#include "ECS/Systems/PhysicsSystem.h"
#include "ECS/Entities/Entity.h"

void PhysicsSystem::Initialize()
{
    get().physicsCommon = new PhysicsCommon();
    PhysicsWorld::WorldSettings settings;
    settings.defaultVelocitySolverNbIterations = 50;
    settings.gravity = Vector3(0, -9.81, 0);
    get().physicsWorld = get().physicsCommon->createPhysicsWorld(settings);
   // std::cout << "Created Physics System";
}

void PhysicsSystem::AddPhysicsComponent(PhysicsComponent* component)
{
    get().physicsComponentList.push_back(component);
}

void PhysicsSystem::RemovePhysicsComponent(PhysicsComponent* component)
{
    int index = -1;
    for (int i = 0; i < get().physicsComponentList.size(); i++) {
        if (component == get().physicsComponentList[i]) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        get().physicsComponentList.erase(get().physicsComponentList.begin() + index);
    }
}

std::vector<PhysicsComponent*> PhysicsSystem::GetAllPhysicsComponents()
{
    return get().physicsComponentList;
}

void PhysicsSystem::Release()
{
    delete get().physicsCommon;
}

void PhysicsSystem::UpdateAllPhysicsComponents()
{
    if (EngineTime::deltaTime() > 0.0f) {
        get().physicsWorld->update(EngineTime::deltaTime());
        for (PhysicsComponent* pc : get().physicsComponentList) {
            pc->Update(pc->owner->cc);
           // pc->Perform(EngineTime::deltaTime());
        }
    }
}

PhysicsWorld* PhysicsSystem::GetPhysicsWorld()
{
    return get().physicsWorld;
}

PhysicsCommon* PhysicsSystem::GetPhysicsCommon()
{
    return get().physicsCommon;
}

//Quaternion PhysicsSystem::EulerToQuaternion(Vector3D euler)
//{
//    double cy = cos(euler.m_x * 0.5);
//    double sy = sin(euler.m_x * 0.5);
//    double cp = cos(euler.m_y * 0.5);
//    double sp = sin(euler.m_y * 0.5);
//    double cr = cos(euler.m_z * 0.5);
//    double sr = sin(euler.m_z * 0.5);
//
//    Quaternion q;
//    q.w = cr * cp * cy + sr * sp * sy;
//    q.x = sr * cp * cy - cr * sp * sy;
//    q.y = cr * sp * cy + sr * cp * sy;
//    q.z = cr * cp * sy - sr * sp * cy;
//
//    return q;
//}
//
//Vector3D PhysicsSystem::QuaternionToEuler(Quaternion q)
//{
//    double w = q.w; double x = q.x;  double y = q.y;  double z = q.z;
//    double roll = atan2(2 * (w * x + y * z), 1 - 2 * (x * x + y * y));
//    //double temp = 2 * (w * y - z * x);
//    double pitch = std::asin(std::clamp(2 * (w * y - z * x), -1.0, 1.0));
//    //double pitch = asin(2 * (w * y - z * x));
//    double yaw = atan2(2 * (w * z + x * y), 1 - 2 * (y * y + z * z));
//
//    //value = std::max(-1.0, std::min(1.0, value));
//}
