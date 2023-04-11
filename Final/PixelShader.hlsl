#include "Lighting.hlsli"

#define LIGHT_COUNT 5

// Input for color
cbuffer ExternalData : register(b0)
{
	float roughness;
	float3 colorTint;
	float3 cameraPosition;
	float2 uvScale;
	float2 uvOffset;
	float3 ambient;
	Light lights[LIGHT_COUNT];
}

Texture2D Albedo			: register(t0);
Texture2D NormalMap			: register(t1);
Texture2D RoughnessMap		: register(t2);
Texture2D MetalnessMap		: register(t3);

SamplerState Sampler	: register(s0);

// Main
float4 main(VertexToPixel input) : SV_TARGET
{
	
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);
	input.uv = input.uv * uvScale + uvOffset;

	input.normal = NormalMapping(NormalMap, Sampler, input.uv, input.normal, input.tangent);

	float roughness = RoughnessMap.Sample(Sampler, input.uv).r;

	float metalness = MetalnessMap.Sample(Sampler, input.uv).r;

	// Calculate the color of the surface
	float3 surfaceColor = pow(Albedo.Sample(Sampler, input.uv).rgb, 2.2f);

	float3 specularColor = lerp(F0_NON_METAL.rrr, surfaceColor.rgb, metalness);

	// Add the specular map to scale lighting
	// float3 specScalar = SpecularMap.Sample(Sampler, input.uv).r;

	float3 finalColor = ambient * surfaceColor;

	for (int i = 0; i < LIGHT_COUNT; i++) {
		Light light = lights[i];
		light.Direction = normalize(light.Direction);

		// Calculate lighting based on type of light, then add that light's effect to the final color
		switch (lights[i].Type) {
		case LIGHT_TYPE_DIRECTIONAL:
			finalColor += CalculateDirectional(light, cameraPosition, input.worldPosition, input.normal, roughness, metalness, surfaceColor, specularColor);
			break;
		case LIGHT_TYPE_POINT:
			finalColor += CalculatePoint(light, cameraPosition, input.worldPosition, input.normal, roughness, metalness, surfaceColor, specularColor);
			break;
		}
		
	}
	return float4(pow(finalColor, 1.0f / 2.2f), 1);
}