#pragma once

#pragma warning(disable:6255)
#define Alloca(type,size) (type*)alloca(sizeof(type) * size)
