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
	SpotLight spotLight;
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

Texture2D diffuse		: register(t0);
Texture2D shadowMap		: register(t1);
SamplerState trilinear	: register(s0);
SamplerComparisonState shadowSamp : register(s1);

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 diffuseColor = diffuse.Sample(trilinear, input.uv);

	float2 shadowUV = input.shadowPos.xy / input.shadowPos.w * 0.5f + 0.5f;
	shadowUV.y = 1.0f - shadowUV.y;

	float depthFromLight = input.shadowPos.z / input.shadowPos.w;

	float shadowAmount = shadowMap.SampleCmpLevelZero(
		shadowSamp,
		shadowUV,
		depthFromLight);

	return diffuseColor * shadowAmount;
}