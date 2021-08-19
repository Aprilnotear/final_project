#include "Camera.h"
#include "../nclgl/Window.h"
#include <iostream>

using namespace T2CAMERA;

void Camera::UpdateCamera(float dt) {
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);

	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw < 0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));

	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);

	float speed = 90.0f * dt;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		position += forward * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		position -= forward * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		position -= right * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		position += right * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
		position.y += speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		position.y -= speed;
	}
}

Matrix4 Camera::BuildViewMatrix() {
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-position);
}


void Camera::AutoMove(float dt) {
	yaw -= 1.5* dt;

	float speed = 60.0f * dt;

	if (position.y > 800 && int(position.x) == 2400 && int(position.z) == 2400) {
		position.y -= speed;
		pitch += dt;
	}
	if (position.x >= 700 && int(position.y+1) == 800 && int(position.z) == 2400) {
		position.x -= speed;
		yaw -= dt;
	}
	if (position.z >= 700 && int(position.x+1) == 700 && int(position.y+1) == 800) {
		position.z -= speed;
		yaw -= dt;
	}
	if (position.y <= 1500 && int(position.x+1) == 700 && int(position.z+1) == 700) {
		position.y += speed;
		yaw -= dt;
	}
	if (position.x <= 2400 && int(position.z+1) == 700 && int(position.y) == 1500) {
		position.x += speed;
		yaw -= dt;
	}
	if (position.z <= 2400 && int(position.x) == 2400 && int(position.y) == 1500) {
		position.z += speed;
		yaw -= dt;
	}
}