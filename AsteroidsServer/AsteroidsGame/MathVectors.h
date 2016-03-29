#pragma once

#include <string>

inline float* alignInit(float* data, int numObjects)
{
	data = (float*)_aligned_malloc(sizeof(float) * numObjects, 32);
	memset(data, 0, sizeof(float) * numObjects);
	return data;
}

/// <summary>
/// Stores a singularvia an array for SIMD operations
/// </summary>
struct Vec1{
	float* value;

	int size;

	Vec1(){
		size = -1;
		value = nullptr;
	}

	Vec1(int length){
		size = length;
		value = (float*)_aligned_malloc(sizeof(float) * length, 32);
		memset(value, 0, sizeof(float) * length);
	}

	~Vec1(){
		if (value){
			_aligned_free(value);
		}
	}

	float* operator+(int i){
		return value + i;
	}

};

/// <summary>
/// Stores two values via arrays for SIMD operations
/// </summary>
struct Vec2{
	float* x;
	float* y;

	int size;

	Vec2(){
		size = -1;
		x = nullptr;
		y = nullptr;
	}

	Vec2(int length){
		size = length;

		x = (float*)_aligned_malloc(sizeof(float) * length, 32);
		memset(x, 0, sizeof(float) * length);
		y = (float*)_aligned_malloc(sizeof(float) * length, 32);
		memset(y, 0, sizeof(float) * length);
	}

	~Vec2(){
		if (x){
			_aligned_free(x);
		}
		if (y){
			_aligned_free(y);
		}
	}

};