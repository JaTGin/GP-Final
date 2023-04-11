#include "Lighting.hlsli"

cbuffer ExternalData : register(b0)
{
	matrix view;
	matrix projection;
}


VertexToPixelSkybox main( VertexShaderInput input )
{
	VertexToPixelSkybox output;

	// Remove translation from view
	matrix viewNoTranslation = view;
	viewNoTranslation._14 = 0;
	viewNoTranslation._24 = 0;
	viewNoTranslation._34 = 0;

	// view * projection
	matrix vp = mul(projection, viewNoTranslation);
	output.screenPosition = mul(vp, float4(input.localPosition, 1.0f));

	// Put the sky on the far clipping plane
	output.screenPosition.z = output.screenPosition.w;

	// Vert's position becomes the sample direction for the cube map
	output.sampleDir = input.localPosition;

	return output;
}