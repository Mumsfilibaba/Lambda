#include <LambdaPch.h>
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12PipelineState.h"
	#include "DX12Shader.h"

namespace Lambda
{
	DX12GraphicsPipelineState::DX12GraphicsPipelineState(ID3D12Device5* pDevice, const GraphicsPipelineStateDesc& desc)
		: m_State(nullptr),
		m_RootSignature(nullptr),
		m_References(0)
	{
		assert(pDevice != nullptr);

		AddRef();
		Init(pDevice, desc);
	}

	DX12GraphicsPipelineState::~DX12GraphicsPipelineState()
	{
	}

	uint32 DX12GraphicsPipelineState::Release()
	{
		IOBJECT_IMPLEMENT_RELEASE(m_References);
	}

	uint32 DX12GraphicsPipelineState::AddRef()
	{
		return ++m_References;
	}

	void DX12GraphicsPipelineState::Init(ID3D12Device5* pDevice, const GraphicsPipelineStateDesc& desc)
	{
		using namespace Microsoft::WRL;

		//Create rootsignature
		{
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
			if (FAILED(hr))
			{
				LOG_DEBUG_ERROR("DX12: Failed to serialize RootSignature\n");
				return;
			}

			hr = pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
			if (FAILED(hr))
			{
				LOG_DEBUG_ERROR("DX12: Failed to create RootSignature\n");
				return;
			}
			else
			{
				LOG_DEBUG_INFO("DX12: Created RootSignature\n");
			}
		}

		//Create pipelinestate
		{
			//Define inputlayout
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			//Retrive shaders from desc
			DX12Shader* pVS = reinterpret_cast<DX12Shader*>(desc.pVertexShader);
			DX12Shader* pPS = reinterpret_cast<DX12Shader*>(desc.pPixelShader);

			//Define pipelinestate
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = { nullptr, 0 };
			psoDesc.pRootSignature = m_RootSignature.Get();
			psoDesc.VS = CD3DX12_SHADER_BYTECODE(pVS->GetShaderBlob());
			psoDesc.PS = CD3DX12_SHADER_BYTECODE(pPS->GetShaderBlob());
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			psoDesc.DepthStencilState.DepthEnable = false;
			psoDesc.DepthStencilState.StencilEnable = false;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;

			HRESULT hr = pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_State));
			if (FAILED(hr))
			{
				LOG_DEBUG_ERROR("DX12: Failed to create pipelinestate\n");
				return;
			}
			else
			{
				LOG_DEBUG_INFO("DX12: Created pipelinestate\n");
			}
		}
	}
}
#endif