#include "Camera.h"
#include "Input.h"

using namespace DirectX;

Camera::Camera(
	float x, 
	float y, 
	float z, 
	float aspectRatio, 
	float moveSpeed,
	float mouseSensitivity,
	float fov,
	float nearClip,
	float farClip,
	bool isPerspective) :
	aspectRatio(aspectRatio),
	moveSpeed(moveSpeed),
	mouseSensitivity(mouseSensitivity),
	fov(fov),
	nearClip(nearClip),
	farClip(farClip),
	isPerspective(isPerspective)
{
	transform.SetPosition(x, y, z);
	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

XMFLOAT4X4 Camera::GetView() {
	return view;
}
XMFLOAT4X4 Camera::GetProjection() {
	return projection;
}
Transform* Camera::GetTransform() {
	return &transform;
}

void Camera::UpdateProjectionMatrix(float aspectRatio) {
	this->aspectRatio = aspectRatio;
	XMMATRIX projMat;

	if (isPerspective) {
		projMat = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClip, farClip);
	}

	XMStoreFloat4x4(&projection, projMat);
}

void Camera::UpdateViewMatrix() {
	XMFLOAT3 fwd = transform.GetForward();
	XMFLOAT3 pos = transform.GetPosition();

	XMMATRIX viewMat = XMMatrixLookToLH(XMLoadFloat3(&pos), XMLoadFloat3(&fwd), XMVectorSet(0, 1, 0, 0));
	XMStoreFloat4x4(&view, viewMat);
}

void Camera::Update(float dt) {
	float dist = dt * moveSpeed;

	Input& input = Input::GetInstance();

	if (input.KeyDown('W')) transform.MoveRelative(0, 0, dist);
	if (input.KeyDown('S')) transform.MoveRelative(0, 0, -dist);
	if (input.KeyDown('A')) transform.MoveRelative(-dist, 0, 0);
	if (input.KeyDown('D')) transform.MoveRelative(dist, 0, 0);
	if (input.KeyDown(VK_CONTROL)) { transform.MoveAbsolute(0, -dist, 0); }
	if (input.KeyDown(VK_SPACE)) { transform.MoveAbsolute(0, dist, 0); }

	if (input.MouseLeftDown())
	{
		float xDiff = dt * mouseSensitivity * input.GetMouseXDelta();
		float yDiff = dt * mouseSensitivity * input.GetMouseYDelta();
		transform.Rotate(yDiff, xDiff, 0);

		XMFLOAT3 clampedRotation = transform.GetPitchYawRoll();
		if (clampedRotation.x > XM_PI / 2) clampedRotation.x = XM_PI / 2;
		if (clampedRotation.x < -XM_PI / 2) clampedRotation.x = -XM_PI / 2;
		
		transform.SetRotation(clampedRotation.x, clampedRotation.y, clampedRotation.z);
	}

	UpdateViewMatrix();
}
