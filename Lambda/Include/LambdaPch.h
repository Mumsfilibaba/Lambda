#include "Defines.h"
#include "Types.h"

#if defined(LAMBDA_EXPORT)
	#if defined(LAMBDA_PLAT_WINDOWS)
		#define WIN32_LEAN_AND_MEAN
		#include <Windows.h>
		#include <Windowsx.h>

		#include "..\Source\Platform\DX12\d3dx12.h"
		#include <wrl/client.h>
		#include <dxgi1_6.h>
		#include <d3dcompiler.h>
		#include <d3d12.h>
	#endif
#endif

#include <System/Log.h>

#include <cassert>

#include <chrono>
#include <locale>
#include <codecvt>
#include <iostream>
#include <fstream>

#include <vector>
#include <mutex>
#include <thread>
#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>