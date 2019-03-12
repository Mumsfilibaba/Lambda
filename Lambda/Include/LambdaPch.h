#include "Defines.h"
#include "Types.h"

#if defined(LAMBDA_EXPORT)
	#if defined(LAMBDA_PLAT_WINDOWS)
		#define WIN32_LEAN_AND_MEAN
		#include <Windows.h>
		#include <Windowsx.h>
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