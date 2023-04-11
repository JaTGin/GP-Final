#include "Lighting.hlsli"

TextureCube SkyTexture		: register(t0);
SamplerState Sampler		: register(s0);

float4 main(VertexToPixelSkybox input) : SV_TARGET
{
	
	return SkyTexture.Sample(Sampler, input.sampleDir);
}