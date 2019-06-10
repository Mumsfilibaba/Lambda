#pragma once
#include "Types.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include <d3d12.h>
namespace Lambda
{
	struct DX12DescriptorHandle
	{
		inline DX12DescriptorHandle()
		{
			CPU.ptr = 0;
			GPU.ptr = 0;
			Index = (uint32)-1;
		}

		inline DX12DescriptorHandle(DX12DescriptorHandle& base, uint64 offset)
		{
			CPU = base.CPU;
			CPU.ptr += offset;

			GPU = base.GPU;
			GPU.ptr += offset;

			Index = base.Index;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE CPU;
		D3D12_GPU_DESCRIPTOR_HANDLE GPU;
		uint32 Index;
	};
}
#endif