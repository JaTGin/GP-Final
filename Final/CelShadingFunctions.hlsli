#ifndef __GGP_CEL_SHADING__
#define __GGP_CEL_SHADING__

// Determines what part of the ramp texture we need to use based on a 0-1 n dot l value
float SampleRampTexture(float nl, Texture2D ramp, SamplerState samp) {
	return ramp.Sample(samp, float2(nl, 0)).r;
}

#endif