#pragma once
#include "CSingleton.h"
#include "CLayerStack.h"

#include "Time/CClock.h"

namespace Lambda
{
	class CTime;
	class CLayer;

	//-------------
	//SEngineParams
	//-------------
	struct SEngineParams
	{
		const char** ppCmdArgs	= nullptr;
		uint32	CmdArgsCount	= 0;
	};

    //-----------------------------------------------
    //CreateGameLayer - NEEDS TO BE DEFINED BY CLIENT
    //-----------------------------------------------
    CLayer* CreateGameLayer();

	//----------
	//LambdaMain
	//----------
	int32 LAMBDA_API LambdaMain(const SEngineParams& params);

	//------
	//Engine
	//------
	namespace Engine
	{
		bool LAMBDA_API Initialize(const SEngineParams& params);
        void LAMBDA_API RunMainLoop();
        void LAMBDA_API Release();
		void LAMBDA_API Exit(int32 exitCode = 0);

		//Update rate of mainloop
		void LAMBDA_API SetUpdateTimeStep(const CTime& timestep);
		void LAMBDA_API const CTime& GetUpdateTimestep();
	}
}
