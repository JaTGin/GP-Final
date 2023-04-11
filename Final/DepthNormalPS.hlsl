// Inputs
cbuffer ExternalData : register(b0) {
	float width;
	float height;
	float normal;
	float depth;
};

struct VertexToPixel {
	float4 position		: SV_POSITION;
	float2 uv           : TEXCOORD0;
};

// Textures
Texture2D Pixels		: register(t0);
Texture2D Normals		: register(t1);
Texture2D Depth			: register(t2);

// Samplers
SamplerState Sampler	: register(s0);

// In main, we do the work of detecting large changes between the depth or normal values of this pixel and pixels directly adjacent it.
// If a large change has been detected, that means there is probably an outline!
float4 main(VertexToPixel input) : SV_TARGET{
	// Grab pixels 
	float2 left = float2(-width, 0);
	float2 right = float2(width, 0);
	float2 up = float2(0, height);
	float2 down = float2(0, -height);

	// Grab the depth values of every adjacent pixel, then calculate the change between them
	float currentDepth = Depth.Sample(Sampler, input.uv).r;
	float lDepth = Depth.Sample(Sampler, input.uv + left).r;
	float rDepth = Depth.Sample(Sampler, input.uv + right).r;
	float uDepth = Depth.Sample(Sampler, input.uv + up).r;
	float dDepth = Depth.Sample(Sampler, input.uv + down).r;
	float depthChange = abs(currentDepth - lDepth) + abs(currentDepth - rDepth) + abs(currentDepth - uDepth) + abs(currentDepth - dDepth);
	float totalDepth = pow(saturate(depthChange), depth);

	// Perform the same operations for normals
	float3 currentNormals = Normals.Sample(Sampler, input.uv).rgb;
	float3 lNormals = Normals.Sample(Sampler, input.uv + left).rgb;
	float3 rNormals = Normals.Sample(Sampler, input.uv + right).rgb;
	float3 uNormals = Normals.Sample(Sampler, input.uv + up).rgb;
	float3 dNormals = Normals.Sample(Sampler, input.uv + down).rgb;
	float3 normalChange = abs(currentNormals - lNormals) + abs(currentNormals - rNormals) + abs(currentNormals - uNormals) + abs(currentNormals - dNormals);
	float totalNormals = pow(saturate(normalChange.x + normalChange.y + normalChange.z), normal);

	// Check which of the two totals is larger
	// Since we're looking for rapid changes in these values, bigger denotes higher impact
	float outline = max(totalDepth, totalNormals);

	// Grab the color of this pixel
	float3 color = Pixels.Sample(Sampler, input.uv).rgb;

	// Lerp between the pixel color and outline value and return it- if a big change is picked up in outline, it will darken this pixel, drawing an outline to the screen
	float3 outputColor = lerp(color, float3(0.0f, 0.0f, 0.0f), outline);
	return float4(outputColor, 1);
}