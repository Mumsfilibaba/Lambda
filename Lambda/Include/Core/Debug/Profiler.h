#pragma once
#include "LambdaCore.h"

#include "Core/Time/Clock.h"

#include "Platform/PlatformTime.h"

namespace Lambda
{
	class LAMBDA_API CProfiler final
	{
	private:
		CProfiler();
		~CProfiler() = default;
	public:
		void BeginSession(const char* pSessionName, const char* pFilename);
		void WriteResult(const char* pResultName, const CTimestamp& start, const CTimestamp& time);
		void EndSession();

		static CProfiler& Get();
	private:
		FILE* m_pOutput;
		const char* m_pSessionName;
		bool m_bHasSession;
	};

	class CScopedProfiler final
	{
	public:
		CScopedProfiler(const char* pName)
			: m_pName(pName),
			m_Clock(),
			m_Start()
		{
			m_Start = CClock::Now();
			m_Clock.Tick();
		}

		~CScopedProfiler()
		{
			m_Clock.Tick();

			CTimestamp time = m_Clock.GetDeltaTime();
			CProfiler::Get().WriteResult(m_pName, m_Start, time);
		}

		LAMBDA_DECL_NO_COPY(CScopedProfiler);
	private:
		const char* m_pName;
		CClock m_Clock;
		CTimestamp m_Start;
	};
}

#define LAMBDA_PROFILER_ACTIVE 1
#if LAMBDA_PROFILER_ACTIVE
	//Needed for the __LINE__ concatenation to work
	#define SCOPED_PROFILER_NAME(x,y) STRING_CONCAT(x,y)

	#define LAMBDA_PROFILER_BEGIN_SESSION(name, filepath)	Lambda::CProfiler::Get().BeginSession(name, filepath)
	#define LAMBDA_PROFILER_END_SESSION()					Lambda::CProfiler::Get().EndSession()
	#define LAMBDA_PROFILER_SCOPE(name)						Lambda::CScopedProfiler SCOPED_PROFILER_NAME(profiler, __LINE__) = Lambda::CScopedProfiler(name)
	#define LAMBDA_PROFILER_FUNCTION()						LAMBDA_PROFILER_SCOPE(__FUNCSIG__)
#else
	#define LAMBDA_PROFILER_BEGIN_SESSION(name, filepath)
	#define LAMBDA_PROFILER_END_SESSION()
	#define LAMBDA_PROFILER_SCOPE(name)
	#define LAMBDA_PROFILER_FUNCTION()
#endif