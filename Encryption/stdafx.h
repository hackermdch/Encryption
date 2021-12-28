#pragma once
#include <d2d1.h>
#include <string>

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

__forceinline std::wstring string2wstring(const std::string& str)
{
	std::wstring result;
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}

__forceinline std::string wstring2string(std::wstring wstr)
{
	std::string result;
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}