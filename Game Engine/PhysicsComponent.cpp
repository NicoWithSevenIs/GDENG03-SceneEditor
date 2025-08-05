#include "ECS/Components/PhysicsComponent.h"
#include "ECS/Entities/Entity.h"
#include "Game Engine/EngineTime.h"

PhysicsComponent::PhysicsComponent(BodyType type) : Component(SceneEditor::ComponentType::PHYSICS)
{ 
    this->bodyType = type;
    this->rigidBody = nullptr;
	PhysicsSystem::AddPhysicsComponent(this);
}

void PhysicsComponent::Update(constant cc)
{
    if (this->rigidBody->getType() == BodyType::DYNAMIC) {
        Transform transform = this->rigidBody->getTransform();
        Vector3 position = transform.getPosition();
        Quaternion quaternion = transform.getOrientation();
        this->owner->m_transform.m_translation = Vector3D(position.x, position.y, position.z);
        this->owner->m_transform.m_rotation = Vector3D(QuaternionToEuler(quaternion));
    }
}

void PhysicsComponent::Release()
{
	PhysicsSystem::RemovePhysicsComponent(this);
}

void PhysicsComponent::Init()
{ 
    PhysicsCommon* physicsCommon = PhysicsSystem::GetPhysicsCommon();
    PhysicsWorld* physicsWorld = PhysicsSystem::GetPhysicsWorld();

    //create rigid body
    Vector3D scale = this->owner->m_transform.m_scale;
    Vector3D posTemp = this->owner->m_transform.m_translation;
    Vector3 position = Vector3(posTemp.m_x, posTemp.m_y, posTemp.m_z);
    Quaternion quaternion = EulerToQuaternion(this->owner->m_transform.m_rotation);

    Transform transform(position, quaternion);
    BoxShape* boxShape = physicsCommon->createBoxShape(Vector3(scale.m_x / 2, scale.m_y / 2, scale.m_z / 2));

    this->rigidBody = physicsWorld->createRigidBody(transform);
    this->rigidBody->setType(this->bodyType);
    this->rigidBody->addCollider(boxShape, Transform::identity());

    if (this->bodyType == BodyType::DYNAMIC) {
        this->rigidBody->updateMassPropertiesFromColliders();
        this->rigidBody->setMass(this->mass);
    }
}

RigidBody* PhysicsComponent::GetRigidBody()
{
	return this->rigidBody;
}

void PhysicsComponent::SetBodyType(BodyType type)
{
    this->bodyType = type;
}

Quaternion PhysicsComponent::EulerToQuaternion(Vector3D euler)
{
    double cy = cos(euler.m_x * 0.5);
    double sy = sin(euler.m_x * 0.5);
    double cp = cos(euler.m_y * 0.5);
    double sp = sin(euler.m_y * 0.5);
    double cr = cos(euler.m_z * 0.5);
    double sr = sin(euler.m_z * 0.5);

    Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;

    return q;
}

Vector3D PhysicsComponent::QuaternionToEuler(Quaternion q)
{
    double w = q.w; double x = q.x;  double y = q.y;  double z = q.z;
    double roll = atan2(2 * (w * x + y * z), 1 - 2 * (x * x + y * y));
    double pitch = std::asin(std::clamp(2 * (w * y - z * x), -1.0, 1.0));
    double yaw = atan2(2 * (w * z + x * y), 1 - 2 * (y * y + z * z));

    return Vector3D(roll, pitch, yaw);
}