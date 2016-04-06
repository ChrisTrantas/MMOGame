#pragma once

#include "dxerr.h"
#include <locale>

#define ReleaseMacro(x) { if(x){ x->Release(); x = 0; } }

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)												\
	{															\
		HRESULT hr = (x);										\
		if(FAILED(hr))											\
		{														\
			DXTrace(__FILEW__, (DWORD)__LINE__, hr, L#x, true);	\
			PostQuitMessage(0);									\
		}														\
	}														
#endif
#else
#ifndef HR
#define HR(x) (x) // Do nothing special!
#endif
#endif

extern std::wstring_convert< std::codecvt<wchar_t, char, std::mbstate_t> > conv;

#define STR_TO_WCHART(str) conv.from_bytes(str)

#define RAND_DOUBLE ((double)rand() / RAND_MAX)
#define RAND_FLOAT (float)RAND_DOUBLE
#define RAND_VEC3 vec3(RAND_FLOAT * 2 - 1, RAND_FLOAT * 2 - 1, RAND_FLOAT * 2 - 1)
