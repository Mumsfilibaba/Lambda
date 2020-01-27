#pragma once
#include "LambdaCore.h"

#include "Time/Timestep.h"

namespace Lambda
{
	class CGame
	{
	public:
		CGame() = default;
		virtual ~CGame() = default;

		LAMBDA_DECL_NO_COPY(CGame);

		virtual void Init() {};
		virtual void Update(const CTimestep&) {}
		virtual void FixedUpdate(const CTimestep&) {}
		virtual void Render(const CTimestep&) {}
		virtual void Release() {}
	};

	typedef CGame* (*CreateGameFunc)();
	LAMBDA_API extern CreateGameFunc _CreateGame;
}
