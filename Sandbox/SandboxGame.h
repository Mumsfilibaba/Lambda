#pragma once
#include <Core/Game.h>

namespace Lambda
{
	class CSandBoxGame final : public CGame
	{
	public:
		CSandBoxGame();
		~CSandBoxGame() = default;

		virtual void Init();
		virtual void Update(const CTimestamp& deltaTime);
		virtual void FixedUpdate(const CTimestamp& deltaTime);
		virtual void Render(const CTimestamp& deltaTime);
		virtual void Release();
	};
}