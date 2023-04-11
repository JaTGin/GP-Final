#include "Lighting.hlsli"
#include "CelShadingFunctions.hlsli"

#define LIGHT_COUNT 5

// Input for color
cbuffer ExternalData : register(b0)
{
	float3 colorTint;
	float3 cameraPosition;
	float2 uvScale;
	float2 uvOffset;
	float3 ambient;
	int lightCount;
	Light lights[LIGHT_COUNT];
}

Texture2D Albedo			: register(t0);
Texture2D NormalMap			: register(t1);
Texture2D RoughnessMap		: register(t2);
Texture2D CelRamp			: register(t3);
Texture2D CelRampSpec		: register(t4);

SamplerState Sampler		: register(s0);
SamplerState Clamp			: register(s1);

struct output {
	float4 color			: SV_TARGET0;
	float4 normals			: SV_TARGET1;
	float4 depth			: SV_TARGET2;
};

// Main
output main(VertexToPixel input)
{
	// Clean up and map normals, adjust uv
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);
	input.uv = input.uv * uvScale + uvOffset;
	input.normal = NormalMapping(NormalMap, Sampler, input.uv, input.normal, input.tangent);

	float roughness = RoughnessMap.Sample(Sampler, input.uv).r;

	// Calculate the color of the surface
	float3 surfaceColor = pow(Albedo.Sample(Sampler, input.uv).rgb, 2.2f);

	// Add the specular map to scale lighting
	// float3 specScalar = SpecularMap.Sample(Sampler, input.uv).r;

	float3 finalColor = ambient * surfaceColor;

	for (int i = 0; i < lightCount; i++) {
		Light light = lights[i];
		light.Direction = normalize(light.Direction);

		float3 toLight = float3(0.0f, 0.0f, 0.0f);
		float3 toCamera = normalize(cameraPosition - input.worldPosition);
		float attenuation = 1.0f;

		// Calculate lighting based on type of light, then add that light's effect to the final color
		switch (light.Type) {
		case LIGHT_TYPE_DIRECTIONAL:
			toLight = normalize(-light.Direction);
			break;
		case LIGHT_TYPE_POINT:
			toLight = normalize(light.Position - input.worldPosition);
			attenuation = Attenuate(light, input.worldPosition);
			break;
		}

		float diffuse = Diffuse(input.normal, toLight);
		diffuse = SampleRampTexture(diffuse, CelRamp, Clamp);
		float spec = SpecPhong(toLight, toCamera, roughness, input.normal);
		spec = SampleRampTexture(spec, CelRampSpec, Clamp);

		finalColor += (diffuse * surfaceColor + spec) * light.Color * light.Intensity * attenuation;

	}

	output outputVals;
	outputVals.color = float4(pow(finalColor, 1.0f / 2.2f), 1);
	outputVals.normals = float4 (input.normal, 0);
	outputVals.depth = input.screenPosition.z;
	return outputVals;
}