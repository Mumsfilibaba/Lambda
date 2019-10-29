#pragma once
#include "DeviceObjectBase.h"
#include "IDeviceContext.h"
#include "ITexture.h"
#include <algorithm>

namespace Lambda
{
	//-----------------
	//DeviceContextBase
	//-----------------

	template <typename TDeviceImpl, typename TTextureImpl, typename TBufferImpl, typename TPipelineStateImpl, typename TShaderVariableTableImpl>
	class DeviceContextBase : public DeviceObjectBase<TDeviceImpl, IDeviceContext>
	{
	public:
		LAMBDA_NO_COPY(DeviceContextBase);

		DeviceContextBase(TDeviceImpl* pDevice, DeviceContextType type)
			: DeviceObjectBase<TDeviceImpl, IDeviceContext>(pDevice),
			m_Type(type)
		{
		}
		~DeviceContextBase() = default;

		virtual void ClearRenderTarget(ITexture* pRenderTarget, float color[4]) override = 0;
		virtual void ClearDepthStencil(ITexture* pDepthStencil, float depth, uint8 stencil) override = 0;

		virtual void SetConstantBlocks(ShaderStage stage, uint32 offset, uint32 sizeInBytes, void* pData) override = 0;

		virtual void UpdateBuffer(IBuffer* pResource, const ResourceData& data) override = 0;
		virtual void UpdateTexture(ITexture* pResource, const ResourceData& data, uint32 mipLevel) override = 0;

		virtual void CopyBuffer(IBuffer* pDst, IBuffer* pSrc) override = 0;

		virtual void MapBuffer(IBuffer* pBuffer, uint32 mapFlags, void** ppData) override = 0;
		virtual void UnmapBuffer(IBuffer* pBuffer) override = 0;

		virtual void ResolveTexture(ITexture* pDst, uint32 dstMipLevel, ITexture* pSrc, uint32 srcMipLevel) override = 0;

		virtual void GenerateMipLevels(ITexture* pTexture) = 0;

		virtual void Draw(uint32 vertexCount, uint32 startVertex) override = 0;
		virtual void DrawIndexed(uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation) override = 0;
		virtual void DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation) override = 0;
		virtual void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation) override = 0;

		virtual void ExecuteDefferedContext(IDeviceContext* pContext) override = 0;

		virtual void ResetQuery(IQuery* pQuery) override = 0;
		virtual void WriteTimeStamp(IQuery* pQuery, PipelineStage stage) override = 0;

		virtual void Flush() override = 0;
		virtual void* GetNativeHandle() const override = 0;

		virtual void SetRendertargets(ITexture* const* ppRenderTargets, uint32 numRenderTargets, ITexture* pDepthStencil) override
		{
			LAMBDA_ASSERT_PRINT(numRenderTargets <= LAMBDA_MAX_RENDERTARGET_COUNT, "Lambda Engine: 'numRendertargets' must be less that the maximum of '%d'\n", LAMBDA_MAX_RENDERTARGET_COUNT);

            //Set rendertarget state
            m_NumRenderTargets = numRenderTargets;
            for (uint32 i = 0; i < numRenderTargets; i++)
            {
                m_RenderTargets[i] = reinterpret_cast<TTextureImpl*>(ppRenderTargets[i]);
                m_RenderTargets[i]->AddRef();
            }
            
            
			//Get size of framebuffer
			if (pDepthStencil)
			{
                //Set depthstencil
                m_DepthStencil = reinterpret_cast<TTextureImpl*>(pDepthStencil);
                m_DepthStencil->AddRef();
                
                const TextureDesc& desc = m_DepthStencil->GetDesc();
                m_FrameBufferHeight         = desc.Height;
                m_FrameBufferWidth          = desc.Width;
                m_FrameBufferSampleCount    = desc.SampleCount;
			}
			else
			{
				const TextureDesc& desc = m_RenderTargets[0]->GetDesc();
				m_FrameBufferHeight			= desc.Height;
				m_FrameBufferWidth			= desc.Width;
				m_FrameBufferSampleCount	= desc.SampleCount;

				//Set depthstencil to nullptr
				m_DepthStencil = nullptr;
			}
		}


		virtual void SetViewports(const Viewport* pViewports, uint32 numViewports) override
		{
			LAMBDA_ASSERT(pViewports != nullptr);
			LAMBDA_ASSERT_PRINT(numViewports <= LAMBDA_MAX_VIEWPORT_COUNT, "Lambda Engine: 'numViewports' must be less that the maximum of '%d'\n", LAMBDA_MAX_VIEWPORT_COUNT);

			//Set viewports
			m_NumViewports = numViewports;
			for (uint32 i = 0; i < numViewports; i++)
				m_Viewports[i] = pViewports[i];
		}


		virtual void SetScissorRects(const Rectangle* pScissorRects, uint32 numRects) override
		{
			LAMBDA_ASSERT(pScissorRects != nullptr);
			LAMBDA_ASSERT_PRINT(numRects <= LAMBDA_MAX_SCISSOR_RECT_COUNT, "Lambda Engine: 'numRects' must be less that the maximum of '%d'\n", LAMBDA_MAX_SCISSOR_RECT_COUNT);

			//Set Scissor Rectangles
			m_NumScissorRects = numRects;
			for (uint32 i = 0; i < numRects; i++)
				m_ScissorRects[i] = pScissorRects[i];
		}


		virtual void SetVertexBuffers(IBuffer* const* ppBuffers, uint32 numBuffers, uint32 slot) override
		{
			LAMBDA_ASSERT_PRINT(numBuffers <= LAMBDA_MAX_VERTEXBUFFER_COUNT, "Lambda Engine: 'numBuffers' must be less that the maximum of '%d'\n", LAMBDA_MAX_VERTEXBUFFER_COUNT);
			LAMBDA_ASSERT_PRINT(slot < LAMBDA_MAX_VERTEXBUFFER_COUNT, "Lambda Engine: 'slot' cannot be more than last slot of '%d' slot\n", LAMBDA_MAX_VERTEXBUFFER_COUNT - 1);
			LAMBDA_ASSERT_PRINT((slot + numBuffers - 1) < LAMBDA_MAX_VERTEXBUFFER_COUNT, "Lambda Engine: Current call to IDeviceContext::SetVertexBuffers will write to a slot outside of the range [0, %d]\n", LAMBDA_MAX_VERTEXBUFFER_COUNT - 1);

			//Set vertexbuffers
			m_NumVertexBuffers = std::max(m_NumVertexBuffers, slot + numBuffers);
			for (uint32 i = 0; i < numBuffers; i++)
			{
				m_VertexBuffers[slot + i] = reinterpret_cast<TBufferImpl*>(ppBuffers[i]);
				if (m_VertexBuffers[slot + i])
					m_VertexBuffers[slot + i]->AddRef();
			}
		}


		virtual void SetIndexBuffer(IBuffer* pBuffer, Format format) override
		{
			LAMBDA_ASSERT_PRINT(format == FORMAT_R32_UINT || format == FORMAT_R16_UINT, "Lambda Engine: Only supported formats for IDeviceContext::SetIndexBuffer is FORMAT_R32_UINT or FORMAT_R16_UINT\n");

			//Set indexbuffer
			if (pBuffer)
			{
				m_IndexBuffer = reinterpret_cast<TBufferImpl*>(pBuffer);
				m_IndexBuffer->AddRef();

				m_IndexBufferFormat = format;
			}
			else
			{
				m_IndexBuffer = nullptr;
			}
		}


		virtual void SetPipelineState(IPipelineState* pPipelineState) override
		{
			//Set pipelinestate
			if (pPipelineState)
			{
				m_PipelineState = reinterpret_cast<TPipelineStateImpl*>(pPipelineState);
				m_PipelineState->AddRef();
			}
			else
			{
				m_PipelineState = nullptr;
			}
		}


		virtual void SetShaderVariableTable(IShaderVariableTable* pShaderVariableTable) override
		{
			//Set ShaderVariableTable
			if (pShaderVariableTable)
			{
				m_ShaderVariableTable = reinterpret_cast<TShaderVariableTableImpl*>(pShaderVariableTable);
				m_ShaderVariableTable->AddRef();
			}
			else
			{
				m_ShaderVariableTable = nullptr;
			}
		}


		virtual void ClearState() override
		{
			//Before clearing state we want to flush the context
			this->Flush();

			//Rendertargets
			for (uint32 i = 0; i < m_NumRenderTargets; i++)
				m_RenderTargets[i] = nullptr;
			m_DepthStencil = nullptr;
			m_NumRenderTargets			= 0;
			m_FrameBufferHeight			= 0;
			m_FrameBufferWidth			= 0;
			m_FrameBufferSampleCount	= 0;

			//Viewport
			for (uint32 i = 0; i < m_NumViewports; i++)
				m_Viewports[i] = Viewport();
			m_NumViewports = 0;

			//Scissor rectangles
			for (uint32 i = 0; i < m_NumScissorRects; i++)
				m_ScissorRects[i] = Rectangle();
			m_NumScissorRects = 0;

			//VertexBuffers
			for (uint32 i = 0; i < m_NumVertexBuffers; i++)
				m_VertexBuffers[i] = nullptr;
			m_NumVertexBuffers = 0;

			//IndexBuffer
			m_IndexBuffer		= nullptr;
			m_IndexBufferFormat = FORMAT_UNKNOWN;

			//PipelineState
			m_PipelineState = nullptr;

			//ShaderVariableTable
			m_ShaderVariableTable = nullptr;
		}


		virtual DeviceContextType GetType() const override
		{
			return m_Type;
		}
	protected:
		//Rendertargets
		AutoRef<TTextureImpl> m_RenderTargets[LAMBDA_MAX_RENDERTARGET_COUNT];
		AutoRef<TTextureImpl> m_DepthStencil;
		uint32 m_NumRenderTargets		= 0;
		uint32 m_FrameBufferHeight		= 0;
		uint32 m_FrameBufferWidth		= 0;
		uint32 m_FrameBufferSampleCount = 0;
		
		//Viewport
		Viewport m_Viewports[LAMBDA_MAX_VIEWPORT_COUNT];
		uint32 m_NumViewports = 0;
		
		//Scissor rectangles
		Rectangle m_ScissorRects[LAMBDA_MAX_SCISSOR_RECT_COUNT];
		uint32 m_NumScissorRects = 0;

		//VertexBuffers
		AutoRef<TBufferImpl> m_VertexBuffers[LAMBDA_MAX_VERTEXBUFFER_COUNT];
		uint32 m_NumVertexBuffers = 0;

		//IndexBuffer
		AutoRef<TBufferImpl> m_IndexBuffer;
		Format m_IndexBufferFormat;

		//PipelineState
		AutoRef<TPipelineStateImpl> m_PipelineState;

		//ShaderVariableTable
		AutoRef<TShaderVariableTableImpl> m_ShaderVariableTable;

		//Type
		DeviceContextType m_Type;
	};
}
