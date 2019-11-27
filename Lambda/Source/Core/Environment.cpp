#include "LambdaPch.h"
#include "Core/Environment.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "../Platform/Windows/WindowsEnvironment.h"
#endif

namespace Lambda
{
	//-----------
	//Environment
	//-----------

	template<>
	Environment* Singleton<Environment>::s_pInstance = nullptr;

	Environment* Environment::Create()
	{
#if defined(LAMBDA_PLAT_WINDOWS)
		return DBG_NEW WindowsEnvironment();
#endif
	}
}