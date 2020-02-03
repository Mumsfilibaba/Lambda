#pragma once
#include "LambdaCore.h"

#include "Time/Timestep.h"

namespace Lambda
{
	class LAMBDA_API CGame
	{
	public:
		CGame();
		virtual ~CGame();

		LAMBDA_DECL_NO_COPY(CGame);

		virtual void Init() {};
		virtual void Update(const CTimestep&) {}
		virtual void FixedUpdate(const CTimestep&) {}
		virtual void Render(const CTimestep&) {}
		virtual void Release() {}

		_forceinline static CGame& Get() { return *s_pInstance; }
	private:
		static CGame* s_pInstance;
	};

	typedef CGame* (*CreateGameInstanceFunc)();
	extern LAMBDA_API CreateGameInstanceFunc _CreateGameInstance;
}
