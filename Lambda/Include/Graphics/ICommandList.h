#pragma once
#include "IObject.h"

namespace Lambda
{
    class IBuffer;
	class ITexture2D;
	class ISamplerState;
	class IGraphicsPipelineState;
	class IRenderPass;
	class IResourceState;


	class LAMBDA_API ICommandList
	{
	public:
		LAMBDA_INTERFACE(ICommandList);

		ICommandList() = default;
		~ICommandList() = default;

		virtual void ClearRenderTarget(ITexture2D* pRenderTarget, float color[4]) = 0;
		virtual void ClearDepthStencil(ITexture2D* pDepthStencil, float depth, uint8 stencil) = 0;

		virtual void BeginRenderPass(IRenderPass* pRenderPass) = 0;
		virtual void EndRenderPass() = 0;

		virtual void SetViewport(const Viewport& viewport) = 0;
		virtual void SetScissorRect(const Rectangle& scissorRect) = 0;
		virtual void SetGraphicsPipelineState(IGraphicsPipelineState* pPSO) = 0;
		virtual void SetVertexBuffer(IBuffer* pBuffer, uint32 slot) = 0;
        virtual void SetIndexBuffer(IBuffer* pBuffer) = 0;
		virtual void SetResourceState(IResourceState* pResourceState) = 0;

		virtual void UpdateBuffer(IBuffer* pResource, const ResourceData* pData) = 0;
		virtual void UpdateTexture(ITexture2D* pResource, const ResourceData* pData, uint32 subresource) = 0;

        virtual void TransitionBuffer(const IBuffer* pBuffer, ResourceState state) = 0;
        virtual void TransitionTexture(const ITexture2D* pTexture, ResourceState state) = 0;
        
		virtual void CopyBuffer(IBuffer* pDst, IBuffer* pSrc) = 0;

		virtual void DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation) = 0;
        virtual void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation) = 0;

		virtual void SetName(const char* pName) = 0;

		virtual void Close() = 0;
		virtual void Reset() = 0;
        
        virtual CommandListType GetType() const = 0;
        virtual void* GetNativeHandle() const = 0;
	};
}
