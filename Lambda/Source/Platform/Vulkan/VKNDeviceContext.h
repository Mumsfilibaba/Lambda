#pragma once
#include "Graphics/Core/IDeviceContext.h"
#include "Graphics/Core/DeviceObjectBase.h"
#include <string>
#include "VKNPipelineState.h"
#include "VKNShaderVariableTable.h"
#include "VKNUtilities.h"

namespace Lambda
{
    class VKNDevice;
    class VKNTexture;
	class IVKNAllocator;
    class VKNRenderPass;
	class VKNUploadBuffer;

	//----------------
	//VKNDeviceContext
	//----------------

    class VKNDeviceContext final : public DeviceObjectBase<VKNDevice, IDeviceContext>
    {
        friend class VKNDevice;
        
    public:
        LAMBDA_NO_COPY(VKNDeviceContext);
        
        VKNDeviceContext(VKNDevice* pDevice, IVKNAllocator* pAllocator, DeviceContextType type);
        ~VKNDeviceContext();
        
        virtual void Begin() override final;

        virtual void ResetQuery(IQuery* pQuery) override final;
        virtual void WriteTimeStamp(IQuery* pQuery, PipelineStage stage) override final;

        virtual void ClearRenderTarget(ITexture* pRenderTarget, float color[4]) override final;
        virtual void ClearDepthStencil(ITexture* pDepthStencil, float depth, uint8 stencil) override final;
        
		virtual void SetRendertargets(const ITexture* const * ppRenderTargets, uint32 numRendertargets, const ITexture* pDepthStencil) override final;
        virtual void SetViewports(const Viewport* pViewports, uint32 numViewports) override final;
        virtual void SetScissorRects(const Rectangle* pScissorRect, uint32 numRects) override final;
        virtual void SetVertexBuffers(IBuffer* const * pBuffers, uint32 numBuffers, uint32 slot) override final;
        virtual void SetIndexBuffer(IBuffer* pBuffer, Format format) override final;
        virtual void SetPipelineState(IPipelineState* pPipelineState) override final;
        virtual void SetShaderVariableTable(IShaderVariableTable* pVariableTable) override final;
        virtual void SetConstantBlocks(ShaderStage stage, uint32 offset, uint32 sizeInBytes, void* pData) override final;

        virtual void UpdateBuffer(IBuffer* pResource, const ResourceData* pData) override final;
        virtual void UpdateTexture(ITexture* pResource, const ResourceData* pData, uint32 mipLevel) override final;
        
        virtual void CopyBuffer(IBuffer* pDst, IBuffer* pSrc) override final;

		virtual void MapBuffer(IBuffer* pBuffer, void** ppData) override final;
		virtual void Unmap(IBuffer* pBuffer) override final;

		virtual void ResolveTexture(ITexture* pDst, uint32 dstMipLevel, ITexture* pSrc, uint32 srcMipLevel) override final;

		virtual void Draw(uint32 vertexCount, uint32 startVertex) override final;
		virtual void DrawIndexed(uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation) override final;
        virtual void DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation) override final;
        virtual void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation) override final;
        
		virtual void ExecuteDefferedContext(IDeviceContext* pContext) override final;

        virtual void End() override final;
        virtual void Flush() override final;
        virtual void ClearState() override final;
        
        virtual void SetName(const char* pName) override final;
        
		virtual DeviceContextType GetType() const override final;
        virtual void* GetNativeHandle() const override final;
		
		void TransitionBuffer(const IBuffer* pBuffer, ResourceState state);
		void TransitionTexture(const ITexture* pTexture, ResourceState state, uint32 startMipLevel, uint32 numMipLevels);
        
        void AddWaitSemaphore(VkSemaphore waitSemaphore, VkPipelineStageFlags waitDstStageMask);
		void AddSignalSemaphore(VkSemaphore signalSemaphore);
		void CommitAndTransitionResources();
        void BlitTexture(VKNTexture* pDst, uint32 dstWidth, uint32 dstHeight, uint32 dstMipLevel, VKNTexture* pSrc, uint32 srcWidth, uint32 srcHeight, uint32 srcMipLevel);
        
        inline VkFence GetVkFence() const					{ return m_CurrentFence; }
		inline VkCommandBuffer GetVkCommandBuffer() const	{ return m_CurrentCommandBuffer; }
    private:
        void Init(IVKNAllocator* pAllocator, DeviceContextType type);
        void BeginRenderPass();
        void EndRenderPass();
		inline bool HasRenderPassInstance() { return m_HasRenderPass; }
    private:
        VkCommandPool	    m_CommandPool;
        VkCommandBuffer*	m_pCommandBuffers;
        VkCommandBuffer     m_CurrentCommandBuffer;
        VkFence*            m_pFences;
        VkFence             m_CurrentFence;
        uint32              m_NumCommandBuffers;
        uint32              m_FrameIndex;
		IVKNAllocator*	    m_pAllocator;
        VKNUploadBuffer*    m_pBufferUpload;
        VKNUploadBuffer*    m_pTextureUpload;
		std::vector<VkSemaphore>			m_SignalSemaphores;
        std::vector<VkSemaphore>			m_WaitSemaphores;
        std::vector<VkPipelineStageFlags>	m_WaitDstStageMasks;
        DeviceContextType	m_Type;
        std::string		    m_Name;
		//State
		bool			m_HasRenderPass;
        VkRenderPass    m_RenderPass;
        VkFramebuffer   m_Framebuffer;
		const VKNTexture* m_DepthStencil;
		const VKNTexture* m_ppRenderTargets[LAMBDA_MAX_RENDERTARGET_COUNT];
		AutoRef<VKNPipelineState> m_PipelineState;
		AutoRef<VKNShaderVariableTable> m_VariableTable;
    };
}
