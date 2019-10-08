#pragma once
#include "Core/IDevice.h"
#include "Core/IDeviceContext.h"
#include "Core/ISwapChain.h"
#include "Core/IBuffer.h"
#include "Core/IQuery.h"
#include "Time/Clock.h"
#include <vector>
#include <glm/glm.hpp>

namespace Lambda
{
	class Camera;

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
		IShaderVariableTable* pVariableTable = nullptr;
		IPipelineState* pPipelineState	= nullptr;
		uint32 HasNormalMap = 0;
		uint32 HasAlbedoMap = 0;
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
		void BeginScene(const Camera& camera, const LightBuffer& light);
		void Submit(const Model& model, const Material& material, const TransformBuffer& transform);
		void EndScene() const;
		void End();
		void Swapbuffers();
		void Release();
		void SetDisplaySize(uint32 width, uint32 height);
		IBuffer* GetCameraCB();
		IBuffer* GetLightCB();
		IBuffer* GetTransformCB();
		const FrameStatistics& GetFrameStatistics() const;
	private:
        AutoRef<IDevice>            m_Device;
        AutoRef<ISwapChain>         m_SwapChain;
        AutoRef<IDeviceContext>     m_Context;
		IQuery*						m_pCurrentQuery;
		std::vector<AutoRef<IQuery>> m_Queries;
		AutoRef<IBuffer>			m_CameraBuffer;
		AutoRef<IBuffer>			m_LightBuffer;
		AutoRef<IBuffer>			m_TransformBuffer;
		AutoRef<IBuffer>			m_MaterialBuffer;
		Viewport					m_Viewport;
		Rectangle					m_ScissorRect;
		FrameStatistics				m_FrameInfo;
		Clock						m_FrameClock;
		uint32						m_CurrentFPS;
        uint32                      m_QueryIndex;
	};
}
