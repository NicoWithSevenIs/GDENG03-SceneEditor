#include "ECS/Components/PhysicsComponent.h"
#include "ECS/Entities/Entity.h"
#include "Game Engine/EngineTime.h"

PhysicsComponent::PhysicsComponent() : Component(SceneEditor::ComponentType::PHYSICS)
{ 
	PhysicsSystem::AddPhysicsComponent(this);
	PhysicsCommon* physicsCommon = PhysicsSystem::GetPhysicsCommon();
	PhysicsWorld* physicsWorld = PhysicsSystem::GetPhysicsWorld();

    ////reactphysics3d::Transform transform = transform.setFromOpenGL(this->owner->getP)

	//create rigid body
	Vector3D scale = this->owner->m_transform.m_scale;
	Vector3D posTemp = this->owner->m_transform.m_translation;
	Vector3 pos3 = Vector3(posTemp.m_x, posTemp.m_y, posTemp.m_z);
    Quaternion quaternion = EulerToQuaternion(this->owner->m_transform.m_rotation);
    Transform transform; transform.setPosition(pos3); transform.setOrientation(quaternion);
    BoxShape* boxShape = physicsCommon->createBoxShape(Vector3(scale.m_x / 2, scale.m_y / 2, scale.m_z / 2));
    this->rigidBody = physicsWorld->createRigidBody(transform);
    this->rigidBody->addCollider(boxShape, transform);
    this->rigidBody->updateMassPropertiesFromColliders();
    this->rigidBody->setMass(this->mass);
    this->rigidBody->setType(BodyType::DYNAMIC);

    transform = this->rigidBody->getTransform();
    pos3 = transform.getPosition();
    quaternion = transform.getOrientation();
    this->owner->m_transform.m_translation = Vector3D(pos3.x, pos3.y, pos3.z);
    this->owner->m_transform.m_rotation = Vector3D(QuaternionToEuler(quaternion));

    //Perform()
}

void PhysicsComponent::Update(constant cc)
{
    Transform transform = this->rigidBody->getTransform();
    Vector3 position = transform.getPosition();
    Quaternion quaternion = transform.getOrientation();
    this->owner->m_transform.m_translation = Vector3D(position.x, position.y, position.z);
    this->owner->m_transform.m_rotation = Vector3D(QuaternionToEuler(quaternion));
}

//void PhysicsComponent::Perform(float deltaTime)
//{
//    //Transform transform = this->rigidBody->getTransform();
//    //Vector3 position = transform.getPosition();
//    //Quaternion quaternion = transform.getOrientation();
//    //this->owner->m_transform.m_translation = Vector3D(position.x, position.y, position.z);
//    //this->owner->m_transform.m_rotation = Vector3D(QuaternionToEuler(quaternion));
//}

void PhysicsComponent::Release()
{
	PhysicsSystem::RemovePhysicsComponent(this);
}

RigidBody* PhysicsComponent::getRigidBody()
{
	return this->rigidBody;
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
    //double temp = 2 * (w * y - z * x);
    double pitch = std::asin(std::clamp(2 * (w * y - z * x), -1.0, 1.0));
    //double pitch = asin(2 * (w * y - z * x));
    double yaw = atan2(2 * (w * z + x * y), 1 - 2 * (y * y + z * z));

    //value = std::max(-1.0, std::min(1.0, value));
}

//auto PhysicsComponent::ConvertPosition(auto pos)
//{
//
//}
