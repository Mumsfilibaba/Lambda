#include "LambdaPch.h"
#include "Utilities/StringHelper.h"
#include "Events/WindowEvent.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#pragma comment(lib, "dxguid.lib")
	#include "DX12Device.h"
	#include "DX12DeviceContext.h"
	#include "DX12PipelineState.h"
	#include "DX12Shader.h"
	#include "DX12Buffer.h"
	#include "DX12Texture.h"
	#include "DX12SamplerState.h"
	#include "DX12Conversions.inl"
	#define INITAL_FENCE_VALUE 0

namespace Lambda
{
	//------------------
	//DX12Device
	//------------------

	DX12Device* DX12Device::s_pInstance = nullptr;

	DX12Device::DX12Device(const DeviceDesc& desc)
		: m_Device(nullptr),
		m_DXRDevice(nullptr),
		m_Debug(nullptr),
		m_Adapter(nullptr),
		m_SwapChain(nullptr),
		m_Factory(nullptr),
		m_pImmediateContext(nullptr),
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

		LOG_SYSTEM_INFO("Creating DX12Device\n");
		
		//TODO: Allow tearing?
		m_BackBufferFlags	= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		m_BackBufferFormat	= DXGI_FORMAT_R8G8B8A8_UNORM;

		Init(desc);
	}


	DX12Device::~DX12Device()
	{
		WaitForGPU();

		if (m_SwapChain.Get())
		{
			m_SwapChain->SetFullscreenState(false, nullptr);
		}

		IDeviceContext* pList = m_pImmediateContext;
		m_pImmediateContext->Release();

		for (uint32 i = 0; i < m_NumBackbuffers; i++)
		{
			ITexture* pTexture = m_BackBuffers[i];
			pTexture->Release();
			m_BackBuffers[i] = nullptr;
		}

		Microsoft::WRL::ComPtr<IDXGIDebug1> debugDevice;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debugDevice))))
		{
			debugDevice->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL| DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}


	void DX12Device::Init(const DeviceDesc& desc)
	{
		using namespace Microsoft::WRL;

		//Should we enable debuglayer
		uint32 factoryFlags = 0;
		if (desc.Flags & DEVICE_FLAG_DEBUG)
		{
			factoryFlags = DXGI_CREATE_FACTORY_DEBUG;

			if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_Debug))))
			{
				LOG_DEBUG_ERROR("DX12: Failed to get debug interface\n");
				return;
			}
			else
			{
				m_Debug->EnableDebugLayer();
			}
		}


		//Create factory
		ComPtr<IDXGIFactory2> factory = nullptr;
		HRESULT hr = CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory));
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Failed to create factory\n");
			return;
		}
		else
		{
			if (FAILED(factory.As<IDXGIFactory5>(&m_Factory)))
			{
				LOG_DEBUG_ERROR("DX12: Failed to retrive IDXGIFactory5\n");
				return;
			}
		}


		//Get an adapter
		if (!QueryAdaper(desc.Flags)) 
		{ 
			return; 
		}


		//Create device
		hr = D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));
		if (SUCCEEDED(hr))
		{
			LOG_DEBUG_INFO("DX12: Created device.\n");
			
			//Set desc
			m_Desc = desc;

			//Check support for DXR
			m_DXRSupported = IsDXRSupported(m_Device.Get());
			if (m_DXRSupported)
			{
				hr = m_Device.As<ID3D12Device5>(&m_DXRDevice);
				if (FAILED(hr))
				{
					LOG_DEBUG_ERROR("DX12: Failed to retrive ID3D12Device5, no DXR support\n");
					return;
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
			if (desc.Flags & DEVICE_FLAG_DEBUG)
			{
				//Retrive infoqueue
				ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
				hr = m_Device.As<ID3D12InfoQueue>(&infoQueue);
				if (FAILED(hr))
				{
					LOG_DEBUG_ERROR("DX12: Could not retrive infoqueue.\n");
					return;
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
			return;
		}


		//Create queues
		m_DirectQueue.Init(m_Device.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT, INITAL_FENCE_VALUE);
		m_ComputeQueue.Init(m_Device.Get(), D3D12_COMMAND_LIST_TYPE_COMPUTE, INITAL_FENCE_VALUE);
		m_CopyQueue.Init(m_Device.Get(), D3D12_COMMAND_LIST_TYPE_COPY, INITAL_FENCE_VALUE);


		//Create swapchain
		DXGI_SWAP_CHAIN_DESC1 scDesc = {};
		memset(&scDesc, 0, sizeof(scDesc));
		scDesc.BufferCount			= m_NumBackbuffers;
		scDesc.Format				= m_BackBufferFormat;
		//scDesc.Width				= desc.pWindow->GetWidth();
		//scDesc.Height				= desc.pWindow->GetHeight();
		scDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_DISCARD;
		scDesc.SampleDesc.Count		= 1;
		scDesc.SampleDesc.Quality	= 0;
		scDesc.Flags				= 0;

		HWND hWnd = 0;// reinterpret_cast<HWND>(desc.pWindow->GetNativeHandle());
		
		ComPtr<IDXGISwapChain1> swapChain = nullptr;
		hr = m_Factory->CreateSwapChainForHwnd(m_DirectQueue.GetQueue(), hWnd, &scDesc, nullptr, nullptr, &swapChain);
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Failed to create swapchain.\n");
			return;
		}
		else
		{
			//Retrive newer interface
			if (FAILED(swapChain.As<IDXGISwapChain3>(&m_SwapChain)))
			{
				LOG_DEBUG_ERROR("DX12: Failed to retrive IDXGISwapChain3.\n");
				return;
			}

			//Set size of swapchain
			m_BackBufferWidth	= scDesc.Width;
			m_BackBufferHeight	= scDesc.Height;

			//No fullscreen with ALT+ENTER
			if (FAILED(m_Factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER)))
			{
				LOG_DEBUG_ERROR("DX12: Failed to retrive disable ALT+ENTER fullscreen.\n");
				return;
			}

			m_CurrentBackBuffer = m_SwapChain->GetCurrentBackBufferIndex();

			//Init fencevalues
			m_FenceValues.resize(m_NumBackbuffers);
			for (uint32 i = 0; i < m_NumBackbuffers; i++)
				m_FenceValues[i] = INITAL_FENCE_VALUE + 1;

			//Create the rendertarget-objects
			m_BackBuffers.resize(m_NumBackbuffers);
			for (uint32 i = 0; i < m_NumBackbuffers; i++)
				m_BackBuffers[i] = DBG_NEW DX12Texture(this, nullptr);

			LOG_DEBUG_INFO("DX12: Created swapchain\n");
		}


		//Create internal commandlist	
		m_pImmediateContext = DBG_NEW DX12DeviceContext(this, DEVICE_CONTEXT_TYPE_IMMEDIATE, m_NullSampler, m_NullSRV, m_NullUAV, m_NullCBV);
		m_pImmediateContext->SetName("Device GraphicsCommandList");

		
		//Create descriptor-allocator
		m_RTAllocator.Init(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 32, false);
		m_DSAllocator.Init(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 32, false);
		m_ResourceAllocator.Init(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, false);
		m_SamplerAllocator.Init(m_Device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 128, false);


		//Null sampler
		D3D12_SAMPLER_DESC samplerDesc = {};
		samplerDesc.AddressU		= D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressV		= D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressW		= D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.ComparisonFunc	= D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc.Filter			= D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		
		m_NullSampler = m_SamplerAllocator.Allocate();
		m_Device->CreateSampler(&samplerDesc, m_NullSampler.CPU);

		//Null constantbuffer
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation	= { 0 };
		cbvDesc.SizeInBytes		= 0;

		m_NullCBV = m_ResourceAllocator.Allocate();
		m_Device->CreateConstantBufferView(&cbvDesc, m_NullCBV.CPU);

		//Null shaderresource
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping	= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format					= DXGI_FORMAT_R32_UINT;
		srvDesc.ViewDimension			= D3D12_SRV_DIMENSION_BUFFER;

		m_NullSRV = m_ResourceAllocator.Allocate();
		m_Device->CreateShaderResourceView(nullptr, &srvDesc, m_NullSRV.CPU);

		//Null unordered access
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format			= DXGI_FORMAT_R32_UINT;
		uavDesc.ViewDimension	= D3D12_UAV_DIMENSION_BUFFER;

		m_NullUAV = m_ResourceAllocator.Allocate();
		m_Device->CreateUnorderedAccessView(nullptr, nullptr, &uavDesc, m_NullUAV.CPU);

		//Create backbuffers
		if (!InitBackBuffers())
		{ 
			return;
		}
	}


	void DX12Device::CreateDefferedContext(IDeviceContext** ppDefferedContext)
	{
		(*ppDefferedContext) = DBG_NEW DX12DeviceContext(this, DEVICE_CONTEXT_TYPE_DEFFERED, m_NullSampler, m_NullSRV, m_NullUAV, m_NullCBV);
	}


	void DX12Device::CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc)
	{
		//Create resource
		DX12Buffer* pBuffer = DBG_NEW DX12Buffer(this, desc);

		//Set initaldata if there are any
		if (pInitalData != nullptr)
		{
			if (desc.Usage == USAGE_DYNAMIC)
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
			else if(desc.Usage == USAGE_DEFAULT)
			{
				//Copy data
				m_pImmediateContext->TransitionBuffer(pBuffer, RESOURCE_STATE_COPY_DEST);
				m_pImmediateContext->UpdateBuffer(pBuffer, *pInitalData);

				//Execute and wait for GPU before creating
				m_pImmediateContext->End();
				m_DirectQueue.ExecuteCommandLists(&m_pImmediateContext, 1);

				WaitForGPU();
				m_pImmediateContext->Begin();
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


	void DX12Device::CreateTexture(ITexture** ppTexture, const ResourceData* pInitalData, const TextureDesc& desc)
	{
		//Return early if errors
		if (desc.Usage == USAGE_DYNAMIC)
		{
			LOG_DEBUG_ERROR("Lambda Engine: Texture2D cannot have resource usage dynamic\n");
			(*ppTexture) = nullptr;
			return;
		}

		//Create resource
		DX12Texture* pTexture = DBG_NEW DX12Texture(this, desc);
		
		//Set initaldata if there are any
		if (pInitalData != nullptr)
		{
			//Copy data
			m_pImmediateContext->TransitionTexture(pTexture, RESOURCE_STATE_COPY_DEST, 0, LAMBDA_TRANSITION_ALL_MIPS);
			m_pImmediateContext->UpdateTexture(pTexture, *pInitalData, 0);
			m_pImmediateContext->TransitionTexture(pTexture, RESOURCE_STATE_PRESENT, 0, LAMBDA_TRANSITION_ALL_MIPS);

			//Execute and wait for GPU before creating
			m_pImmediateContext->End();

			IDeviceContext* pList = m_pImmediateContext;
			ExecuteCommandList(&pList, 1);

			WaitForGPU();
			m_pImmediateContext->Begin();
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


	void DX12Device::CreateShader(IShader** ppShader, const ShaderDesc& desc)
	{
		(*ppShader) = DBG_NEW DX12Shader(this, desc);
	}


	void DX12Device::CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc)
	{
		DX12DescriptorHandle hDescriptor = m_SamplerAllocator.Allocate();
		(*ppSamplerState) = DBG_NEW DX12SamplerState(this, hDescriptor, desc);
	}


	void DX12Device::CreatePipelineState(IPipelineState** ppPSO, const PipelineStateDesc& desc)
	{
		//(*ppPSO) = DBG_NEW DX12PipelineState(m_Device.Get(), desc);
	}


	void DX12Device::CreateQuery(IQuery** ppQuery, const QueryDesc& desc)
	{
	}


	//void DX12Device::DestroyCommandList(IDeviceContext** ppList)
	//{
	//	LAMBDA_ASSERT(ppList != nullptr);

	//	DX12DeviceContext* pList = reinterpret_cast<DX12DeviceContext*>(*ppList);
	//	delete pList;
	//	*ppList = nullptr;
	//}


	//void DX12Device::DestroyBuffer(IBuffer** ppBuffer)
	//{
	//	LAMBDA_ASSERT(ppBuffer != nullptr);

	//	DX12Buffer* pBuffer = reinterpret_cast<DX12Buffer*>(*ppBuffer);
	//	
	//	//Check if constantbuffer and release descriptor
	//	BufferDesc desc = pBuffer->GetDesc();
	//	if (desc.Flags & BUFFER_FLAGS_CONSTANT_BUFFER)
	//	{
	//		DX12DescriptorHandle hDescriptor = pBuffer->GetDescriptorHandle();
	//		m_ResourceAllocator.Free(hDescriptor);
	//	}

	//	//Remove state from global resourcetracker
	//	DX12ResourceStateTracker::RemoveGlobalState(pBuffer->GetResource());

	//	delete pBuffer;
	//	*ppBuffer = nullptr;
	//}


	//void DX12Device::DestroyTexture(ITexture** ppTexture)
	//{
	//	LAMBDA_ASSERT(ppTexture != nullptr);

	//	DX12Texture* pTexture = reinterpret_cast<DX12Texture*>(*ppTexture);
	//	TextureDesc desc = pTexture->GetDesc();

	//	//Check what descriptor and free it
	//	DX12DescriptorHandle hDescriptor = pTexture->GetDescriptorHandle();		
	//	if (desc.Flags & TEXTURE_FLAGS_DEPTH_STENCIL)
	//	{
	//		m_DSAllocator.Free(hDescriptor);
	//	}
	//	else if (desc.Flags & TEXTURE_FLAGS_RENDER_TARGET)
	//	{
	//		m_RTAllocator.Free(hDescriptor);
	//	}
	//	else if (desc.Flags & TEXTURE_FLAGS_SHADER_RESOURCE)
	//	{
	//		m_ResourceAllocator.Free(hDescriptor);
	//	}

	//	//Remove state from global resourcetracker
	//	DX12ResourceStateTracker::RemoveGlobalState(pTexture->GetResource());

	//	delete pTexture;
	//	*ppTexture = nullptr;
	//}


	//void DX12Device::DestroyShader(IShader** ppShader)
	//{
	//	LAMBDA_ASSERT(ppShader != nullptr);

	//	DX12Shader* pShader = reinterpret_cast<DX12Shader*>(*ppShader);
	//	delete pShader;
	//	*ppShader = nullptr;
	//}


	//void DX12Device::DestroySamplerState(ISamplerState** ppSamplerState)
	//{
	//	LAMBDA_ASSERT(ppSamplerState != nullptr);

	//	DX12SamplerState* pSamplerState = reinterpret_cast<DX12SamplerState*>(*ppSamplerState);

	//	//Release descriptor
	//	DX12DescriptorHandle hDescriptor = pSamplerState->GetDescriptorHandle();
	//	m_SamplerAllocator.Free(hDescriptor);

	//	delete pSamplerState;
	//	*ppSamplerState = nullptr;
	//}


	//void DX12Device::DestroyGraphicsPipelineState(IGraphicsPipelineState** ppPipelineState)
	//{
	//	LAMBDA_ASSERT(ppPipelineState != nullptr);

	//	DX12PipelineState* pPipelineState = reinterpret_cast<DX12PipelineState*>(*ppPipelineState);
	//	delete pPipelineState;
	//	*ppPipelineState = nullptr;
	//}


	//void DX12Device::DestroyRenderPass(IRenderPass** ppRenderPass)
	//{
	//	LAMBDA_ASSERT(ppRenderPass);
	//}


	//void DX12Device::DestroyResourceState(IPipelineResourceState** ppResourceState)
	//{
	//}

	//void DX12Device::DestroyQuery(IQuery** ppQuery)
	//{
	//}


	//void DX12Device::Destroy() const
	//{
	//	delete this;
	//}


	void DX12Device::ExecuteCommandList(IDeviceContext* const * ppLists, uint32 numLists) const
	{
		m_DirectQueue.ExecuteCommandLists(reinterpret_cast<DX12DeviceContext* const *>(ppLists), numLists);
	}


	/*void DX12Device::PresentEnd(IDeviceContext* const* ppLists, uint32 numLists) const
	{
		m_DirectQueue.ExecuteCommandLists(reinterpret_cast<DX12DeviceContext * const*>(ppLists), numLists);
		m_SwapChain->Present(1, 0);
	}*/


	void DX12Device::GPUWaitForFrame() const
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


	void DX12Device::WaitForGPU() const
	{
		//Increment the fencecalue on the GPU (signal) for the current frame
		uint64 fenceValue = m_FenceValues[m_CurrentBackBuffer];
		m_DirectQueue.Signal(fenceValue);

		//Wait for value
		m_DirectQueue.WaitForFenceValue(fenceValue);

		//Increment fencevalue
		m_FenceValues[m_CurrentBackBuffer]++;
	}


	/*ITexture* DX12Device::GetRenderTarget() const
	{
		return m_BackBuffers[m_SwapChain->GetCurrentBackBufferIndex()];
	}


	ITexture* DX12Device::GetDepthStencil() const
	{
		return nullptr;
	}


	Format DX12Device::GetBackBufferFormat() const
	{
		return ConvertDXGIFormat(m_BackBufferFormat);
	}


	uint32 DX12Device::GetBackBufferIndex() const
	{
		return m_SwapChain->GetCurrentBackBufferIndex();
	}


	uint32 DX12Device::GetSwapChainWidth() const
	{
		return m_BackBufferWidth;
	}


	uint32 DX12Device::GetSwapChainHeight() const
	{
		return m_BackBufferHeight;
	}*/


	const DeviceProperties& DX12Device::GetProperties() const
	{
		return DeviceProperties();
	}


	const DeviceDesc& DX12Device::GetDesc() const
	{
		return m_Desc;
	}


	void* DX12Device::GetNativeHandle() const
	{
		//Return the ID3D12Device-ptr
		return m_Device.Get();
	}


	void DX12Device::ReleaseBackBuffers()
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


	bool DX12Device::QueryAdaper(uint32 flags)
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
				if (!(flags & DEVICE_FLAG_ALLOW_SOFTWARE_ADAPTER))
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


	bool DX12Device::InitBackBuffers()
	{
		using namespace Microsoft::WRL;

		//Create rendertarget desc
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		ZeroMemory(&rtvDesc, sizeof(rtvDesc));
		rtvDesc.ViewDimension			= D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Format					= m_BackBufferFormat;
		rtvDesc.Texture2D.MipSlice		= 0;
		rtvDesc.Texture2D.PlaneSlice	= 0;

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
			m_Device->CreateRenderTargetView(backBuffer.Get(), &rtvDesc, descriptor.CPU);

			m_BackBuffers[i]->SetResource(backBuffer.Get());
			m_BackBuffers[i]->SetDescriptorHandle(descriptor);
		}

		LOG_DEBUG_INFO("DX12: Created textures and descriptors for backbuffers\n");
		return true;
	}


	bool DX12Device::IsDXRSupported(ID3D12Device* pDevice)
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


	/*bool DX12Device::OnResize(const WindowResizeEvent& event)
	{
		//if size is zero then do not resize
		if (event.GetWidth() > 0 && event.GetHeight() > 0)
		{
			//Make sure frames are finished
			WaitForGPU();

			LOG_SYSTEM_INFO("Resize - w: %d, h: %d\n", event.GetWidth(), event.GetHeight());

			//Release the old buffers
			ReleaseBackBuffers();

			//Resize
			HRESULT hr = m_SwapChain->ResizeBuffers(0, event.GetWidth(), event.GetHeight(), DXGI_FORMAT_UNKNOWN, m_BackBufferFlags);
			if (FAILED(hr))
			{
				LOG_DEBUG_ERROR("DX12: Failed to resize window\n");
			}
			else
			{
				InitBackBuffers();

				m_BackBufferWidth	= event.GetWidth();
				m_BackBufferHeight	= event.GetHeight();
			}
		}

		return false;
	}*/
	
	
	DX12Device& DX12Device::Get()
	{
		LAMBDA_ASSERT(s_pInstance != nullptr);

		DX12Device* pDXDevice = reinterpret_cast<DX12Device*>(s_pInstance);
		return *pDXDevice;
	}
}
#endif
