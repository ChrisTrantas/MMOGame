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
};

float4 calcDirectionalLight(DirectionalLight light, float3 normal)
{
	float3 lightDir = normalize(-1 * light.Direction);
	float lightAmount = saturate(dot(normal, lightDir));

	return light.DiffuseColor * lightAmount;
}

float4 main(VertexToPixel input) : SV_TARGET
{
	float4 lights = calcDirectionalLight(light1, input.normal) * 0.5f + calcDirectionalLight(light2, input.normal) * 0.5f;
	return lights + light1.AmbientColor * 0.3f + light2.AmbientColor * 0.3f;
}