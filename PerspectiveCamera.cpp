#include "PerspectiveCamera.h"
#include "Time.h"

PerspectiveCamera::PerspectiveCamera(float fov, float aspect_ratio): 
	Camera(), m_fov(fov), m_aspect_ratio(aspect_ratio)
{
}

Matrix4x4 PerspectiveCamera::GetProjectionMatrix()
{
    Matrix4x4 proj;
    proj.setPerspectiveFovLH(m_fov, m_aspect_ratio, 0.1f, 100.f);
    return proj;
}

void PerspectiveCamera::Update()
{
	Matrix4x4 camera_matrix = this->m_transform.GetTransformationMatrix();
	Vector3D forward = camera_matrix.getLocalZDirection() * Time::deltaTime() * dir;
	Vector3D right = camera_matrix.getLocalXDirection() * Time::deltaTime() * r;

	this->m_transform.m_translation = this->m_transform.m_translation + 
		forward + right + Vector3D(0, Time::deltaTime() * up, 0);
	this->m_transform.m_rotation = Vector3D(xRot, yRot, 0);
}

void PerspectiveCamera::OnKeyDown(int key)
{
	switch (key) {
		case 'W': dir = 1.f; break;
		case 'S': dir = -1.f;  break;

		case 'A': r = -1.f; break;
		case 'D': r = 1.f;  break;

		case 'Q': up = -1.f; break;
		case 'E':  up = 1.f;  break;
	}
}

void PerspectiveCamera::OnKeyUp(int key)
{
	switch (key) {
		case 'W':
		case 'S': dir = 0.f;  break;
		case 'A':
		case 'D': r = 0.f;  break;
		case 'Q':
		case 'E': up = 0.f;  break;
	}
}

void PerspectiveCamera::OnMouseMove(float x, float y)
{
	xRot -= y * Time::deltaTime() * 0.1f;
	yRot -= x * Time::deltaTime() * 0.1f;
}

