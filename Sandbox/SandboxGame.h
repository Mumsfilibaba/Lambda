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
		virtual void Update(const CTimestep& deltaTime);
		virtual void FixedUpdate(const CTimestep& deltaTime);
		virtual void Render(const CTimestep& deltaTime);
		virtual void Release();
	};
}