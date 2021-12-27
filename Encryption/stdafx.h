#pragma once
#include <d2d1.h>

#pragma warning(disable:6255)
#define Alloca(type,size) (type*)alloca(sizeof(type) * (size))

__forceinline D2D1_COLOR_F Color(UINT32 hex)
{
	const auto ptr = reinterpret_cast<UINT8*>(&hex);
	const float r = *(ptr + 3) / 255.0f;
	const float g = *(ptr + 2) / 255.0f;
	const float b = *(ptr + 1) / 255.0f;
	const float a = *ptr / 255.0f;
	return { r,g,b,a };
}