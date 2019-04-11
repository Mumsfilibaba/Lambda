#include <LambdaPch.h>
#include <Utilities/StringHelper.h>
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12GraphicsDevice.h"
	#include "DX12CommandList.h"
	#include "DX12PipelineState.h"
	#include "DX12Shader.h"
	#include "DX12Buffer.h"
	#include "DX12Texture2D.h"

namespace Lambda
{
	IGraphicsDevice* IGraphicsDevice::s_pInstance = nullptr;

	IGraphicsDevice* IGraphicsDevice::Create(IWindow* pWindow, GraphicsContextFlags flags)
	{
		return DBG_NEW DX12GraphicsDevice(pWindow, flags);
	}

	IGraphicsDevice* IGraphicsDevice::GetInstance()
	{
		return s_pInstance;
	}

	bool IGraphicsDevice::OnEvent(const Event& event)
	{
		return (s_pInstance) ? s_pInstance->InternalOnEvent(event) : false;
	}


	DX12GraphicsDevice::DX12GraphicsDevice(IWindow* pWindow, GraphicsContextFlags flags)
		: m_pRTAllocator(nullptr),
		m_pDSAllocator(nullptr),
		m_pResourceAllocator(nullptr),
		m_pSamplerAllocator(nullptr),
		m_BackBufferFlags(0),
		m_GPUWaitEvent(0),
		m_NumBackbuffers(0),
		m_CurrentBackBuffer(0),
		m_References(0)
	{
		assert(s_pInstance == nullptr);
		s_pInstance = this;

		LOG_SYSTEM_INFO("Creating DX12GraphicsDevice\n");
		
		//TODO: Allow tearing?
		m_BackBufferFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_NumBackbuffers = 3;

		AddRef();
		Init(pWindow, flags);
	}


	DX12GraphicsDevice::~DX12GraphicsDevice()
	{
		WaitForGPU();

		if (m_SwapChain.Get())
		{
			m_SwapChain->SetFullscreenState(false, nullptr);
		}

		if (m_DebugDevice.Get())
		{
			m_DebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL);
		}

		SafeRelease(m_pCommandList);
		for (uint32 i = 0; i < m_NumBackbuffers; i++)
			SafeRelease(m_BackBuffers[i]);

		CloseHandle(m_GPUWaitEvent);

		SafeDelete(m_pRTAllocator);
		SafeDelete(m_pDSAllocator);
		SafeDelete(m_pResourceAllocator);
		SafeDelete(m_pSamplerAllocator);
	}


	void DX12GraphicsDevice::CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) const
	{
		DX12Buffer* pBuffer = nullptr;
		//Create a dynamic resource, or default if there is no inital data
		if (desc.Usage == RESOURCE_USAGE_DYNAMIC || pInitalData == nullptr)
		{
			pBuffer = DBG_NEW DX12Buffer(m_Device.Get(), desc);
		}
		//Create a normal resource
		else if(desc.Usage == RESOURCE_USAGE_DEFAULT)
		{
			//Create actual buffer
			pBuffer = DBG_NEW DX12Buffer(m_Device.Get(), desc);

			//Copy data
			m_pCommandList->TransitionResource(pBuffer, RESOURCE_STATE_COPY_DEST);
			m_pCommandList->UpdateBuffer(pBuffer, pInitalData);
			m_pCommandList->TransitionResource(pBuffer, RESOURCE_STATE_PRESENT_COMMON);

			//Execute and wait for GPU before creating
			m_pCommandList->Close();

			ICommandList* pList = m_pCommandList;
			ExecuteCommandList(&pList, 1);

			WaitForGPU();
			m_pCommandList->Reset();
		}

		//Create constantbuffer
		if (desc.Flags & BUFFER_FLAGS_CONSTANT_BUFFER)
		{
			//Allocate and set descriptor
			DX12DescriptorHandle hDescriptor = m_pResourceAllocator->Allocate();
			pBuffer->SetDescriporHandle(hDescriptor);

			//Create view
			D3D12_CONSTANT_BUFFER_VIEW_DESC vDesc = {};
			vDesc.BufferLocation = pBuffer->GetGPUVirtualAdress();
			vDesc.SizeInBytes = desc.SizeInBytes;
			m_Device->CreateConstantBufferView(&vDesc, hDescriptor.CPU);
		}

		(*ppBuffer) = pBuffer;
	}


	void DX12GraphicsDevice::CreateTexture2D(ITexture2D** ppTexture, const ResourceData* pInitalData, const Texture2DDesc& desc) const
	{
		DX12Texture2D* pTexture = DBG_NEW DX12Texture2D(m_Device.Get(), desc);
		//DepthStencil
		if (desc.Flags & TEXTURE_FLAGS_DEPTH_STENCIL)
		{
			//Allocate depth stencil descriptor
			DX12DescriptorHandle hDescriptor = m_pDSAllocator->Allocate();
			pTexture->SetDescriptorHandle(hDescriptor);

			//Create view
			D3D12_DEPTH_STENCIL_VIEW_DESC vDesc = {};
			vDesc.Flags = D3D12_DSV_FLAG_NONE;
			vDesc.Format = ConvertFormat(desc.Format);
			vDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			vDesc.Texture2D.MipSlice = 0;
			m_Device->CreateDepthStencilView(pTexture->GetResource(), &vDesc, hDescriptor.CPU);
		}
		
		(*ppTexture) = pTexture;
	}


	void DX12GraphicsDevice::CreateShader(IShader** ppShader, const ShaderDesc& desc) const
	{
		(*ppShader) = DBG_NEW DX12Shader(desc);
	}


	void DX12GraphicsDevice::CreateShaderFromFile(IShader** ppShader, const char* pFilename, const ShaderDesc& desc) const
	{
		ShaderDesc tempDesc = desc;
		
		std::ifstream file(pFilename, std::ios::in);
		std::string source;

		if (file.is_open())
		{
			std::stringstream buf;
			buf << file.rdbuf();
			source = buf.str();
			file.close();
		}
		else
		{
			LOG_DEBUG_ERROR("DX12: Failed to read shaderfile '%s'\n", pFilename);
			return;
		}
		
		tempDesc.pSource = source.c_str();
		CreateShader(ppShader, tempDesc);
	}


	void DX12GraphicsDevice::CreateGraphicsPipelineState(IGraphicsPipelineState** ppPSO, const GraphicsPipelineStateDesc& desc) const
	{
		(*ppPSO) = DBG_NEW DX12GraphicsPipelineState(m_Device.Get(), desc);
	}


	void DX12GraphicsDevice::ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const
	{
		//Convert lists
		for (uint32 i = 0; i < numLists; i++)
		{
			DX12CommandList* pList = reinterpret_cast<DX12CommandList*>(ppLists[i]);
			m_PendingLists.push_back(pList->GetList());
		}

		//Execute
		m_Queue->ExecuteCommandLists(numLists, m_PendingLists.data());
		m_PendingLists.clear();
	}


	void DX12GraphicsDevice::Present(uint32 verticalSync) const
	{
		m_SwapChain->Present(verticalSync, 0);
	}


	void DX12GraphicsDevice::GPUWaitForFrame() const
	{
		//Increment the fencecalue on the GPU (signal) for the current frame
		uint64 fenceValue = m_FenceValues[m_CurrentBackBuffer];
		m_Queue->Signal(m_Fence.Get(), fenceValue);

		//Update current backbuffer
		m_CurrentBackBuffer = m_SwapChain->GetCurrentBackBufferIndex();

		//Wait if value is lower than current frame
		if (m_Fence->GetCompletedValue() < m_FenceValues[m_CurrentBackBuffer])
		{
			m_Fence->SetEventOnCompletion(m_FenceValues[m_CurrentBackBuffer], m_GPUWaitEvent);
			WaitForSingleObjectEx(m_GPUWaitEvent, INFINITE, FALSE);
		}

		//Increment fencevalue
		m_FenceValues[m_CurrentBackBuffer] = fenceValue + 1;
	}


	void DX12GraphicsDevice::WaitForGPU() const
	{
		//Increment the fencecalue on the GPU (signal) for the current frame
		uint64 fenceValue = m_FenceValues[m_CurrentBackBuffer] + 1;
		m_Queue->Signal(m_Fence.Get(), fenceValue);

		//Wait for value
		m_Fence->SetEventOnCompletion(fenceValue, m_GPUWaitEvent);
		WaitForSingleObjectEx(m_GPUWaitEvent, INFINITE, FALSE);

		//Increment fencevalue
		m_FenceValues[m_CurrentBackBuffer]++;
	}


	ITexture2D* DX12GraphicsDevice::GetCurrentRenderTarget()
	{
		return m_BackBuffers[m_SwapChain->GetCurrentBackBufferIndex()];
	}


	uint32 DX12GraphicsDevice::GetCurrentBackBufferIndex() const
	{
		return m_SwapChain->GetCurrentBackBufferIndex();
	}


	void* DX12GraphicsDevice::GetNativeHandle() const
	{
		//Return the ID3D12Device-ptr
		return m_Device.Get();
	}


	uint32 DX12GraphicsDevice::Release()
	{
		IOBJECT_IMPLEMENT_RELEASE(m_References);
	}


	uint32 DX12GraphicsDevice::AddRef()
	{
		return ++m_References;
	}


	void DX12GraphicsDevice::Init(IWindow* pWindow, GraphicsContextFlags flags)
	{
		if (!CreateFactory(flags)) { return; }
		if (!QueryAdaper(flags)) { return; }
		if (!CreateDeviceAndCommandQueue(flags)) { return; }
		if (!CreateSwapChain(pWindow)) { return; }
		if (!CreateCommandList()) { return; }
		if (!CreateDescriptorHeaps()) { return; }
		if (!InitBackBuffers()) { return; }
	}


	void DX12GraphicsDevice::ReleaseBackBuffers()
	{
		using namespace Microsoft::WRL;

		for (DX12Texture2D* pTarget : m_BackBuffers)
		{
			m_pRTAllocator->Free(pTarget->GetDescriptorHandle());
			pTarget->SetResource(nullptr);
		}
	}


	bool DX12GraphicsDevice::CreateFactory(GraphicsContextFlags flags)
	{
		using namespace Microsoft::WRL;

		//Should we enable debuglayer
		uint32 factoryFlags = 0;
		if (flags & GRAPHICS_CONTEXT_FLAG_DEBUG)
		{
			factoryFlags = DXGI_CREATE_FACTORY_DEBUG;

			ComPtr<ID3D12Debug> debugController = nullptr;
			if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				LOG_DEBUG_ERROR("DX12: Failed to get debug interface\n");
				return false;
			}
			else
			{
				if (SUCCEEDED(debugController.As<ID3D12Debug3>(&m_Debug)))
				{
					m_Debug->EnableDebugLayer();
				}
				else
				{
					LOG_DEBUG_ERROR("DX12: Failed to retrive ID3D12Debug3\n");
					return false;
				}
			}
		}

		//Create factory
		ComPtr<IDXGIFactory2> factory = nullptr;
		HRESULT hr = CreateDXGIFactory2(flags, IID_PPV_ARGS(&factory));
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Failed to create factory\n");
			return false;
		}
		else
		{
			if (FAILED(factory.As<IDXGIFactory7>(&m_Factory)))
			{
				LOG_DEBUG_ERROR("DX12: Failed to retrive IDXGIFactory7\n");
				return false;
			}
		}

		return true;
	}


	bool DX12GraphicsDevice::QueryAdaper(GraphicsContextFlags flags)
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
			//Get chosen adapter
			if (FAILED(m_Factory->EnumAdapters1(bestAdapterIndex, &adapter)))
			{
				return false;
			}
			else
			{
				//Get newer interface
				if (FAILED(adapter.As<IDXGIAdapter4>(&m_Adapter)))
				{
					LOG_DEBUG_ERROR("DX12: Failed to query IDXGIAdapter4\n");
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


	bool DX12GraphicsDevice::CreateDeviceAndCommandQueue(GraphicsContextFlags flags)
	{
		using namespace Microsoft::WRL;

		//Create device
		ComPtr<ID3D12Device> device = nullptr;
		HRESULT hr = D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
		if (SUCCEEDED(hr))
		{
			LOG_DEBUG_INFO("DX12: Created device.\n");

			hr = device.As<ID3D12Device5>(&m_Device);
			if (FAILED(hr))
			{
				LOG_DEBUG_ERROR("DX12: Failed to retrive ID3D12Device5\n");
				return false;
			}

			//Create debug interface
			if (flags & GRAPHICS_CONTEXT_FLAG_DEBUG)
			{
				hr = m_Device->QueryInterface<ID3D12DebugDevice1>(&m_DebugDevice);
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

		//Create commandqueue
		D3D12_COMMAND_QUEUE_DESC qDesc = {};
		qDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		qDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		qDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		qDesc.NodeMask = 0;

		hr = m_Device->CreateCommandQueue(&qDesc, IID_PPV_ARGS(&m_Queue));
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Could not create CommandQueue.\n");
			return false;
		}
		else
		{
			LOG_DEBUG_INFO("DX12: Created CommandQueue.\n");

			m_GPUWaitEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (m_GPUWaitEvent == 0)
			{
				LOG_DEBUG_INFO("DX12: Failed to create GPU-event handle.\n");
				return false;
			}
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


	bool DX12GraphicsDevice::CreateCommandList()
	{
		m_pCommandList = DBG_NEW DX12CommandList(m_Device.Get(), COMMAND_LIST_TYPE_GRAPHICS);
		m_pCommandList->Reset();
		return true;
	}


	bool DX12GraphicsDevice::CreateSwapChain(IWindow* pWindow)
	{
		using namespace Microsoft::WRL;

		DXGI_SWAP_CHAIN_DESC1 desc = {};
		memset(&desc, 0, sizeof(desc));
		desc.BufferCount = m_NumBackbuffers;
		desc.Format = m_BackBufferFormat;
		desc.Width = pWindow->GetWidth();
		desc.Height = pWindow->GetHeight();
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
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
			//Retrive newer interface
			if (FAILED(swapChain.As<IDXGISwapChain4>(&m_SwapChain)))
			{
				LOG_DEBUG_ERROR("DX12: Failed to retrive IDXGISwapChain4.\n");
				return false;
			}

			//No fullscreen with ALT+ENTER
			if (FAILED(m_Factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER)))
			{
				LOG_DEBUG_ERROR("DX12: Failed to retrive disable ALT+ENTER fullscreen.\n");
				return false;
			}

			m_CurrentBackBuffer = m_SwapChain->GetCurrentBackBufferIndex();

			//Init fencevalues
			m_FenceValues.resize(m_NumBackbuffers);
			for (uint32 i = 0; i < m_NumBackbuffers; i++)
				m_FenceValues[i] = 0;

			//Create the rendertarget-objects
			m_BackBuffers.resize(m_NumBackbuffers);
			for (uint32 i = 0; i < m_NumBackbuffers; i++)
				m_BackBuffers[i] = DBG_NEW DX12Texture2D(nullptr);

			LOG_DEBUG_INFO("DX12: Created swapchain\n");
		}

		return true;
	}


	bool DX12GraphicsDevice::CreateDescriptorHeaps()
	{
		//Create descriptor-allocator
		m_pRTAllocator =			DBG_NEW DX12DescriptorAllocator(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 32, false);
		m_pDSAllocator =			DBG_NEW DX12DescriptorAllocator(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1024, false);
		m_pResourceAllocator =		DBG_NEW DX12DescriptorAllocator(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, false);
		m_pSamplerAllocator =		DBG_NEW DX12DescriptorAllocator(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 128, false);

		// Create null descriptors
		{
			//Null sampler
			D3D12_SAMPLER_DESC desc = {};
			desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			m_NullSampler = m_pSamplerAllocator->Allocate();
			m_Device->CreateSampler(&desc, m_NullSampler.CPU);
		}

		{
			//Null constantbuffer
			D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
			m_NullCBV = m_pResourceAllocator->Allocate();
			m_Device->CreateConstantBufferView(&desc, m_NullCBV.CPU);
		}

		{
			//Null shaderresource
			D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
			desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			desc.Format = DXGI_FORMAT_R32_UINT;
			desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			m_NullSRV = m_pResourceAllocator->Allocate();
			m_Device->CreateShaderResourceView(nullptr, &desc, m_NullSRV.CPU);
		}

		return true;
	}


	bool DX12GraphicsDevice::InitBackBuffers()
	{
		using namespace Microsoft::WRL;

		//Create rendertarget desc
		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		memset(&desc, 0, sizeof(desc));

		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		desc.Format = m_BackBufferFormat;
		desc.Texture2D.MipSlice = 0;
		desc.Texture2D.PlaneSlice = 0;

		//Retrive textures and create views
		for (uint32 i = 0; i < m_NumBackbuffers; i++)
		{
			ComPtr<ID3D12Resource> backBuffer = nullptr;
			if (FAILED(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer))))
			{
				LOG_DEBUG_ERROR("DX12: Failed to retrive backbuffer '%d'.", i);
				return false;
			}

			DX12DescriptorHandle descriptor = m_pRTAllocator->Allocate();
			m_Device->CreateRenderTargetView(backBuffer.Get(), &desc, descriptor.CPU);

			m_BackBuffers[i]->SetResource(backBuffer.Get());
			m_BackBuffers[i]->SetDescriptorHandle(descriptor);
		}

		LOG_DEBUG_INFO("DX12: Created textures and descriptors for backbuffers\n");
		return true;
	}


	bool DX12GraphicsDevice::InternalOnEvent(const Event& event)
	{
		if (event.Type == EVENT_TYPE_WINDOW_RESIZE)
		{
			//Make sure frames are finished
			WaitForGPU();

			LOG_SYSTEM_INFO("Resize - w: %d, h: %d\n", event.WindowResize.Width, event.WindowResize.Height);

			//Resize the swapchain on resize
			ReleaseBackBuffers();
			HRESULT hr = m_SwapChain->ResizeBuffers(0, event.WindowResize.Width, event.WindowResize.Height, DXGI_FORMAT_UNKNOWN, m_BackBufferFlags);
			if (FAILED(hr))
			{
				LOG_DEBUG_ERROR("DX12: Failed to resize window\n");
			}

			InitBackBuffers();
		}

		return false;
	}
}
#endif