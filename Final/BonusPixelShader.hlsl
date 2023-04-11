#include "Lighting.hlsli"

// Input for color
cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	float time;
}

// Main
// Creating a 'beach ball' pattern for a sphere
float4 main(VertexToPixel input) : SV_TARGET
{
	// Establish two colors based on proximity to sin
	float4 outputColorOver = float4(1.0f, 0.73f, 0.0f, 0);
	float4 outputColorUnder = float4(1.0f, 1.0f, 1.0f, 1.0f);
	
	// Create a sin wave
	float heightScale = 10.0f;
	float v = (input.uv.y * -2 + 1) * heightScale;
	float speed = 10.0f;
	float frequency = 3.0f;
	float amplitude = 3.0f;
	float3 total = 0.0f;
	const float PI = 3.14159f;

	float f = frequency * PI * 2.0f;
	float s = sin(input.uv.x * f + time * speed + sin(time)) * amplitude;

	// Calculate location relative to sin wave
	float dist = 1.0f - saturate(abs(v - s));

	// Set a different color based on whether or not we are 'above' the wave
	if (dist <= s)
	{
		return float4(outputColorUnder);
	}
	else
	{
		return float4(outputColorOver);
	}
}