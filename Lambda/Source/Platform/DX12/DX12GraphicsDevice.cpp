#include "LambdaPch.h"
#include "Utilities/StringHelper.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#pragma comment(lib, "dxguid.lib")

	#include "DX12GraphicsDevice.h"
	#include "DX12CommandList.h"
	#include "DX12PipelineState.h"
	#include "DX12Shader.h"
	#include "DX12Buffer.h"
	#include "DX12Texture.h"
	#include "DX12SamplerState.h"
	#define INITAL_FENCE_VALUE 0

namespace Lambda
{
	DX12GraphicsDevice::DX12GraphicsDevice(IWindow* pWindow, const GraphicsDeviceDesc& desc)
		: m_Device(nullptr),
		m_DXRDevice(nullptr),
		m_Debug(nullptr),
		m_Adapter(nullptr),
		m_SwapChain(nullptr),
		m_Factory(nullptr),
		m_pCommandList(nullptr),
		m_DirectQueue(),
		m_ComputeQueue(),
		m_CopyQueue(),
		m_RTAllocator(),
		m_DSAllocator(),
		m_ResourceAllocator(),
		m_SamplerAllocator(),
		m_DXRSupported(false),
		m_BackBufferFlags(0),
		m_NumBackbuffers(0),
		m_CurrentBackBuffer(0),
		m_BackBufferHeight(0),
		m_BackBufferWidth(0)
	{
		LAMBDA_ASSERT(s_pInstance == nullptr);
		s_pInstance = this;

		LOG_SYSTEM_INFO("Creating DX12GraphicsDevice\n");
		
		//TODO: Allow tearing?
		m_BackBufferFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_NumBackbuffers = 3;

		Init(pWindow, desc);
	}


	DX12GraphicsDevice::~DX12GraphicsDevice()
	{
		WaitForGPU();

		if (m_SwapChain.Get())
		{
			m_SwapChain->SetFullscreenState(false, nullptr);
		}

		ICommandList* pList = m_pCommandList;
		DestroyCommandList(&pList);
		m_pCommandList = nullptr;

		for (uint32 i = 0; i < m_NumBackbuffers; i++)
		{
			ITexture* pTexture = m_BackBuffers[i];
			DestroyTexture(&pTexture);
			m_BackBuffers[i] = nullptr;
		}

		Microsoft::WRL::ComPtr<IDXGIDebug1> debugDevice;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debugDevice))))
		{
			debugDevice->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL| DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}


	void DX12GraphicsDevice::CreateCommandList(ICommandList** ppList, CommandListType type)
	{
		*ppList = DBG_NEW DX12CommandList(m_Device.Get(), type, m_NullSampler, m_NullSRV, m_NullUAV, m_NullCBV);
	}


	void DX12GraphicsDevice::CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc)
	{
		//Create resource
		DX12Buffer* pBuffer = DBG_NEW DX12Buffer(m_Device.Get(), desc);

		//Set initaldata if there are any
		if (pInitalData != nullptr)
		{
			if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
			{
				//Set initial data
				if (pInitalData != nullptr)
				{
					void* pCPU = nullptr;
					pBuffer->Map(&pCPU);
					memcpy(pCPU, pInitalData->pData, pInitalData->SizeInBytes);
					pBuffer->Unmap();
				}
			}
			else if(desc.Usage == RESOURCE_USAGE_DEFAULT)
			{
				//Copy data
				m_pCommandList->TransitionBuffer(pBuffer, RESOURCE_STATE_COPY_DEST);
				m_pCommandList->UpdateBuffer(pBuffer, pInitalData);

				//Execute and wait for GPU before creating
				m_pCommandList->Close();
				m_DirectQueue.ExecuteCommandLists(&m_pCommandList, 1);

				WaitForGPU();
				m_pCommandList->Reset();
			}
		}

		//Create constantbufferview
		if (desc.Flags & BUFFER_FLAGS_CONSTANT_BUFFER)
		{
			//Allocate and set descriptor
			DX12DescriptorHandle hDescriptor = m_ResourceAllocator.Allocate();
			pBuffer->SetDescriporHandle(hDescriptor);

			//Create view
			D3D12_CONSTANT_BUFFER_VIEW_DESC vDesc = {};
			vDesc.BufferLocation = pBuffer->GetGPUVirtualAdress();
			vDesc.SizeInBytes = desc.SizeInBytes;
			m_Device->CreateConstantBufferView(&vDesc, hDescriptor.CPU);
		}

		(*ppBuffer) = pBuffer;
	}


	void DX12GraphicsDevice::CreateTexture(ITexture** ppTexture, const ResourceData* pInitalData, const TextureDesc& desc)
	{
		//Return early if errors
		if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
		{
			LOG_DEBUG_ERROR("Lambda Engine: Texture2D cannot have resource usage dynamic\n");
			(*ppTexture) = nullptr;
			return;
		}

		//Create resource
		DX12Texture* pTexture = DBG_NEW DX12Texture(m_Device.Get(), desc);
		
		//Set initaldata if there are any
		if (pInitalData != nullptr)
		{
			//Copy data
			m_pCommandList->TransitionTexture(pTexture, RESOURCE_STATE_COPY_DEST, 0, LAMBDA_TRANSITION_ALL_MIPS);
			m_pCommandList->UpdateTexture(pTexture, pInitalData, 0);
			m_pCommandList->TransitionTexture(pTexture, RESOURCE_STATE_RENDERTARGET_PRESENT, 0, LAMBDA_TRANSITION_ALL_MIPS);

			//Execute and wait for GPU before creating
			m_pCommandList->Close();

			ICommandList* pList = m_pCommandList;
			ExecuteCommandList(&pList, 1);

			WaitForGPU();
			m_pCommandList->Reset();
		}
		
		//DepthStencil
		if (desc.Flags & TEXTURE_FLAGS_DEPTH_STENCIL)
		{
			//Allocate depth stencil descriptor

			//TODO: Free when resource is freed
			DX12DescriptorHandle hDescriptor = m_DSAllocator.Allocate();
			pTexture->SetDescriptorHandle(hDescriptor);

			//Create view
			D3D12_DEPTH_STENCIL_VIEW_DESC vDesc = {};
			vDesc.Flags = D3D12_DSV_FLAG_NONE;
			vDesc.Format = ConvertFormat(desc.Format);
			vDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			vDesc.Texture2D.MipSlice = 0;
			m_Device->CreateDepthStencilView(pTexture->GetResource(), &vDesc, hDescriptor.CPU);
		}

		//Shader resource
		if (desc.Flags & TEXTURE_FLAGS_SHADER_RESOURCE)
		{
			//Allocate depth stencil descriptor

			//TODO: Free when resource is freed
			DX12DescriptorHandle hDescriptor = m_ResourceAllocator.Allocate();
			pTexture->SetDescriptorHandle(hDescriptor);

			//Create view
			D3D12_SHADER_RESOURCE_VIEW_DESC vDesc = {};
			vDesc.Format = ConvertFormat(desc.Format);
			vDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			vDesc.Texture2D.MipLevels = desc.MipLevels;
			vDesc.Texture2D.MostDetailedMip = 0;
			vDesc.Texture2D.PlaneSlice = 0;
			vDesc.Texture2D.ResourceMinLODClamp = 0;
			vDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

			m_Device->CreateShaderResourceView(pTexture->GetResource(), &vDesc, hDescriptor.CPU);
		}
		
		(*ppTexture) = pTexture;
	}


	void DX12GraphicsDevice::CreateShader(IShader** ppShader, const ShaderDesc& desc)
	{
		(*ppShader) = DBG_NEW DX12Shader(desc);
	}


	void DX12GraphicsDevice::CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc)
	{
		DX12DescriptorHandle hDescriptor = m_SamplerAllocator.Allocate();
		(*ppSamplerState) = DBG_NEW DX12SamplerState(m_Device.Get(), hDescriptor, desc);
	}


	void DX12GraphicsDevice::CreateGraphicsPipelineState(IGraphicsPipelineState** ppPSO, const GraphicsPipelineStateDesc& desc)
	{
		(*ppPSO) = DBG_NEW DX12GraphicsPipelineState(m_Device.Get(), desc);
	}


	void DX12GraphicsDevice::CreateRenderPass(IRenderPass** ppRenderPass, const RenderPassDesc& desc)
	{
		LAMBDA_ASSERT(ppRenderPass && &desc);
	}

	void DX12GraphicsDevice::CreatePipelineResourceState(IPipelineResourceState** ppResourceState, const PipelineResourceStateDesc& desc)
	{
	}


	void DX12GraphicsDevice::DestroyCommandList(ICommandList** ppList)
	{
		LAMBDA_ASSERT(ppList != nullptr);

		DX12CommandList* pList = reinterpret_cast<DX12CommandList*>(*ppList);
		delete pList;
		*ppList = nullptr;
	}


	void DX12GraphicsDevice::DestroyBuffer(IBuffer** ppBuffer)
	{
		LAMBDA_ASSERT(ppBuffer != nullptr);

		DX12Buffer* pBuffer = reinterpret_cast<DX12Buffer*>(*ppBuffer);
		
		//Check if constantbuffer and release descriptor
		BufferDesc desc = pBuffer->GetDesc();
		if (desc.Flags & BUFFER_FLAGS_CONSTANT_BUFFER)
		{
			DX12DescriptorHandle hDescriptor = pBuffer->GetDescriptorHandle();
			m_ResourceAllocator.Free(hDescriptor);
		}

		//Remove state from global resourcetracker
		DX12ResourceStateTracker::RemoveGlobalState(pBuffer->GetResource());

		delete pBuffer;
		*ppBuffer = nullptr;
	}


	void DX12GraphicsDevice::DestroyTexture(ITexture** ppTexture)
	{
		LAMBDA_ASSERT(ppTexture != nullptr);

		DX12Texture* pTexture = reinterpret_cast<DX12Texture*>(*ppTexture);
		TextureDesc desc = pTexture->GetDesc();

		//Check what descriptor and free it
		DX12DescriptorHandle hDescriptor = pTexture->GetDescriptorHandle();		
		if (desc.Flags & TEXTURE_FLAGS_DEPTH_STENCIL)
		{
			m_DSAllocator.Free(hDescriptor);
		}
		else if (desc.Flags & TEXTURE_FLAGS_RENDER_TARGET)
		{
			m_RTAllocator.Free(hDescriptor);
		}
		else if (desc.Flags & TEXTURE_FLAGS_SHADER_RESOURCE)
		{
			m_ResourceAllocator.Free(hDescriptor);
		}

		//Remove state from global resourcetracker
		DX12ResourceStateTracker::RemoveGlobalState(pTexture->GetResource());

		delete pTexture;
		*ppTexture = nullptr;
	}


	void DX12GraphicsDevice::DestroyShader(IShader** ppShader)
	{
		LAMBDA_ASSERT(ppShader != nullptr);

		DX12Shader* pShader = reinterpret_cast<DX12Shader*>(*ppShader);
		delete pShader;
		*ppShader = nullptr;
	}


	void DX12GraphicsDevice::DestroySamplerState(ISamplerState** ppSamplerState)
	{
		LAMBDA_ASSERT(ppSamplerState != nullptr);

		DX12SamplerState* pSamplerState = reinterpret_cast<DX12SamplerState*>(*ppSamplerState);

		//Release descriptor
		DX12DescriptorHandle hDescriptor = pSamplerState->GetDescriptorHandle();
		m_SamplerAllocator.Free(hDescriptor);

		delete pSamplerState;
		*ppSamplerState = nullptr;
	}


	void DX12GraphicsDevice::DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState)
	{
		LAMBDA_ASSERT(ppPipelineState != nullptr);

		DX12GraphicsPipelineState* pPipelineState = reinterpret_cast<DX12GraphicsPipelineState*>(*ppPipelineState);
		delete pPipelineState;
		*ppPipelineState = nullptr;
	}


	void DX12GraphicsDevice::DestroyRenderPass(IRenderPass** ppRenderPass)
	{
		LAMBDA_ASSERT(ppRenderPass);
	}


	void DX12GraphicsDevice::DestroyResourceState(IPipelineResourceState** ppResourceState)
	{
	}


	void DX12GraphicsDevice::Destroy() const
	{
		delete this;
	}


	void DX12GraphicsDevice::ExecuteCommandList(ICommandList* const * ppLists, uint32 numLists) const
	{
		m_DirectQueue.ExecuteCommandLists(reinterpret_cast<DX12CommandList* const *>(ppLists), numLists);
	}


	void DX12GraphicsDevice::ExecuteCommandListAndPresent(ICommandList* const* ppLists, uint32 numLists) const
	{
		m_DirectQueue.ExecuteCommandLists(reinterpret_cast<DX12CommandList * const*>(ppLists), numLists);
		Present();
	}


	void DX12GraphicsDevice::Present() const
	{
		m_SwapChain->Present(1, 0);
	}


	void DX12GraphicsDevice::GPUWaitForFrame() const
	{
		//Increment the fencecalue on the GPU (signal) for the current frame
		uint64 fenceValue = m_FenceValues[m_CurrentBackBuffer];
		m_DirectQueue.Signal(fenceValue);

		//Update current backbuffer
		m_CurrentBackBuffer = m_SwapChain->GetCurrentBackBufferIndex();

		//Wait if value is lower than current frame
		if (m_DirectQueue.GetCompletedFenceValue() < m_FenceValues[m_CurrentBackBuffer])
		{
			m_DirectQueue.WaitForFenceValue(m_FenceValues[m_CurrentBackBuffer]);
		}

		//Increment fencevalue
		m_FenceValues[m_CurrentBackBuffer] = fenceValue + 1;
	}


	void DX12GraphicsDevice::WaitForGPU() const
	{
		//Increment the fencecalue on the GPU (signal) for the current frame
		uint64 fenceValue = m_FenceValues[m_CurrentBackBuffer];
		m_DirectQueue.Signal(fenceValue);

		//Wait for value
		m_DirectQueue.WaitForFenceValue(fenceValue);

		//Increment fencevalue
		m_FenceValues[m_CurrentBackBuffer]++;
	}


	ITexture* DX12GraphicsDevice::GetRenderTarget() const
	{
		return m_BackBuffers[m_SwapChain->GetCurrentBackBufferIndex()];
	}


	ITexture* DX12GraphicsDevice::GetDepthStencil() const
	{
		return nullptr;
	}


	ResourceFormat DX12GraphicsDevice::GetBackBufferFormat() const
	{
		return ConvertDXGIFormat(m_BackBufferFormat);
	}


	uint32 DX12GraphicsDevice::GetBackBufferIndex() const
	{
		return m_SwapChain->GetCurrentBackBufferIndex();
	}


	uint32 DX12GraphicsDevice::GetSwapChainWidth() const
	{
		return m_BackBufferWidth;
	}


	uint32 DX12GraphicsDevice::GetSwapChainHeight() const
	{
		return m_BackBufferHeight;
	}


	GraphicsDeviceDesc DX12GraphicsDevice::GetDesc() const
	{
		return m_Desc;
	}


	void* DX12GraphicsDevice::GetNativeHandle() const
	{
		//Return the ID3D12Device-ptr
		return m_Device.Get();
	}


	void DX12GraphicsDevice::Init(IWindow* pWindow, const GraphicsDeviceDesc& desc)
	{
		if (!CreateFactory(desc.Flags)) { return; }
		if (!QueryAdaper(desc.Flags)) { return; }
		if (!CreateDeviceAndCommandQueues(desc.Flags)) { return; }
		if (!CreateSwapChain(pWindow)) { return; }
		if (!CreateCommandList()) { return; }
		if (!CreateDescriptorHeaps()) { return; }
		if (!InitBackBuffers()) { return; }
	}


	void DX12GraphicsDevice::ReleaseBackBuffers()
	{
		using namespace Microsoft::WRL;

		for (DX12Texture* pTarget : m_BackBuffers)
		{
			//Free descriptor
			m_RTAllocator.Free(pTarget->GetDescriptorHandle());
			
			//Remove resource state
			DX12ResourceStateTracker::RemoveGlobalState(pTarget->GetResource());
			pTarget->SetResource(nullptr);
		}
	}


	bool DX12GraphicsDevice::CreateFactory(uint32 flags)
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
		ComPtr<IDXGIFactory2> factory = nullptr;
		HRESULT hr = CreateDXGIFactory2(flags, IID_PPV_ARGS(&factory));
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Failed to create factory\n");
			return false;
		}
		else
		{
			if (FAILED(factory.As<IDXGIFactory5>(&m_Factory)))
			{
				LOG_DEBUG_ERROR("DX12: Failed to retrive IDXGIFactory5\n");
				return false;
			}
		}

		return true;
	}


	bool DX12GraphicsDevice::QueryAdaper(uint32 flags)
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
				if (FAILED(adapter.As<IDXGIAdapter3>(&m_Adapter)))
				{
					LOG_DEBUG_ERROR("DX12: Failed to query IDXGIAdapter3\n");
					return false;
				}
				else
				{
					DXGI_ADAPTER_DESC1 desc = {};
					m_Adapter->GetDesc1(&desc);

					std::string adaptername = WidestringToString(desc.Description);
					LOG_DEBUG_PRINT("DX12: Selected adapter: %s\n", adaptername.c_str());

					return true;
				}
			}
		}
		return true;
	}


	bool DX12GraphicsDevice::CreateDeviceAndCommandQueues(uint32 flags)
	{
		using namespace Microsoft::WRL;

		//Create device
		HRESULT hr = D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));
		if (SUCCEEDED(hr))
		{
			LOG_DEBUG_INFO("DX12: Created device.\n");

			//Check support for DXR
			m_DXRSupported = IsDXRSupported(m_Device.Get());
			if (m_DXRSupported)
			{
				hr = m_Device.As<ID3D12Device5>(&m_DXRDevice);
				if (FAILED(hr))
				{
					LOG_DEBUG_ERROR("DX12: Failed to retrive ID3D12Device5, no DXR support\n");
					return false;
				}
				else
				{
					LOG_DEBUG_PRINT("DX12: DXR is supported\n");
				}
			}
			else
			{
				LOG_DEBUG_WARNING("DX12: DXR is not supported on this system\n");
			}

			//Create debug interface
			if (flags & GRAPHICS_CONTEXT_FLAG_DEBUG)
			{
				//Retrive infoqueue
				ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
				hr = m_Device.As<ID3D12InfoQueue>(&infoQueue);
				if (FAILED(hr))
				{
					LOG_DEBUG_ERROR("DX12: Could not retrive infoqueue.\n");
					return false;
				}
				else
				{
					//Enable break on error
					infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
					//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
					//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

					//Disable some messages
					D3D12_MESSAGE_ID blockedIds[] = {
						D3D12_MESSAGE_ID_COPY_DESCRIPTORS_INVALID_RANGES //Not a valid error message on RTX hardware
					};

					D3D12_INFO_QUEUE_FILTER filter = {};
					filter.DenyList.pIDList = blockedIds;
					filter.DenyList.NumIDs = 1;

					infoQueue->AddRetrievalFilterEntries(&filter);
					infoQueue->AddStorageFilterEntries(&filter);
				}
			}
		}
		else
		{
			LOG_DEBUG_ERROR("DX12: Could not create device.\n");
			return false;
		}

		//Create queues
		bool result = false;
		result = m_DirectQueue.Init(m_Device.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT, INITAL_FENCE_VALUE);
		result = m_ComputeQueue.Init(m_Device.Get(), D3D12_COMMAND_LIST_TYPE_COMPUTE, INITAL_FENCE_VALUE);
		result = m_CopyQueue.Init(m_Device.Get(), D3D12_COMMAND_LIST_TYPE_COPY, INITAL_FENCE_VALUE);

		return result;
	}


	bool DX12GraphicsDevice::CreateCommandList()
	{
		CreateCommandList(reinterpret_cast<ICommandList**>(&m_pCommandList), COMMAND_LIST_TYPE_GRAPHICS);

		m_pCommandList->SetName("Device GraphicsCommandList");
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
		HRESULT hr = m_Factory->CreateSwapChainForHwnd(m_DirectQueue.GetQueue(), hWnd, &desc, nullptr, nullptr, &swapChain);
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Failed to create swapchain.\n");
			return false;
		}
		else
		{
			//Retrive newer interface
			if (FAILED(swapChain.As<IDXGISwapChain3>(&m_SwapChain)))
			{
				LOG_DEBUG_ERROR("DX12: Failed to retrive IDXGISwapChain3.\n");
				return false;
			}

			//Set size of swapchain
			m_BackBufferWidth = desc.Width;
			m_BackBufferHeight = desc.Height;

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
				m_FenceValues[i] = INITAL_FENCE_VALUE + 1;

			//Create the rendertarget-objects
			m_BackBuffers.resize(m_NumBackbuffers);
			for (uint32 i = 0; i < m_NumBackbuffers; i++)
				m_BackBuffers[i] = DBG_NEW DX12Texture(nullptr);

			LOG_DEBUG_INFO("DX12: Created swapchain\n");
		}

		return true;
	}


	bool DX12GraphicsDevice::CreateDescriptorHeaps()
	{
		//Create descriptor-allocator
		m_RTAllocator.Init(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 32, false);
		m_DSAllocator.Init(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 32, false);
		m_ResourceAllocator.Init(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, false);
		m_SamplerAllocator.Init(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 128, false);

		// Create null descriptors
		{
			//Null sampler
			D3D12_SAMPLER_DESC desc = {};
			desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			m_NullSampler = m_SamplerAllocator.Allocate();
			m_Device->CreateSampler(&desc, m_NullSampler.CPU);
		}

		{
			//Null constantbuffer
			D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
			m_NullCBV = m_ResourceAllocator.Allocate();
			m_Device->CreateConstantBufferView(&desc, m_NullCBV.CPU);
		}

		{
			//Null shaderresource
			D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
			desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			desc.Format = DXGI_FORMAT_R32_UINT;
			desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			m_NullSRV = m_ResourceAllocator.Allocate();
			m_Device->CreateShaderResourceView(nullptr, &desc, m_NullSRV.CPU);
		}

		{
			//Null unordered access
			D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
			uav_desc.Format = DXGI_FORMAT_R32_UINT;
			uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			m_NullUAV = m_ResourceAllocator.Allocate();
			m_Device->CreateUnorderedAccessView(nullptr, nullptr, &uav_desc, m_NullUAV.CPU);
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

			DX12DescriptorHandle descriptor = m_RTAllocator.Allocate();
			m_Device->CreateRenderTargetView(backBuffer.Get(), &desc, descriptor.CPU);

			m_BackBuffers[i]->SetResource(backBuffer.Get());
			m_BackBuffers[i]->SetDescriptorHandle(descriptor);
		}

		LOG_DEBUG_INFO("DX12: Created textures and descriptors for backbuffers\n");
		return true;
	}


	bool DX12GraphicsDevice::IsDXRSupported(ID3D12Device* pDevice)
	{
		//Check support for DXR
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 supportData = {};
		HRESULT hr = pDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &supportData, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS5));
		if (SUCCEEDED(hr))
		{
			return supportData.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
		}
		else
		{
			return false;
		}
	}


	bool DX12GraphicsDevice::InternalOnEvent(const Event& event)
	{
		if (event.Type == EVENT_TYPE_WINDOW_RESIZE)
		{
			//if size is zero then do not resize
			if (event.WindowResize.Width > 0 && event.WindowResize.Height > 0)
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
				else
				{
					InitBackBuffers();

					m_BackBufferWidth = event.WindowResize.Width;
					m_BackBufferHeight = event.WindowResize.Height;
				}
			}
		}

		return false;
	}
}
#endif
