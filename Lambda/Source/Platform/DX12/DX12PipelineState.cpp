#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12Device.h"
	#include "DX12PipelineState.h"
	#include "DX12Shader.h"

namespace Lambda
{
	DX12PipelineState::DX12PipelineState(DX12Device* pDevice, const SPipelineStateDesc& desc)
		: CDeviceObjectBase<DX12Device, IPipelineState>(pDevice),
		m_State(nullptr),
		m_RootSignature(nullptr)
	{
		LAMBDA_ASSERT(pDevice != nullptr);
		Init(desc);
	}


	void DX12PipelineState::CreateShaderVariableTable(IShaderVariableTable**)
	{
	}


	void DX12PipelineState::SetName(const char* pName)
	{
		m_Name = std::string(pName);
	}


	void* DX12PipelineState::GetNativeHandle() const
	{
		return m_State.Get();
	}

	
	const SPipelineStateDesc& DX12PipelineState::GetDesc() const
	{
		return SPipelineStateDesc();
	}


	void DX12PipelineState::Init(const SPipelineStateDesc& desc)
	{
		using namespace Microsoft::WRL;

		//Create default rootsignature
		{
			//Default is descriptors per stage 
			constexpr uint32 descriptorCount = 8;

			//Samplers
			D3D12_DESCRIPTOR_RANGE samplerRange = {};
			samplerRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
			samplerRange.BaseShaderRegister = 0;
			samplerRange.RegisterSpace = 0;
			samplerRange.NumDescriptors = descriptorCount;
			samplerRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			constexpr uint32 descriptorRangeCount = 3;
			D3D12_DESCRIPTOR_RANGE descriptorRanges[descriptorRangeCount];
			descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			descriptorRanges[0].BaseShaderRegister = 0;
			descriptorRanges[0].RegisterSpace = 0;
			descriptorRanges[0].NumDescriptors = descriptorCount;
			descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			descriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			descriptorRanges[1].BaseShaderRegister = 0;
			descriptorRanges[1].RegisterSpace = 0;
			descriptorRanges[1].NumDescriptors = descriptorCount;
			descriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			descriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			descriptorRanges[2].BaseShaderRegister = 0;
			descriptorRanges[2].RegisterSpace = 0;
			descriptorRanges[2].NumDescriptors = descriptorCount;
			descriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


			constexpr uint32 paramCount = 2 * 5; // 2: resources and sampler, 5: all shaderstages except compute
			D3D12_ROOT_PARAMETER params[paramCount];
			params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			params[0].DescriptorTable.NumDescriptorRanges = descriptorRangeCount;
			params[0].DescriptorTable.pDescriptorRanges = descriptorRanges;

			params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			params[1].DescriptorTable.NumDescriptorRanges = 1;
			params[1].DescriptorTable.pDescriptorRanges = &samplerRange;

			params[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			params[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_HULL;
			params[2].DescriptorTable.NumDescriptorRanges = descriptorRangeCount;
			params[2].DescriptorTable.pDescriptorRanges = descriptorRanges;

			params[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			params[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_HULL;
			params[3].DescriptorTable.NumDescriptorRanges = 1;
			params[3].DescriptorTable.pDescriptorRanges = &samplerRange;

			params[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			params[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_DOMAIN;
			params[4].DescriptorTable.NumDescriptorRanges = descriptorRangeCount;
			params[4].DescriptorTable.pDescriptorRanges = descriptorRanges;

			params[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			params[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_DOMAIN;
			params[5].DescriptorTable.NumDescriptorRanges = 1;
			params[5].DescriptorTable.pDescriptorRanges = &samplerRange;

			params[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			params[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
			params[6].DescriptorTable.NumDescriptorRanges = descriptorRangeCount;
			params[6].DescriptorTable.pDescriptorRanges = descriptorRanges;

			params[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			params[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
			params[7].DescriptorTable.NumDescriptorRanges = 1;
			params[7].DescriptorTable.pDescriptorRanges = &samplerRange;

			params[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			params[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			params[8].DescriptorTable.NumDescriptorRanges = descriptorRangeCount;
			params[8].DescriptorTable.pDescriptorRanges = descriptorRanges;

			params[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			params[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			params[9].DescriptorTable.NumDescriptorRanges = 1;
			params[9].DescriptorTable.pDescriptorRanges = &samplerRange;

			D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
			rootSignatureDesc.NumStaticSamplers = 0;
			rootSignatureDesc.NumParameters = paramCount;
			rootSignatureDesc.pParameters = params;
			rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
			if (FAILED(hr))
			{
#if defined(LAMBDA_DEBUG)
				const char* pMessage = reinterpret_cast<const char*>(error->GetBufferPointer());
				LOG_RENDER_API_ERROR("DX12: Failed to serialize RootSignature. Error-message:\n%s\n", pMessage);
#endif
				return;
			}

			hr = m_pDevice->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
			if (FAILED(hr))
			{
				LOG_RENDER_API_ERROR("DX12: Failed to create RootSignature\n");
				return;
			}
			else
			{
				LOG_RENDER_API_INFO("DX12: Created RootSignature\n");
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
			DX12Shader* pVS = reinterpret_cast<DX12Shader*>(desc.GraphicsPipeline.pVertexShader);
			DX12Shader* pPS = reinterpret_cast<DX12Shader*>(desc.GraphicsPipeline.pPixelShader);

			//Create shader
			D3D12_SHADER_BYTECODE vertexShader	= { pVS->GetShaderBlobData(), pVS->GetShaderBlobSize() };
			D3D12_SHADER_BYTECODE pixelShader	= { pPS->GetShaderBlobData(), pPS->GetShaderBlobSize() };

			//Define pipelinestate
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
			psoDesc.pRootSignature = m_RootSignature.Get();
			psoDesc.VS = vertexShader;
			psoDesc.PS = pixelShader;
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			psoDesc.DepthStencilState.DepthEnable = false;
			psoDesc.DepthStencilState.StencilEnable = false;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;

			HRESULT hr = m_pDevice->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_State));
			if (FAILED(hr))
			{
				LOG_RENDER_API_ERROR("DX12: Failed to create pipelinestate\n");
			}
			else
			{
				LOG_RENDER_API_INFO("DX12: Created pipelinestate\n");
			}
		}
	}
}
#endif
