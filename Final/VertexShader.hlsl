#include "Lighting.hlsli"

// Input for m, v, p mats
cbuffer ExternalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	matrix worldInvTranspose;
}



// Main
VertexToPixel main(VertexShaderInput input)
{
	
	VertexToPixel output;

	matrix wvp = mul(projection, mul(view, world));
	output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
	output.uv = input.uv;
	output.normal = mul((float3x3)worldInvTranspose, input.normal);
	output.tangent = mul((float3x3)world, input.tangent);
	output.worldPosition = mul(world, float4(input.localPosition, 1.0f)).xyz;

	return output;
}