#include <LambdaPch.h>
#include <System/Log.h>
#include <Utilities/StringHelper.h>
#include "DX12GraphicsContext.h"

#if defined(LAMBDA_PLAT_WINDOWS)
namespace Lambda
{
	IGraphicsContext* IGraphicsContext::s_pInstance = nullptr;

	IGraphicsContext* IGraphicsContext::Create(IWindow* pWindow, GraphicsContextFlags flags)
	{
		return DBG_NEW DX12GraphicsContext(pWindow, flags);
	}

	IGraphicsContext* IGraphicsContext::GetInstance()
	{
		return s_pInstance;
	}

	bool IGraphicsContext::OnEvent(const Event& event)
	{
		return (s_pInstance) ? s_pInstance->InternalOnEvent(event) : false;
	}


	DX12GraphicsContext::DX12GraphicsContext(IWindow* pWindow, GraphicsContextFlags flags)
		: m_NumBackbuffers(0),
		m_References(0)
	{
		assert(s_pInstance == nullptr);
		s_pInstance = this;

		LOG_SYSTEM_INFO("Creating DX12GraphicsDevice\n");

		AddRef();
		Init(pWindow, flags);
	}

	DX12GraphicsContext::~DX12GraphicsContext()
	{
		if (m_SwapChain.Get())
		{
			m_SwapChain->SetFullscreenState(false, nullptr);
		}

		if (m_DebugDevice.Get())
		{
			m_DebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL);
		}
	}

	void* DX12GraphicsContext::GetNativeHandle() const
	{
		return m_Device.Get();
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

	void DX12GraphicsContext::Init(IWindow* pWindow, GraphicsContextFlags flags)
	{
		if (!CreateFactory(flags)) { return; }
		if (!QueryAdaper(flags)) { return; }
		if (!CreateDeviceAndCommandQueue(flags)) { return; }
		if (!CreateSwapChain(pWindow)) { return; }
		if (!InitBackBuffers()) { return; }
	}

	void DX12GraphicsContext::ReleaseBackBuffers()
	{
		using namespace Microsoft::WRL;

		for (ComPtr<ID3D12Resource> resource : m_BackBuffers)
			resource.Reset();
	}

	bool DX12GraphicsContext::CreateFactory(GraphicsContextFlags flags)
	{
		using namespace Microsoft::WRL;

		//Should we enable debuglayer
		uint32 factoryFlags = 0;
		if (flags & GRAPHICS_CONTEXT_FLAG_DEBUG)
		{
			factoryFlags = DXGI_CREATE_FACTORY_DEBUG;

			if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_Debug))))
			{
				LOG_DEBUG_ERROR("DX12: Failed to get debug interface\n");
				return false;
			}
			else
			{
				m_Debug->EnableDebugLayer();
			}
		}

		//Create factory
		HRESULT hr = CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_Factory));
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Failed to create factory\n");
			return false;
		}

		return true;
	}

	bool DX12GraphicsContext::QueryAdaper(GraphicsContextFlags flags)
	{
		using namespace Microsoft::WRL;

		int32 adapterCount = 0;
		int32 bestAdapterIndex = -1;
		uint32 vendorID = 0;

		//Retrive all adapters 
		ComPtr<IDXGIAdapter1> adapter = nullptr;
		for (uint32 i = 0; m_Factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++)
		{
			DXGI_ADAPTER_DESC1 desc = {};
			adapter->GetDesc1(&desc);

			//Skip software adapters
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				if (!(flags & GRAPHICS_CONTEXT_FLAG_ALLOW_SOFTWARE_ADAPTER))
					continue;
			}

			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
			{
				//We skip the intel once for now
				if ((desc.VendorId != 0x163C && desc.VendorId != 0x8086 && desc.VendorId != 0x8087 && (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0) || (bestAdapterIndex < 0))
				{
					bestAdapterIndex = i;
					vendorID = desc.VendorId;
				}

				adapterCount++;
			}
			else
			{
				LOG_DEBUG_WARNING("DX12: Adapter %d does not support D3D12", i);
			}
		}

		if (adapterCount < 1 || bestAdapterIndex < 0)
		{
			LOG_DEBUG_ERROR("DX12: No adapter supporting D3D12 was found\n");
			return false;
		}
		else
		{
			adapter = nullptr;
			if (FAILED(m_Factory->EnumAdapters1(bestAdapterIndex, &adapter)))
			{
				return false;
			}
			else
			{
				if (FAILED(adapter.As<IDXGIAdapter4>(&m_Adapter)))
				{
					LOG_DEBUG_WARNING("DX12: Failed to query IDXGIAdapter3\n");
					return false;
				}
				else
				{
					DXGI_ADAPTER_DESC1 desc = {};
					m_Adapter->GetDesc1(&desc);

					std::string adaptername = WidestringToString(desc.Description);
					LOG_DEBUG_INFO("DX12: Selected adapter: %s\n", adaptername.c_str());

					return true;
				}
			}
		}
		return true;
	}

	bool DX12GraphicsContext::CreateDeviceAndCommandQueue(GraphicsContextFlags flags)
	{
		//Create device
		HRESULT hr = D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));
		if (SUCCEEDED(hr))
		{
			LOG_DEBUG_INFO("DX12: Created device.\n");

			if (flags & GRAPHICS_CONTEXT_FLAG_DEBUG)
			{
				hr = m_Device->QueryInterface<ID3D12DebugDevice2>(&m_DebugDevice);
				if (FAILED(hr))
				{
					LOG_DEBUG_ERROR("DX12: Could not create DebugDevice.\n");
					return false;
				}
			}
		}
		else
		{
			LOG_DEBUG_ERROR("DX12: Could not create device.\n");
			return false;
		}

		D3D12_COMMAND_QUEUE_DESC qDesc = {};
		qDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		qDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		qDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		qDesc.NodeMask = 0;

		//Create commandqueue
		hr = m_Device->CreateCommandQueue(&qDesc, IID_PPV_ARGS(&m_Queue));
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Could not create CommandQueue.\n");
			return false;
		}
		else
		{
			LOG_DEBUG_INFO("DX12: Created CommandQueue.\n");
		}

		//Create fence
		hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Could not create Fence.\n");
			return false;
		}
		else
		{
			LOG_DEBUG_INFO("DX12: Created Fence.\n");
		}

		return true;
	}

	bool DX12GraphicsContext::CreateSwapChain(IWindow* pWindow)
	{
		using namespace Microsoft::WRL;

		DXGI_SWAP_CHAIN_DESC1 desc = {};
		memset(&desc, 0, sizeof(desc));
		desc.BufferCount = 3;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Width = pWindow->GetWidth();
		desc.Height = pWindow->GetHeight();
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		//TODO: Tearing?
		desc.Flags = 0;

		HWND hWnd = (HWND)pWindow->GetNativeHandle();
		ComPtr<IDXGISwapChain1> swapChain = nullptr;
		HRESULT hr = m_Factory->CreateSwapChainForHwnd(m_Queue.Get(), hWnd, &desc, nullptr, nullptr, &swapChain);
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Failed to create swapchain.\n");
			return false;
		}
		else
		{
			if (FAILED(swapChain.As<IDXGISwapChain4>(&m_SwapChain)))
			{
				LOG_DEBUG_ERROR("DX12: Failed to retrive IDXGISwapChain4.\n");
				return false;
			}

			LOG_DEBUG_INFO("DX12: Created swapchain\n");
			m_NumBackbuffers = desc.BufferCount;
		}

		return true;
	}

	bool DX12GraphicsContext::InitBackBuffers()
	{
		using namespace Microsoft::WRL;

		m_BackBuffers.resize(m_NumBackbuffers);
		for (uint32 i = 0; i < m_NumBackbuffers; i++)
		{
			if (FAILED(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&(m_BackBuffers[i])))))
			{
				LOG_DEBUG_ERROR("DX12: Failed to retrice backbuffer '%d'.", i);
				return false;
			}
		}

		return true;
	}

	bool DX12GraphicsContext::InternalOnEvent(const Event& event)
	{
		if (event.Type == EVENT_TYPE_WINDOW_RESIZE)
		{
			LOG_SYSTEM_INFO("Resize\n");

			ReleaseBackBuffers();
			InitBackBuffers();

			return true;
		}

		return false;
	}
}
#endif