#pragma once
#include "IObject.h"

#define LAMBDA_TRANSITION_ALL_MIPS uint32(-1)

namespace Lambda
{
    class IQuery;
    class IBuffer;
	class ITexture;
    class IRenderPass;
    class ISamplerState;
    class IPipelineState;

    //------------
    //IDeviceContext
    //------------

	class LAMBDA_API IDeviceContext : public IObject
	{
	public:
		LAMBDA_IOBJECT_INTERFACE(IDeviceContext);

		virtual void ClearRenderTarget(ITexture* pRenderTarget, float color[4]) = 0;
		virtual void ClearDepthStencil(ITexture* pDepthStencil, float depth, uint8 stencil) = 0;

		virtual void BeginRenderPass(IRenderPass* pRenderPass) = 0;
		virtual void EndRenderPass() = 0;

        virtual void ResetQuery(IQuery* pQuery) = 0;
        virtual void WriteTimeStamp(IQuery* pQuery, PipelineStage stage) = 0;
        
		virtual void SetViewport(const Viewport& viewport) = 0;
		virtual void SetScissorRect(const Rectangle& scissorRect) = 0;
		virtual void SetVertexBuffer(IBuffer* pBuffer, uint32 slot) = 0;
        virtual void SetIndexBuffer(IBuffer* pBuffer, Format format) = 0;
		virtual void SetConstantBlocks(ShaderStage stage, uint32 offset, uint32 sizeInBytes, void* pData) = 0;
		virtual void SetPipelineState(IPipelineState* pPiplineState) = 0;

		virtual void UpdateBuffer(IBuffer* pResource, const ResourceData* pData) = 0;
		virtual void UpdateTexture(ITexture* pResource, const ResourceData* pData, uint32 mipLevel) = 0;

        virtual void TransitionBuffer(const IBuffer* pBuffer, ResourceState state) = 0;
        virtual void TransitionTexture(const ITexture* pTexture, ResourceState state, uint32 startMipLevel, uint32 numMipLevels) = 0;
        
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
