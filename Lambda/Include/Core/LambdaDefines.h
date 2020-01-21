#pragma once

//DLL declaration
#if defined(LAMBDA_SHARED_LIB) && defined(LAMBDA_PLAT_WINDOWS)
	#if defined(LAMBDA_EXPORT)
		#define LAMBDA_API __declspec(dllexport)
	#else
		#define LAMBDA_API __declspec(dllimport)
	#endif
#else
	#define LAMBDA_API
#endif


//Define forceinline
#if defined(LAMBDA_PLAT_WINDOWS)
	#define _forceinline __forceinline
#elif defined(LAMBDA_PLAT_MACOS)
	#define _forceinline inline __attribute__((always_inline))
#endif


//Remove default functions
#if !defined(LAMBDA_NO_COPY)
#define LAMBDA_NO_COPY(Type) \
	Type(Type&&) = delete; \
	Type(const Type&) = delete;	\
	Type& operator=(Type&&) = delete; \
	Type& operator=(const Type&) = delete
#endif

#if !defined(LAMBDA_INTERFACE)
#define LAMBDA_INTERFACE(Type) \
	Type() = default; \
	virtual ~Type() = default; \
	LAMBDA_NO_COPY(Type)				
#endif

#if !defined(LAMBDA_ISHAREDOBJECT_INTERFACE)
#define LAMBDA_ISHAREDOBJECT_INTERFACE(Type) \
	Type() = default; \
	~Type() = default; \
	LAMBDA_NO_COPY(Type)				
#endif

#if !defined(LAMBDA_STATIC_CLASS)
#define LAMBDA_STATIC_CLASS(name) \
	name() = delete; \
	~name() = delete; \
	LAMBDA_NO_COPY(name)
#endif


//Saferelease and delete
#if !defined(SafeRelease)
	#define SafeRelease(x) if (x != nullptr) { x->Release(); x = nullptr; }
#endif

#if !defined(SafeDelete)
	#define SafeDelete(x) if (x != nullptr) { delete x; x = nullptr; }
#endif

#if !defined(SafeDeleteArr)
	#define SafeDeleteArr(x) if (x != nullptr) { delete[] x; x = nullptr; }
#endif


//Memleak debugging
#if defined(LAMBDA_DEBUG) && defined(LAMBDA_PLAT_WINDOWS)
	#include <crtdbg.h>

	#define DBG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#else
	#define DBG_NEW new
	#define DBG_MEMLEAK_CHECK()
#endif


//Disable warnings
#if defined(LAMBDA_VISUAL_STUDIO)
	#pragma warning(error : 4456)		//"Declaration of 'identifier' hides previous local declaration"	- error
	#pragma warning(error : 4005)		//"The macro identifier is defined twice"							- error
	#pragma warning(error : 4002)		//"Too manu arguments for function-like macro"						- error

	#pragma warning(disable : 4251)		//Disable DLL-linkage warning				- Disabled 
	#pragma warning(disable : 4201)		//Anonomys unions and structs are allowed	- Disabled
	#pragma warning(disable : 4307)		//Disable constant overflow warning			- Disabled
#endif
