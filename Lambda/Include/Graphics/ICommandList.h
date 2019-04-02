#pragma once
#include <IObject.h>
#include <Math/Rectangle.h>

namespace Lambda
{
	enum CommandListType : uint32
	{
		COMMAND_LIST_TYPE_UNKNOWN = 0,
		COMMAND_LIST_TYPE_GRAPHICS = 1,
		COMMAND_LIST_TYPE_COMPUTE = 2,
	};

	enum ResourceState
	{
		RESOURCE_STATE_UNKNOWN = 0,
		RESOURCE_STATE_RENDERTARGET = 1,
		RESOURCE_STATE_DEPTH_WRITE = 2,
		RESOURCE_STATE_PRESENT_COMMON = 3,
		RESOURCE_STATE_COPY_DEST = 4,
		RESOURCE_STATE_COPY_SRC = 5,
	};


	struct Viewport
	{
		float TopX;
		float TopY;
		float Width;
		float Height;
		float MinDepth;
		float MaxDepth;
	};


	class IBuffer;
	class ITexture2D;
	class IRenderTarget;
	class IGraphicsPipelineState;

	class LAMBDA_API ICommandList : public IObject
	{
	public:
		LAMBDA_INTERFACE(ICommandList);

		ICommandList() = default;
		~ICommandList() = default;

		virtual void ClearRenderTarget(ITexture2D* pRenderTarget, float color[4]) = 0;
		virtual void ClearDepthStencil(ITexture2D* pDepthStencil, float depth, uint8 stencil) = 0;

		virtual void SetRenderTarget(ITexture2D* pRenderTarget, ITexture2D* pDepthStencil) = 0;
		virtual void SetViewport(const Viewport& viewport) = 0;
		virtual void SetScissorRect(const Math::Rectangle& scissorRect) = 0;
		virtual void SetPrimtiveTopology(PrimitiveTopology topology) = 0;
		virtual void SetGraphicsPipelineState(IGraphicsPipelineState* pPSO) = 0;
		virtual void SetVertexBuffer(IBuffer* pBuffer, uint32 slot) = 0;

		virtual void VSSetConstantBuffers(const IBuffer* const * ppBuffers, uint32 numBuffers, uint32 startSlot) = 0;
		virtual void HSSetConstantBuffers(const IBuffer* const * ppBuffers, uint32 numBuffers, uint32 startSlot) = 0;
		virtual void DSSetConstantBuffers(const IBuffer* const * ppBuffers, uint32 numBuffers, uint32 startSlot) = 0;
		virtual void GSSetConstantBuffers(const IBuffer* const * ppBuffers, uint32 numBuffers, uint32 startSlot) = 0;
		virtual void PSSetConstantBuffers(const IBuffer* const * ppBuffers, uint32 numBuffers, uint32 startSlot) = 0;

		virtual void UpdateBuffer(IBuffer* pResource, const ResourceData* pData) = 0;

		virtual void CopyBuffer(IBuffer* pDst, IBuffer* pSrc) = 0;

		virtual void TransitionResource(IBuffer* pResource, ResourceState resourceState) = 0;
		virtual void TransitionResource(ITexture2D* pResource, ResourceState resourceState) = 0;

		virtual void DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation) = 0;

		virtual void Close() = 0;
		virtual void Reset() = 0;

	public:
		static ICommandList* Create(CommandListType type);
	};
}