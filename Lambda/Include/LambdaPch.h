#include "Defines.h"
#include "Types.h"

#if defined(LAMBDA_EXPORT)
	//All headers specific for windows
	#if defined(LAMBDA_PLAT_WINDOWS)
		//Remove unneccesary stuff
		#define WIN32_LEAN_AND_MEAN 
		//Remove min max macros
        #define NOMINMAX
        #include <Windows.h>
		#include <Windowsx.h>
		#include <crtdbg.h>

		#include "..\Source\Platform\DX12\d3dx12.h"
		#include <wrl/client.h>
		#include <dxgi1_6.h>
		#include <d3dcompiler.h>
		#include <d3d12.h>
	#endif
#endif

#include "System/Log.h"

#include <cassert>

#include <chrono>
#include <locale>
#include <codecvt>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>

#include <vector>
#include <mutex>
#include <thread>
#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>
