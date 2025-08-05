#define NOMINMAX

#include "Window/AppWindow.h"
#include <Windows.h>

#include "Math/Vector3D.h"
#include "Math/Matrix4x4.h"
#include "Input/InputSystem.h"

#include <iostream>
#include <utility>
#include <cstdlib>
#include <algorithm>
#include <DirectXMath.h>
#include <corecrt_math_defines.h>

#include "Game Engine/EngineTime.h"
#include "GameObject/ParentingManager.h"
#include "ECS/Systems/EntityManager.h"
#include "ECS/Systems/TimelineManager.h"

#include "ECS/Components/CubeRenderer.h"

AppWindow::AppWindow()
{
}

AppWindow::~AppWindow()
{
}

void AppWindow::OnCreate()
{
	Window::OnCreate();

	srand(time(0));

	GraphicsEngine::get()->init();
	this->m_swap_chain = GraphicsEngine::get()->createSwapChain();

	RECT rc = this->getClientWindowRect();

	float width = rc.right - rc.left;
	float height = rc.bottom - rc.top;

	this->m_swap_chain->init(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);
	
	pc = new PerspectiveCamera(1.57f, width/height);
	pc->m_transform.m_translation = Vector3D(0, 2, -3);

	UIManager::initialize(this->m_hwnd, GraphicsEngine::get()->getDevice(), GraphicsEngine::get()->getImmediateDeviceContext()->getContext());
	EntityManager::Initialize();
	TimelineManager::get().CreateSnapshot();
}


void AppWindow::OnUpdate()
{
	Window::OnUpdate();

	
	InputSystem::get()->Update();
	//InputSystem::get()->ShowCursor(false);

	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,  0, 0.3f, 0.4f, 1);

	RECT rc = this->getClientWindowRect();
	float width = rc.right - rc.left;
	float height = rc.bottom - rc.top;

	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	

	pc->Update();

	EntityManager::ResetUpdatedFlags();
	EntityManager::Update(pc->GetViewMatrix(), pc->GetProjectionMatrix());
	EntityManager::Draw();

	UIManager::draw();

	this->m_swap_chain->present(true);

	if (TimelineManager::get().IsDirty()) {
		TimelineManager::get().CreateSnapshot();
	}

}



void AppWindow::OnDestroy()
{
	Window::OnDestroy();
	this->m_swap_chain->release();

	EntityManager::Release();

	GraphicsEngine::get()->release();
}

void AppWindow::OnFocus()
{
	std::cout << "Gained Focus\n";
	InputSystem::get()->AddListener(this);
}

void AppWindow::OnKillFocus()
{
	std::cout << "Lost Focus\n";
	InputSystem::get()->RemoveListener(this);
	InputSystem::get()->first = true;
}

void AppWindow::onKeyDown(int key)
{
	pc->OnKeyDown(key);

	if(key == VK_LCONTROL)
		is_ctrl_held = true;
	
}

void AppWindow::onKeyUp(int key)
{
	pc->OnKeyUp(key);

	if (key == '1') {
		toggle_camera_movement = false;
	}

	if (key == '2') {
		toggle_camera_movement = true;
	}

	if (key == VK_LCONTROL)
		is_ctrl_held = false;

	if (is_ctrl_held && key == 'Z') {
		std::cout << "Undo" << std::endl;
		TimelineManager::get().Undo();
	}

	if (is_ctrl_held && key == 'Y') {
		std::cout << "Redo" << std::endl;
		TimelineManager::get().Redo();
	}
}

void AppWindow::onMouseMove(const Point& delta_mouse_point,
	const Point& mouse_pos)
{
	if(toggle_camera_movement)
		pc->OnMouseMove(delta_mouse_point.m_x, delta_mouse_point.m_y);
}

void AppWindow::onLeftMouseDown(const Point& delta_mouse_point)
{
}

void AppWindow::onLeftMouseUp(const Point& delta_mouse_point)
{
}


void AppWindow::onRightMouseDown(const Point& delta_mouse_point)
{
	
}

void AppWindow::onRightMouseUp(const Point& delta_mouse_point)
{
	
}

