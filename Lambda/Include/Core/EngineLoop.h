#pragma once
#include "LambdaCore.h"
#include "Singleton.h"

#include "Time/Clock.h"

namespace Lambda
{
	class LAMBDA_API CEngineLoop : public CSingleton<CEngineLoop>
	{
	public:
		bool IsRunning() const { return m_bIsRunning; }
		void Init();
		void Start();
		void Tick();
		void Release();

		static CEngineLoop* Create()
		{
			return DBG_NEW CEngineLoop();
		}
	private:
		CEngineLoop();
		~CEngineLoop() = default;
	private:
		bool m_bIsRunning;
		CClock m_Frameclock;
	};
}