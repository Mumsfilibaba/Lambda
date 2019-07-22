#pragma once

//DLL declaration
#if defined(LAMBDA_PLAT_WINDOWS)
	#if defined(LAMBDA_EXPORT)
		#define LAMBDA_API __declspec(dllexport)
	#else
		#define LAMBDA_API __declspec(dllimport)
	#endif
#else
	#define LAMBDA_API
#endif


//Remove default functions
#if !defined(LAMBDA_NO_COPY)
	#define LAMBDA_NO_COPY(name)					\
			name(name&&) = delete;					\
			name(const name&) = delete;				\
			name& operator=(name&&) = delete;		\
			name& operator=(const name&) = delete
#endif

#if !defined(LAMBDA_INTERFACE)
	#define LAMBDA_INTERFACE(name) LAMBDA_NO_COPY(name)
#endif

#if !defined(LAMBDA_STATIC_CLASS)
	#define LAMBDA_STATIC_CLASS(name)				\
	name() = delete;								\
	~name() = delete;								\
	LAMBDA_NO_COPY(name)
#endif


//Saferelease and delete
#if !defined(SafeRelease)
	#define SafeRelease(x) if (x != nullptr) { x->Release(); x = nullptr; }
#endif

#if !defined(SafeDelete)
#define SafeDelete(x) if (x != nullptr) { delete x; x = nullptr; }
#endif


//Memleak debugging
#if defined(LAMBDA_DEBUG) && defined(LAMBDA_PLAT_WINDOWS)
	#define DBG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
	#define DBG_MEMLEAK_CHECK() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#else
	#define DBG_NEW new
	#define DBG_MEMLEAK_CHECK()
#endif


//Disable warnings
#if defined(LAMBDA_VISUAL_STUDIO)
	#pragma warning(disable : 4251) //Disable DLL-linkage warning
	#pragma warning(error : 4456) //Redefinition is an error
	#pragma warning(error : 4005) //Redefinition is an error
	#pragma warning(disable : 4201) //Anonomys unions and structs are allowed

	#define DEBUG_BREAK __debugbreak
#else
	#define DEBUG_BREAK
#endif


//Sizes
#define MB(num) num * 1024 * 1024


//Shaderstages
#define LAMBDA_SHADERSTAGE_COUNT    5 //VS, HS, DS, GS, PS
#define LAMBDA_SHADERSTAGE_VERTEX   0
#define LAMBDA_SHADERSTAGE_HULL     1
#define LAMBDA_SHADERSTAGE_DOMAIN   2
#define LAMBDA_SHADERSTAGE_GEOMETRY 3
#define LAMBDA_SHADERSTAGE_PIXEL    4


//Count of resource binding slots per shaderstage
#define LAMBDA_SHADERSTAGE_UNIFORM_COUNT 8
#define LAMBDA_SHADERSTAGE_SAMPLER_COUNT 8
#define LAMBDA_SHADERSTAGE_TEXTURE_COUNT 8


//Counts
#define LAMBDA_RENDERTARGET_COUNT 8
