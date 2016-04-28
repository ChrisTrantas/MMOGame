struct SpotLight
{
	matrix view;
	matrix projection;
	float3 direction;
	float fov;
	float3 position;
	float range;
};

cbuffer externalData : register(b0)
{
	matrix world;
	SpotLight spotLight;
};

struct VertexShaderInput
{
	float3 position		: POSITION;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float2 uv			: TEXCOORD;
};

struct VertexToPixel
{
	float4 position		: SV_POSITION;
};

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	matrix wvp = mul(world, mul(spotLight.view, spotLight.projection));
	output.position = mul(float4(input.position, 1.0f), wvp);

	return output;
}