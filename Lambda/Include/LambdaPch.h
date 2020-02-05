#include "LambdaCore.h"

#if defined(LAMBDA_EXPORT)
	//All headers specific for windows
	#if defined(LAMBDA_PLAT_WINDOWS)
		//Remove unneccesary WIN32 apis
		#if !defined(WIN32_LEAN_AND_MEAN )
			#define WIN32_LEAN_AND_MEAN 1
		#endif

		//Remove min max macros
		#define NOMINMAX
		#include <Windows.h>
		#include <Windowsx.h>
		#include <crtdbg.h>

		//#include "../Source/Platform/DX12/d3dx12.h"
		#include <wrl/client.h>
		#include <dxgi1_6.h>
		#include <d3dcompiler.h>
		#include <d3d12.h>
	#endif
#endif

#include "Core/Log/EngineLog.h"

#include <cassert>
#include <cfloat>
#include <cmath>

#include <chrono>
#include <locale>
#include <codecvt>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <utility>

#include <vector>
#include <mutex>
#include <thread>
#include <memory>
#include <string>
#include <sstream>
#include <set>
#include <unordered_map>