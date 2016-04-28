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
	matrix view;
	matrix projection;
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
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float2 uv			: TEXCOORD0;
	float3 worldPos		: TEXCOORD1;
	float4 shadowPos	: TEXCOORD2;
};

VertexToPixel main( VertexShaderInput input )
{
	VertexToPixel output;
	
	matrix worldViewProj = mul(mul(world, view), projection);
	output.position = mul(float4(input.position, 1.0f), worldViewProj);

	output.normal = mul(input.normal, (float3x3)world);
	output.tangent = mul(input.tangent, (float3x3)world);

	output.uv = input.uv;

	output.worldPos = mul(float4(input.position, 1), world).xyz;

	matrix shadowWVP = mul(mul(world, spotLight.view), spotLight.projection);
	output.shadowPos = mul(float4(input.position, 1.0f), shadowWVP);

	return output;
}