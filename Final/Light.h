#pragma once

#include <DirectXMath.h>

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

using namespace DirectX;

struct Light
{
	int Type;
	XMFLOAT3 Direction;

	float Range;
	XMFLOAT3 Position;
	
	float Intensity;
	XMFLOAT3 Color;
	
	float SpotFalloff;
	XMFLOAT3 Padding;
};

