#include "Defines.h"
#include "Types.h"

#if defined(LAMBDA_EXPORT)
	#if defined(LAMBDA_PLAT_WINDOWS)
		#define WIN32_LEAN_AND_MEAN
		#include <Windows.h>
		#include <Windowsx.h>

		#include <wrl/client.h>
		#include <dxgi1_6.h>
		#include <d3dcompiler.h>
		#include <d3d12.h>
	#endif
#endif

#include <cassert>

#include <chrono>
#include <locale>
#include <codecvt>
#include <iostream>

#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>