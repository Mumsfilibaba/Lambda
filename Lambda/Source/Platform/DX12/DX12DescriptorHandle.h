#pragma once
#include <Types.h>
#if defined(LAMBDA_PLAT_WINDOWS)
	#include <d3d12.h>
namespace Lambda
{
	struct DX12DescriptorHandle
	{
		D3D12_CPU_DESCRIPTOR_HANDLE CPU = { 0 };
		D3D12_GPU_DESCRIPTOR_HANDLE GPU = { 0 };
		uint32 Index = 0;
	};
}
#endif