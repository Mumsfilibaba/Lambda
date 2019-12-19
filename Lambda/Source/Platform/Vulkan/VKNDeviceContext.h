#pragma once
#include "Graphics/Core/CDeviceContextBase.h"
#include "VKNBuffer.h"
#include "VKNTexture.h"
#include "VKNPipelineState.h"
#include "VKNShaderVariableTable.h"
#include "VKNResourceLayoutTracker.h"
#include "VKNUtilities.h"

namespace Lambda
{
    class VKNDevice;
	class VKNUploadAllocator;

	//------------------
	//DeviceContextState
	//------------------

	enum EDeviceContextState : uint32
	{
		DEVICE_CONTEXT_STATE_UNKNOWN	= 0,
		DEVICE_CONTEXT_STATE_WAITING	= (1 << 0),
		DEVICE_CONTEXT_STATE_RECORDING	= (1 << 1),
		DEVICE_CONTEXT_STATE_RENDERPASS	= (1 << 2) | DEVICE_CONTEXT_STATE_RECORDING,
	};

	//----------------
	//VKNDeviceContext
	//----------------

    class VKNDeviceContext final : public CDeviceContextBase<VKNDevice, VKNTexture, VKNBuffer, VKNPipelineState, VKNShaderVariableTable>
    {
        friend class VKNDevice;    
		using TDeviceContext = CDeviceContextBase<VKNDevice, VKNTexture, VKNBuffer, VKNPipelineState, VKNShaderVariableTable>;

	private:

		//----------------
		//VKNFrameResource
		//----------------

		struct VKNFrameResource
		{
			VkFence				Fence			= VK_NULL_HANDLE;
			VkCommandBuffer		CommandBuffer	= VK_NULL_HANDLE;
			VKNUploadAllocator*	pBufferUpload	= nullptr;
			VKNUploadAllocator*	pTextureUpload	= nullptr;
		};

    public:
        LAMBDA_NO_COPY(VKNDeviceContext);
        
        VKNDeviceContext(VKNDevice* pVkDevice, EDeviceContextType type);
        ~VKNDeviceContext();
        
        virtual void ClearRenderTarget(ITexture* pRenderTarget, float color[4]) override final;
        virtual void ClearDepthStencil(ITexture* pDepthStencil, float depth, uint8 stencil) override final;
        
		virtual void SetRendertargets(ITexture* const* ppRenderTargets, uint32 numRenderTargets, ITexture* pDepthStencil) override final;
        virtual void SetViewports(const SViewport* pViewports, uint32 numViewports) override final;
        virtual void SetScissorRects(const SRectangle* pScissorRect, uint32 numRects) override final;
        virtual void SetVertexBuffers(IBuffer* const * pBuffers, uint32 numBuffers, uint32 slot) override final;
        virtual void SetIndexBuffer(IBuffer* pBuffer, EFormat format) override final;
        virtual void SetPipelineState(IPipelineState* pPipelineState) override final;
        virtual void SetShaderVariableTable(IShaderVariableTable* pVariableTable) override final;
        virtual void SetConstantBlocks(EShaderStage stage, uint32 offset, uint32 sizeInBytes, void* pData) override final;

        virtual void UpdateBuffer(IBuffer* pResource, const SResourceData& data) override final;
        virtual void UpdateTexture(ITexture* pResource, const SResourceData& data, uint32 mipLevel) override final;
        
        virtual void CopyBuffer(IBuffer* pDst, IBuffer* pSrc) override final;

		virtual void MapBuffer(IBuffer* pBuffer, uint32 mapFlags, void** ppData) override final;
		virtual void UnmapBuffer(IBuffer* pBuffer) override final;

		virtual void ResolveTexture(ITexture* pDst, uint32 dstMipLevel, ITexture* pSrc, uint32 srcMipLevel) override final;

		virtual void GenerateMipLevels(ITexture* pTexture) override final;

		virtual void TransitionTextureStates(const STextureTransitionBarrier* pBarriers, uint32 numBarriers) override final;

		virtual void Draw(uint32 vertexCount, uint32 startVertex) override final;
		virtual void DrawIndexed(uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation) override final;
        virtual void DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation) override final;
        virtual void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation) override final;
        
		virtual void ExecuteDefferedContext(IDeviceContext* pContext) override final;

		virtual void ResetQuery(IQuery* pQuery) override final;
		virtual void WriteTimeStamp(IQuery* pQuery) override final;

        virtual void Flush() override final;
        virtual void* GetNativeHandle() const override final;
		virtual void SetName(const char* pName) override final;
		       
		void CopyBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkBuffer srcBuffer, VkDeviceSize srcOffset, VkDeviceSize sizeInBytes);
		void CopyBufferToImage(VkImage image, uint32_t mipLevel, VkImageAspectFlags aspectFlags, uint32 width, uint32 height, uint32 depth, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize sizeInBytes);
		void BlitTexture(VKNTexture* pDst, uint32 dstWidth, uint32 dstHeight, uint32 dstMipLevel, VKNTexture* pSrc, uint32 srcWidth, uint32 srcHeight, uint32 srcMipLevel);
        
		void AddWaitSemaphore(VkSemaphore waitSemaphore, VkPipelineStageFlags waitDstStageMask);
		void AddSignalSemaphore(VkSemaphore signalSemaphore);
		
		void CommitPipelineState();
		void CommitResources();
		void CommitVertexBuffers();
		void CommitIndexBuffer();
		void CommitRenderTargetsAndDepthStencil();
		void CommitViewports();
		void CommitScissorRects();
        

        _forceinline VkFence GetVkFence() const					
		{ 
			return m_pCurrentFrameResource->Fence; 
		}


		_forceinline VkCommandBuffer GetVkCommandBuffer() const	
		{ 
			return m_pCurrentFrameResource->CommandBuffer; 
		}
    private:
        void Init(EDeviceContextType type);
		void QueryCommandBuffer();
		void EndCommandBuffer();
		void BeginRenderPass();
        void EndRenderPass();
		void FlushResourceBarriers();
		void PrepareForDraw();
        void TransitionBuffer(const IBuffer* pBuffer, EResourceState state);
		void TransitionTexture(const VKNTexture* pVkTexture, VkImageLayout layout, uint32 mipLevel);


		_forceinline bool IsInsideRenderPass() 
		{ 
			return m_ContextState == DEVICE_CONTEXT_STATE_RENDERPASS; 
		}
    private:
		std::vector<VkSemaphore> m_SignalSemaphores;
        std::vector<VkSemaphore> m_WaitSemaphores;
        std::vector<VkPipelineStageFlags> m_WaitDstStageMasks;
		VKNFrameResource* m_pFrameResources;
		VKNFrameResource* m_pCurrentFrameResource;
		VKNResourceLayoutTracker* m_pResourceTracker;
        VkCommandPool m_VkCommandPool;
        VkRenderPass m_VkRenderPass;
        VkFramebuffer m_VkFramebuffer;
		VkPipeline m_VkPipeline;
		VkPipelineLayout m_VkPipelineLayout;
		EDeviceContextState m_ContextState;
		uint32 m_MaxNumCommands;
		uint32 m_NumCommands;
        uint32 m_NumFrameResources;
        uint32 m_FrameIndex;
		bool m_CommitScissorRects;
		bool m_CommitViewports;
		bool m_CommitVertexBuffers;
		bool m_CommitIndexBuffer;
    };
}
