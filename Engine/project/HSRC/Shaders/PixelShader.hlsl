struct DirectionalLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
};

cbuffer externalData : register(b0)
{
	DirectionalLight light1;
	DirectionalLight light2;
};

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float2 uv			: TEXCOORD0;
	float3 worldPos		: TEXCOORD1;
};

Texture2D diffuse		: register(t0);
Texture2D normalMap		: register(t1);
SamplerState trilinear	: register(s0);

float4 calcDirectionalLight(DirectionalLight light, float3 normal)
{
	float3 lightDir = normalize(light.Direction);
	float lightAmount = saturate(dot(normal, -lightDir));

	return light.DiffuseColor * lightAmount;
}

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

	float4 lights = calcDirectionalLight(light1, input.normal) * 0.4f + calcDirectionalLight(light2, input.normal) * 0.4f;
	lights += light1.AmbientColor * 0.05f + light2.AmbientColor * 0.05f;
	return lights * diffuseColor;
}