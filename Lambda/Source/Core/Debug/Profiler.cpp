#include "LambdaPch.h"

#include "Core/Log/EngineLog.h"
#include "Core/Debug/Profiler.h"

#include <stdio.h>
#include <sstream>

namespace Lambda
{
	CProfiler::CProfiler()
		: m_pOutput(nullptr),
		m_pSessionName(nullptr),
		m_bHasSession(false)
	{
	}

	void CProfiler::BeginSession(const char* pSessionName, const char* pFilename)
	{
		//If there is already is an active session, end it
		if (m_pOutput)
		{
			EndSession();
		}

		LAMBDA_ASSERT(pFilename != nullptr && pSessionName != nullptr);

		FILE* pOutput = fopen(pFilename, "w");
		if (pOutput != nullptr)
		{
			fprintf(pOutput, "{\"otherData\": {},\"traceEvents\":[{}");
			fflush(pOutput);

			m_pSessionName = pSessionName;
			m_pOutput = pOutput;

			LOG_CORE_INFO("Started new profiling session '%s'", pSessionName);
		}
		else
		{
			LOG_CORE_ERROR("Failed to open session file '%s'", pFilename);
		}
	}

	void CProfiler::WriteResult(const char* pResultName, const CTimestamp& start, const CTimestamp& time)
	{
		std::stringstream ss;
		ss << std::this_thread::get_id();

		fprintf(m_pOutput, 
			",{"
			"\"cat\":\"function\","
			"\"dur\":%llu,"
			"\"name\":\"%s\","
			"\"ph\":\"X\","
			"\"pid\":0,"
			"\"tid\":%s,"
			"\"ts\":%llu"
			"}",
			time.AsMicroSeconds(),
			pResultName,
			ss.str().c_str(),
			start.AsMicroSeconds()
		);

		fflush(m_pOutput);
	}

	void CProfiler::EndSession()
	{
		if (m_pOutput)
		{
			fprintf(m_pOutput, "]}");
			fflush(m_pOutput);

			fclose(m_pOutput);

			LOG_CORE_INFO("Stopped profiling session '%s'", m_pSessionName);
			
			m_pOutput = nullptr;
			m_pSessionName = nullptr;
		}
		else
		{
			LOG_CORE_ERROR("No active session");
		}
	}
}