#pragma once
#include "ICommandList.h"
#include "IRenderPass.h"
#include "Time/Clock.h"
#include <vector>
#include <glm/glm.hpp>

namespace Lambda
{
	class Camera;
	class IQuery;
    class IBuffer;
	class ITexture;
	class ISamplerState;
    class IGraphicsPipelineState;

	//---------------
	//FrameStatistics
	//---------------

	struct FrameStatistics
	{
		uint32 FPS = 0;
		Timestep CPUTime;
		Timestep GPUTime;
	};

	//------------
	//CameraBuffer
	//------------

	struct CameraBuffer
	{
		glm::mat4 View;
		glm::mat4 Projection;
		glm::vec3 Position;
	};

	//-----------
	//LightBuffer
	//-----------

	struct LightBuffer
	{
		glm::vec4 Color;
		glm::vec3 Position;
	};

	//-----
	//Model
	//-----

	struct Model
	{
		IBuffer* pVertexBuffer	= nullptr;
		IBuffer* pIndexBuffer	= nullptr;
		uint32 IndexCount		= 0;
	};

	//--------
	//Material
	//--------

	struct Material
	{
		ITexture* pAlbedoMap					= nullptr;
		ITexture* pNormalMap					= nullptr;
		ISamplerState* pSamplerState			= nullptr;
		IPipelineResourceState* pResourceState	= nullptr;
		IGraphicsPipelineState* pPipelineState	= nullptr;
		glm::vec4 Color;
	};

	//---------------
	//TransformBuffer
	//---------------

	struct TransformBuffer
	{
		glm::mat4 Model;
	};

	//---------------
	//TransformBuffer
	//---------------

	struct MaterialBuffer
	{
		glm::vec4 Color;
		int32 HasAlbedoMap = 0;
		int32 HasNormalMap = 0;
	};

    //----------
    //Renderer3D
    //----------

    class LAMBDA_API Renderer3D
	{
		friend class Application;

	public:
		LAMBDA_NO_COPY(Renderer3D);

		Renderer3D();
		~Renderer3D() = default;

		void Init();
		void Begin();
		void BeginScene(const Camera& camera, const LightBuffer& light) const;
		void Submit(const Model& model, const Material& material, const TransformBuffer& transform) const;
		void EndScene() const;
		void End();
		void Swapbuffers();
		void Release();
		void SetDisplaySize(uint32 width, uint32 height);
		IBuffer* GetCameraCB() const;
		IBuffer* GetLightCB() const;
		IBuffer* GetTransformCB() const;
		IRenderPass* GetRenderPass() const;
		const FrameStatistics& GetFrameStatistics() const;
	private:
		ICommandList*				m_pCurrentList;
		std::vector<ICommandList*>	m_Lists;
		std::vector<IQuery*>		m_Queries;
		IQuery*						m_pCurrentQuery;
		IRenderPass*				m_pRenderPass;
		IBuffer*					m_pCameraBuffer;
		IBuffer*					m_pLightBuffer;
		IBuffer*					m_pTransformBuffer;
		IBuffer*					m_pMaterialBuffer;
		Viewport					m_Viewport;
		Rectangle					m_ScissorRect;
		FrameStatistics				m_FrameInfo;
		Clock						m_FrameClock;
		uint32						m_CurrentFPS;
	};
}
