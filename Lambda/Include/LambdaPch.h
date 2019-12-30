#include "LambdaCore.h"

#if defined(LAMBDA_EXPORT)
	//All headers specific for windows
	#if defined(LAMBDA_PLAT_WINDOWS)
		#define WIN32_LEAN_AND_MEAN 1
        #define NOMINMAX
        #include <Windows.h>
		#include <Windowsx.h>
		#include <crtdbg.h>

		#include <wrl/client.h>
		#include <dxgi1_6.h>
		#include <d3dcompiler.h>
		#include <d3d12.h>
	#else
		#include <cassert>
		#include <chrono>
	#endif
#endif

#include <cfloat>
#include <cmath>

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
