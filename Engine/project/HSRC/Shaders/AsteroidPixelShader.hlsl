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
Texture2D normalMap		: register(t1);
Texture2D shadowMap		: register(t2);
SamplerState trilinear	: register(s0);
SamplerComparisonState shadowSamp : register(s1);

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);
	
	float3 normalFromMap = normalMap.Sample(trilinear, input.uv).rgb;
	normalFromMap = normalFromMap * 2 - 1;
	
	float3 N = input.normal;
	float3 T = normalize(input.tangent - N * dot(input.tangent, N));
	float3 B = cross(T, N);
	
	float3x3 TBN = float3x3(T, B, N);
	
	input.normal = normalize(mul(normalFromMap, TBN));

	float4 diffuseColor = diffuse.Sample(trilinear, input.uv);

	float2 shadowUV = input.shadowPos.xy / input.shadowPos.w * 0.5f + 0.5f;
	shadowUV.y = 1.0f - shadowUV.y;

	float depthFromLight = input.shadowPos.z / input.shadowPos.w;

	float shadowAmount = shadowMap.SampleCmpLevelZero(
		shadowSamp,
		shadowUV,
		depthFromLight);

	float lightDistance = length(input.worldPos - spotLight.position);

	float viewCheck = dot(spotLight.direction, input.worldPos - spotLight.position);
	viewCheck = acos(viewCheck / lightDistance);
	shadowAmount *= viewCheck <= spotLight.fov * 0.5f;

	float lightNormalDot = dot(input.normal, normalize(spotLight.position - input.worldPos));
	float lightAmount = (1 - (viewCheck / (spotLight.fov * 0.5f))) * saturate(lightNormalDot) / (lightDistance * 0.5f);

	return diffuseColor * shadowAmount * lightAmount;
}