#include <LambdaPch.h>
#include <System/Log.h>
#include "DX12GraphicsContext.h"

#if defined(LAMBDA_PLAT_WINDOWS)
namespace Lambda
{
	IGraphicsContext* IGraphicsContext::Create()
	{
		return DBG_NEW DX12GraphicsContext();
	}

	DX12GraphicsContext::DX12GraphicsContext()
		: m_References(0)
	{
		AddRef();
		Init();

		LOG_SYSTEM_INFO("Creating DX12GraphicsDevice\n");
	}

	DX12GraphicsContext::~DX12GraphicsContext()
	{
	}

	uint32 DX12GraphicsContext::Release()
	{
		uint32 refs = --m_References;
		if (refs < 1)
			delete this;

		return refs;
	}

	uint32 DX12GraphicsContext::AddRef()
	{
		return ++m_References;
	}

	void DX12GraphicsContext::Init()
	{
	}
}
#endif