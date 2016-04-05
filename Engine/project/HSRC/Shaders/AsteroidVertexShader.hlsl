cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float3 seed;
	float deformity;
	float magnitude;
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
};

// http://stackoverflow.com/questions/15628039/simplex-noise-shader
#ifndef __noise_hlsl_
#define __noise_hlsl_

// hash based 3d value noise
// function taken from https://www.shadertoy.com/view/XslGRr
// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

// ported from GLSL to HLSL

float hash(float n)
{
	return frac(sin(n)*43758.5453);
}

float noise(float3 x)
{
	// The noise function returns a value in the range -1.0f -> 1.0f

	float3 p = floor(x);
	float3 f = frac(x);

	f = f*f*(3.0 - 2.0*f);
	float n = p.x + p.y*57.0 + 113.0*p.z;

	return lerp(lerp(lerp(hash(n + 0.0), hash(n + 1.0), f.x),
		lerp(hash(n + 57.0), hash(n + 58.0), f.x), f.y),
		lerp(lerp(hash(n + 113.0), hash(n + 114.0), f.x),
			lerp(hash(n + 170.0), hash(n + 171.0), f.x), f.y), f.z);
}

#endif

VertexToPixel main( VertexShaderInput input )
{
	VertexToPixel output;
	
	matrix worldViewProj = mul(mul(world, view), projection);

	float3 perlinSample = seed + input.normal;
	float n = noise(perlinSample * deformity) * magnitude;
	output.position = float4(input.position + input.normal * n, 1);

	output.position = mul(output.position, worldViewProj);

	// http://math.stackexchange.com/questions/1071662/surface-normal-to-point-on-displaced-sphere
	float3 gradient = float3(
		(noise((perlinSample + float3(0.0001f, 0, 0)) * deformity) * magnitude - n) / 0.0001f,
		(noise((perlinSample + float3(0, 0.0001f, 0)) * deformity) * magnitude - n) / 0.0001f,
		(noise((perlinSample + float3(0, 0, 0.0001f)) * deformity) * magnitude - n) / 0.0001f
		);
	float3 tangentSpaceNormal = gradient - dot(gradient, input.position) * input.position;
	output.normal = input.position - n * tangentSpaceNormal;
	output.normal = mul(output.normal, (float3x3)world);

	output.tangent = mul(input.tangent, (float3x3)world);

	output.uv = input.uv;

	output.worldPos = mul(float4(input.position, 1), world).xyz;

	return output;
}