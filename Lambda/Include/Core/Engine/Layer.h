#pragma once
#include "LambdaCore.h"

#include "Core/Time/Time.h"

#include <string>

namespace Lambda
{
	struct SSystemEvent;

	//------
	//CLayer
	//------

	class LAMBDA_API CLayer
	{
	public:
		CLayer(const char* pName);
		~CLayer() = default;

		LAMBDA_NO_COPY(CLayer);

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(const CTime&) {}
		virtual bool OnSystemEvent(const SSystemEvent&) { return false; }

		virtual const char* GetName() const { return m_Name.c_str(); }
	private:
		std::string m_Name;
	};
}