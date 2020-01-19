#pragma once
#include "LambdaCore.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <Windows.h>
#endif

#ifdef LAMBDA_PLAT_WINDOWS
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#else
int main(int, const char**)
#endif
{
	OutputDebugStringA("Test\n");
	return 0;
}