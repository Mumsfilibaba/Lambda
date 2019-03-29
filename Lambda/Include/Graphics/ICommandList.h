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
		RESOURCE_STATE_PRESENT_COMMON = 2,
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


	class LAMBDA_API ICommandList : public IObject
	{
	public:
		LAMBDA_INTERFACE(ICommandList);

		ICommandList() = default;
		~ICommandList() = default;

		virtual void ClearRenderTargetView(IRenderTarget* pRenderTarget, float color[4]) = 0;

		virtual void SetRenderTarget(IRenderTarget* pRenderTarget) = 0;
		virtual void SetViewport(const Viewport& viewport) = 0;
		virtual void SetScissorRect(const Math::Rectangle& scissorRect) = 0;
		virtual void SetPrimtiveTopology(PrimitiveTopology topology) = 0;
		virtual void SetGraphicsPipelineState(IGraphicsPipelineState* pPSO) = 0;

		virtual void TransitionResource(IRenderTarget* pRenderTarget, ResourceState resourceState) = 0;

		virtual void DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation) = 0;

		virtual void Close() = 0;
		virtual void Reset() = 0;

	public:
		static ICommandList* Create(CommandListType type);
	};
}