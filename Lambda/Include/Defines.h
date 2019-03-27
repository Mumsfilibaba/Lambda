#pragma once
#if defined(LAMBDA_PLAT_WINDOWS)
	#if defined(LAMBDA_EXPORT)
		#define LAMBDA_API __declspec(dllexport)
	#else
		#define LAMBDA_API __declspec(dllimport)
	#endif
#endif

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

#if !defined(SafeRelease)
	#define SafeRelease(x) if (x != nullptr) { x->Release(); x = nullptr; }
#endif

#if !defined(SafeDelete)
#define SafeDelete(x) if (x != nullptr) { delete x; x = nullptr; }
#endif

#if defined(LAMBDA_DEBUG)
	#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
	#define DBG_NEW new
#endif

#if defined(LAMBDA_VISUAL_STUDIO)
	#pragma warning(disable : 4251) //Disable DLL-linkage warning
	#pragma warning(error : 4456) //Redefinition is an error
	#pragma warning(error : 4005) //Redefinition is an error
#endif
