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


//Define forceinline
#if defined(LAMBDA_PLAT_WINDOWS)
	#define forceinline __forceinline
#elif defined(LAMBDA_PLAT_MACOS)
	#define forceinline inline
#endif


//Remove default functions
#if !defined(LAMBDA_NO_COPY)
	#define LAMBDA_NO_COPY(Type)					\
			Type(Type&&) = delete;					\
			Type(const Type&) = delete;				\
			Type& operator=(Type&&) = delete;		\
			Type& operator=(const Type&) = delete
#endif

#if !defined(LAMBDA_INTERFACE)
	#define LAMBDA_INTERFACE(Type)					\
			Type() = default;						\
			virtual ~Type() = default;				\
			LAMBDA_NO_COPY(Type)				
#endif

#if !defined(LAMBDA_IOBJECT_INTERFACE)
#define LAMBDA_IOBJECT_INTERFACE(Type)				\
			Type() = default;						\
			~Type() = default;						\
			LAMBDA_NO_COPY(Type)				
#endif

#if !defined(LAMBDA_STATIC_CLASS)
	#define LAMBDA_STATIC_CLASS(name)				\
			name() = delete;						\
			~name() = delete;						\
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
	#define DBG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
	#define DBG_MEMLEAK_CHECK() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#else
	#define DBG_NEW new
	#define DBG_MEMLEAK_CHECK()
#endif


//Disable warnings
#if defined(LAMBDA_VISUAL_STUDIO)
	#pragma warning(disable : 4251)		//Disable DLL-linkage warning
	#pragma warning(error : 4456)		//Redefinition is an error
	#pragma warning(error : 4005)		//Redefinition is an error
	#pragma warning(disable : 4201)		//Anonomys unions and structs are allowed
	#pragma warning(disable : 4307)		//Disable constant overflow warning

	#define DEBUG_BREAK __debugbreak()
#else
	#define DEBUG_BREAK assert(false)
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
#define LAMBDA_MAX_VERTEXBUFFER_COUNT	32
#define LAMBDA_MAX_RENDERTARGET_COUNT	8
#define LAMBDA_MAX_VIEWPORT_COUNT		LAMBDA_MAX_RENDERTARGET_COUNT
#define LAMBDA_MAX_SCISSOR_RECT_COUNT	LAMBDA_MAX_VIEWPORT_COUNT
