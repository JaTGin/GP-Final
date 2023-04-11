#pragma once
#include <DirectXMath.h>

class Transform
{
private:
	DirectX::XMFLOAT4X4 worldMatrix, worldInverseTransposeMatrix;
	DirectX::XMFLOAT3 position, scale, rotation, up, right, forward;
	bool dirtyMat, dirtyVec;

	void UpdateMatrices();
	void UpdateVectors();
public:
	Transform();

	void SetPosition(float x, float y, float z);
	void SetRotation(float pitch, float yaw, float roll);
	void SetScale(float x, float y, float z);

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetPitchYawRoll();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetForward();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();

	void MoveAbsolute(float x, float y, float z);
	void MoveRelative(float x, float y, float z);
	void Rotate(float pitch, float yaw, float roll);
	void Scale(float x, float y, float z);
};

