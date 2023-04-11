#include "Transform.h"

using namespace DirectX;

// ctor
Transform::Transform() :
	position(0, 0, 0),
	scale(1, 1, 1),
	rotation(0, 0, 0),
	up(0, 1, 0),
	right(1, 0, 0),
	forward(0, 0, 1),
	dirtyMat(false),
	dirtyVec(false)
{
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixIdentity());
}

// Getters
// Matric getters are updating the matrices
DirectX::XMFLOAT3 Transform::GetPosition() {
	return position;
}
DirectX::XMFLOAT3 Transform::GetPitchYawRoll() {
	return rotation;
}
DirectX::XMFLOAT3 Transform::GetScale() {
	return scale;
}
DirectX::XMFLOAT3 Transform::GetUp() {
	UpdateVectors();
	return up;
}
DirectX::XMFLOAT3 Transform::GetRight() {
	UpdateVectors();
	return right;
}
DirectX::XMFLOAT3 Transform::GetForward() {
	UpdateVectors();
	return forward;
}
DirectX::XMFLOAT4X4 Transform::GetWorldMatrix() {
	UpdateMatrices();
	return worldMatrix;
}
DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix() {
	UpdateMatrices();
	return worldInverseTransposeMatrix;
}

// Setters
void Transform::SetPosition(float x, float y, float z) {
	position.x = x;
	position.y = y;
	position.z = z;
	dirtyMat = true;
}
void Transform::SetRotation(float x, float y, float z) {
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
	dirtyMat = true;
	dirtyVec = true;
}
void Transform::SetScale(float x, float y, float z) {
	scale.x = x;
	scale.y = y;
	scale.z = z;
	dirtyMat = true;
}

// Transform functions- these are just adding the inputted values to the transform vectors
void Transform::MoveAbsolute(float x, float y, float z) {
	position.x += x;
	position.y += y;
	position.z += z;
	dirtyMat = true;
}
void Transform::MoveRelative(float x, float y, float z) {
	XMVECTOR moveCoords = XMVectorSet(x, y, z, 0);
	XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation));
	XMVECTOR completeRotation = XMVector3Rotate(moveCoords, rotationQuat);
	XMStoreFloat3(&position, XMLoadFloat3(&position) + completeRotation);
	dirtyMat = true;

}
void Transform::Rotate(float pitch, float yaw, float roll) {
	rotation.x += pitch;
	rotation.y += yaw;
	rotation.z += roll;

	dirtyMat = true;
	dirtyVec = true;
}
void Transform::Scale(float x, float y, float z) {
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
	dirtyMat = true;
}

// Updating the matrices as a helper function called at EOF as suggested for optimization
void Transform::UpdateMatrices() {
	if (dirtyMat) {
		XMMATRIX translationMat = XMMatrixTranslation(position.x, position.y, position.z);
		XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
		XMMATRIX scaleMat = XMMatrixScaling(scale.x, scale.y, scale.z);

		XMMATRIX world = scaleMat * rotationMat * translationMat;

		XMStoreFloat4x4(&worldMatrix, world);
		XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(world)));
	}
}
void Transform::UpdateVectors() {
	if (dirtyVec) {
		XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation));
		XMStoreFloat3(&up, XMVector3Rotate(XMVectorSet(0, 1, 0, 0), rotationQuat));
		XMStoreFloat3(&right, XMVector3Rotate(XMVectorSet(1, 0, 0, 0), rotationQuat));
		XMStoreFloat3(&forward, XMVector3Rotate(XMVectorSet(0, 0, 1, 0), rotationQuat));
		dirtyVec = false;
	}
}