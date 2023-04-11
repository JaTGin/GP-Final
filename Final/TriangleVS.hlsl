// The data getting passed to the pixel shader
// This struct shaves a lot of stuff from the one in lighting.hlsli since we don't need it
struct VertexToPixel {
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD0;
};

// Calculates the UV and position based on the ID of the vertex
VertexToPixel main(uint id : SV_VERTEXID)
{
	VertexToPixel output;

	output.uv = float2((id << 1) & 2, id & 2);
	output.position = float4(output.uv, 0, 1);
	output.position.x = output.position.x * 2 - 1;
	output.position.y = output.position.y * -2 + 1;

	return output;
}