#include "LambdaPch.h"
#include "VKNRenderPass.h"
#include "VKNFramebuffer.h"
#include "VKNTexture.h"
#include "VKNDevice.h"
#include "VKNConversions.inl"

namespace Lambda
{
	//-------------
	//VKNRenderPass
	//-------------

	/*VKNRenderPass::VKNRenderPass(VKNDevice* pDevice, const RenderPassDesc& desc)
		: DeviceObjectBase<VKNDevice, IRenderPass>(pDevice),
		m_RenderPass(VK_NULL_HANDLE),
		m_Framebuffer(VK_NULL_HANDLE),
		m_FramebufferExtent(),
		m_Desc(),
		m_ClearValues()
	{
		//Add a ref to the refcounter
		this->AddRef();
		Init(desc);
	}
	
	
	VKNRenderPass::~VKNRenderPass()
	{
		if (m_RenderPass != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(m_pDevice->GetVkDevice(), m_RenderPass, nullptr);
			m_RenderPass = VK_NULL_HANDLE;
		}

		LOG_DEBUG_INFO("Vulkan: Destroyed RenderPass\n");
	}



	void VKNRenderPass::Init(const RenderPassDesc& desc)
	{
		
	}


	void VKNRenderPass::SetRenderTargets(const ITexture* const* const ppRenderTargets, uint32 numRenderTargets, const ITexture* pDepthStencil)
	{
		LAMBDA_ASSERT(numRenderTargets == m_Desc.NumRenderTargets);

		
	}


	void VKNRenderPass::SetClearValues(float color[4], float depth, uint8 stencil)
	{
		for (uint32 i = 0; i < m_Desc.NumRenderTargets; i++)
			memcpy(m_ClearValues[i].color.float32, color, sizeof(float) * 4);

		m_ClearValues[m_Desc.NumRenderTargets].depthStencil.depth	= depth;
		m_ClearValues[m_Desc.NumRenderTargets].depthStencil.stencil = uint32(stencil);
	}


	void* VKNRenderPass::GetNativeHandle() const
	{
		return reinterpret_cast<void*>(m_RenderPass);
	}


	const RenderPassDesc& VKNRenderPass::GetDesc() const
	{
		return m_Desc;
	}*/
}
