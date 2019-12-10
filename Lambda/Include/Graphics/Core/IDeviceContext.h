#pragma once
#include "IDeviceObject.h"

#define LAMBDA_TRANSITION_ALL_MIPS uint32(-1)

namespace Lambda
{
    class IQuery;
    class IBuffer;
	class ITexture;
    class ISamplerState;
    class IPipelineState;
	class IShaderVariableTable;

    //--------------
    //IDeviceContext
    //--------------

	class LAMBDA_API IDeviceContext : public IDeviceObject
	{
	public:
		LAMBDA_IOBJECT_INTERFACE(IDeviceContext);
       
        virtual void ClearRenderTarget(ITexture* pRenderTarget, float color[4]) = 0;
        virtual void ClearDepthStencil(ITexture* pDepthStencil, float depth, uint8 stencil) = 0;
        
        virtual void SetRendertargets(ITexture* const* ppRenderTargets, uint32 numRenderTargets, ITexture* pDepthStencil) = 0;
        virtual void SetViewports(const SViewport* pViewports, uint32 numViewports) = 0;
        virtual void SetScissorRects(const SRectangle* pScissorRects, uint32 numScissorRects) = 0;
        virtual void SetVertexBuffers(IBuffer* const * pBuffers, uint32 numBuffers, uint32 startSlot) = 0;
        virtual void SetIndexBuffer(IBuffer* pBuffer, Format format) = 0;
        virtual void SetShaderVariableTable(IShaderVariableTable* pVariableTable) = 0;
        virtual void SetConstantBlocks(ShaderStage stage, uint32 offset, uint32 sizeInBytes, void* pData) = 0;
        virtual void SetPipelineState(IPipelineState* pPiplineState) = 0;

        virtual void UpdateBuffer(IBuffer* pResource, const ResourceData& data) = 0;
        virtual void UpdateTexture(ITexture* pResource, const ResourceData& data, uint32 mipLevel) = 0;

        virtual void CopyBuffer(IBuffer* pDst, IBuffer* pSrc) = 0;

        virtual void MapBuffer(IBuffer* pBuffer, uint32 mapFlags, void** ppData) = 0;
        virtual void UnmapBuffer(IBuffer* pBuffer) = 0;
        
        virtual void ResolveTexture(ITexture* pDst, uint32 dstMipLevel, ITexture* pSrc, uint32 srcMipLevel) = 0;
        
		virtual void GenerateMipLevels(ITexture* pTexture) = 0;

		virtual void TransitionTextureStates(const TextureTransitionBarrier* pBarriers, uint32 numBarriers) = 0;

        virtual void Draw(uint32 vertexCount, uint32 startVertex) = 0;
        virtual void DrawIndexed(uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation) = 0;
        virtual void DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation) = 0;
        virtual void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation) = 0;
       
        virtual void ExecuteDefferedContext(IDeviceContext* pContext) = 0;
        
		virtual void ResetQuery(IQuery* pQuery) = 0;
		virtual void WriteTimeStamp(IQuery* pQuery) = 0;

        virtual void Flush() = 0;
        virtual void ClearState() = 0;

        virtual DeviceContextType GetType() const = 0;
        virtual void* GetNativeHandle() const = 0;
	};
}
