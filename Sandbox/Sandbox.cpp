#include "SandBox.h"
#include "System/Log.h"
#include "System/Input.h"
#include "Events/WindowEvent.h"
#include "Events/KeyEvent.h"
#include "Graphics/MeshFactory.h"
#include <glm/gtc/matrix_transform.hpp>

#define SINGLE_CUBE
#define RGB_F(r, g, b) float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f

namespace Lambda
{   
	SandBox::SandBox(const EngineParams& params)
		: Application(params),
        EventLayer("SandBoxLayer"),
		m_pLists(),
        m_pCurrentList(nullptr),
		m_pVS(nullptr),
		m_pPS(nullptr),
		m_pVertexBuffer(nullptr),
        m_pIndexBuffer(nullptr),
        m_pPipelineState(nullptr)
	{
		for (uint32 i = 0; i < 3; i++)
			m_pLists[i] = nullptr;
	}
	

	SandBox::~SandBox()
	{
	}


	void SandBox::OnLoad()
	{
		//Add Sandbox-level eventlayer
		EventDispatcher::PushEventLayer(this);

		//Init size
		m_Width		= (float)GetWindow()->GetWidth();
		m_Height	= (float)GetWindow()->GetHeight();

		IGraphicsDevice* pDevice = IGraphicsDevice::Get();
        if (pDevice)
        {
            //Create commandlists
            {
                for (uint32 i = 0; i < 3; i++)
                {
                    pDevice->CreateCommandList(&m_pLists[i], COMMAND_LIST_TYPE_GRAPHICS);
                    if (m_pLists[i])
                    {
                        std::string name = "CommandList [" + std::to_string(i) + "]";
                        m_pLists[i]->SetName(name.c_str());
                    }
                }

                m_pCurrentList = m_pLists[0];
                if (m_pCurrentList)
                {
                    m_pCurrentList->Reset();
                }
            }

            //Create queries
            {
                QueryDesc desc = {};
                desc.Type       = QUERY_TYPE_TIMESTAMP;
                desc.QueryCount = 2;
                for (uint32 i = 0; i < 3; i++)
                {
                    pDevice->CreateQuery(&m_pQueries[i], desc);
                    m_pCurrentList->ResetQuery(m_pQueries[i]);
                }
            }
            
            //Create shaders
			GraphicsDeviceDesc deviceDesc = pDevice->GetDesc();
			if (deviceDesc.Api == GRAPHICS_API_VULKAN)
			{
				m_pVS = IShader::CreateShaderFromFile(pDevice, "vert.spv", "main", SHADER_STAGE_VERTEX, SHADER_LANG_SPIRV);
				m_pPS = IShader::CreateShaderFromFile(pDevice, "frag.spv", "main", SHADER_STAGE_PIXEL,	SHADER_LANG_SPIRV);
			}
			else if (deviceDesc.Api == GRAPHICS_API_D3D12)
			{
				m_pVS = IShader::CreateShaderFromFile(pDevice, "Triangle.hlsl", "VSMain", SHADER_STAGE_VERTEX,	SHADER_LANG_HLSL);
				m_pPS = IShader::CreateShaderFromFile(pDevice, "Triangle.hlsl", "PSMain", SHADER_STAGE_PIXEL,	SHADER_LANG_HLSL);
			}
            

            //Define depthformat
            ResourceFormat depthFormat = FORMAT_D24_UNORM_S8_UINT;
            

			//Create RenderPass
			{
				RenderPassDesc desc = {};
				desc.SampleCount						= GetEngineParams().SampleCount;
				desc.NumRenderTargets					= 1;
				desc.RenderTargets[0].Format			= pDevice->GetBackBufferFormat();
                desc.RenderTargets[0].Flags				= RENDER_PASS_ATTACHMENT_FLAG_RESOLVE;
                desc.RenderTargets[0].LoadOperation		= LOAD_OP_CLEAR;
                desc.RenderTargets[0].StoreOperation	= STORE_OP_STORE;
                desc.RenderTargets[0].FinalState		= RESOURCE_STATE_RENDERTARGET_PRESENT;
				desc.DepthStencil.Format				= depthFormat;
                desc.DepthStencil.Flags					= 0;
                desc.DepthStencil.LoadOperation			= LOAD_OP_CLEAR;
                desc.DepthStencil.StoreOperation		= STORE_OP_UNKNOWN;
                desc.DepthStencil.FinalState			= RESOURCE_STATE_DEPTH_STENCIL;
				pDevice->CreateRenderPass(&m_pRenderPass, desc);
			}


			//Create ResourceState
			{
				ResourceSlot slots[7];
				slots[0].Slot	= 0;
				slots[0].Stage	= SHADER_STAGE_VERTEX;
				slots[0].Type	= RESOURCE_TYPE_CONSTANT_BUFFER;
				slots[0].Usage	= RESOURCE_USAGE_DEFAULT;

				slots[1].Slot	= 1;
				slots[1].Stage	= SHADER_STAGE_VERTEX;
				slots[1].Type	= RESOURCE_TYPE_CONSTANT_BUFFER;
				slots[1].Usage	= RESOURCE_USAGE_DYNAMIC;

				slots[2].Slot	= 2;
				slots[2].Stage	= SHADER_STAGE_PIXEL;
				slots[2].Type	= RESOURCE_TYPE_CONSTANT_BUFFER;
				slots[2].Usage	= RESOURCE_USAGE_DYNAMIC;
                
                slots[3].Slot   = 3;
                slots[3].Stage  = SHADER_STAGE_PIXEL;
                slots[3].Type   = RESOURCE_TYPE_CONSTANT_BUFFER;
                slots[3].Usage  = RESOURCE_USAGE_DEFAULT;

				slots[4].Slot	= 4;
				slots[4].Stage	= SHADER_STAGE_PIXEL;
				slots[4].Type	= RESOURCE_TYPE_TEXTURE;
				slots[4].Usage	= RESOURCE_USAGE_DEFAULT;
                
                slots[5].Slot   = 5;
                slots[5].Stage  = SHADER_STAGE_PIXEL;
                slots[5].Type   = RESOURCE_TYPE_TEXTURE;
                slots[5].Usage  = RESOURCE_USAGE_DEFAULT;

				slots[6].Slot	= 6;
				slots[6].Stage	= SHADER_STAGE_PIXEL;
				slots[6].Type	= RESOURCE_TYPE_SAMPLER_STATE;
				slots[6].Usage	= RESOURCE_USAGE_DEFAULT;

				PipelineResourceStateDesc desc = {};
				desc.NumResourceSlots	= 7;
				desc.pResourceSlots		= slots;
				pDevice->CreatePipelineResourceState(&m_pResourceState, desc);
			}


            //Create pipelinestate
            {
                InputElement elements[]
                {
                    { "POSITION",   FORMAT_R32G32B32_FLOAT, 0, 0, sizeof(Vertex), 0,                     false },
                    { "NORMAL",     FORMAT_R32G32B32_FLOAT, 0, 1, sizeof(Vertex), sizeof(glm::vec3),     false },
                    { "TANGENT",    FORMAT_R32G32B32_FLOAT, 0, 2, sizeof(Vertex), sizeof(glm::vec3) * 2, false },
                    { "TEXCOORD",   FORMAT_R32G32_FLOAT,    0, 3, sizeof(Vertex), sizeof(glm::vec3) * 3, false },
                };
                
                GraphicsPipelineStateDesc desc = {};
                desc.pVertexShader		= m_pVS;
                desc.pPixelShader		= m_pPS;
                desc.pInputElements		= elements;
                desc.InputElementCount	= sizeof(elements) / sizeof(InputElement);
                desc.Cull				= CULL_MODE_BACK;
				desc.Mode				= POLYGON_MODE_FILL;
                desc.Topology			= PRIMITIVE_TOPOLOGY_TRIANGLELIST;
				desc.pRenderPass		= m_pRenderPass;
				desc.pResourceState		= m_pResourceState;
                desc.DepthTest			= true;
                
                pDevice->CreateGraphicsPipelineState(&m_pPipelineState, desc);
            }

            //Create vertexbuffer
            MeshData mesh = MeshFactory::CreateFromFile("revolver.obj");
			m_IndexCount = uint32(mesh.Indices.size());
			{
                BufferDesc desc     = {};
                desc.pName          = "VertexBuffer";
                desc.Usage          = RESOURCE_USAGE_DEFAULT;
                desc.Flags          = BUFFER_FLAGS_VERTEX_BUFFER;
                desc.SizeInBytes    = sizeof(Vertex) * uint32(mesh.Vertices.size());
                desc.StrideInBytes  = sizeof(Vertex);

                ResourceData data   = {};
                data.pData          = mesh.Vertices.data();
                data.SizeInBytes    = desc.SizeInBytes;

                pDevice->CreateBuffer(&m_pVertexBuffer, &data, desc);
            }
            
            //Create indexbuffer
            {
                BufferDesc desc     = {};
                desc.pName          = "IndexBuffer";
                desc.Usage          = RESOURCE_USAGE_DEFAULT;
                desc.Flags          = BUFFER_FLAGS_INDEX_BUFFER;
                desc.SizeInBytes    = sizeof(uint32) * uint32(mesh.Indices.size());
                desc.StrideInBytes  = sizeof(uint32);
                
                ResourceData data   = {};
                data.pData          = mesh.Indices.data();
                data.SizeInBytes    = desc.SizeInBytes;
                
                pDevice->CreateBuffer(&m_pIndexBuffer, &data, desc);
            }

            //Create colorbuffer
            {
                glm::vec4 color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

                BufferDesc desc     = {};
                desc.pName          = "ColorBuffer";
                desc.Usage			= RESOURCE_USAGE_DYNAMIC;
                desc.Flags			= BUFFER_FLAGS_CONSTANT_BUFFER;
                desc.SizeInBytes	= sizeof(glm::vec4);
                desc.StrideInBytes	= sizeof(glm::vec4);

                ResourceData data   = {};
                data.pData			= &color;
                data.SizeInBytes	= sizeof(color);

                pDevice->CreateBuffer(&m_pColorBuffer, &data, desc);
            }
            
            //Create lightbuffer
            {
                LightBuffer light = {};
                
                BufferDesc desc = {};
                desc.pName            = "LightBuffer";
                desc.Usage            = RESOURCE_USAGE_DEFAULT;
                desc.Flags            = BUFFER_FLAGS_CONSTANT_BUFFER;
                desc.SizeInBytes      = sizeof(LightBuffer);
                desc.StrideInBytes    = sizeof(LightBuffer);
                
                ResourceData data   = {};
                data.pData          = &light;
                data.SizeInBytes    = sizeof(LightBuffer);
                
                pDevice->CreateBuffer(&m_pLightBuffer, &data, desc);
            }

            //Create camerabuffer
            {
                //Set camera
                m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, -2.0f));
                m_Camera.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
                m_Camera.CreateView();
                
                BufferDesc desc = {};
                desc.pName          = "CameraBuffer";
                desc.Usage			= RESOURCE_USAGE_DEFAULT;
                desc.Flags			= BUFFER_FLAGS_CONSTANT_BUFFER;
                desc.SizeInBytes	= sizeof(CameraBuffer);
                desc.StrideInBytes	= sizeof(CameraBuffer);

                ResourceData data = {};
                data.pData			= &m_Camera;
                data.SizeInBytes	= desc.SizeInBytes;

                pDevice->CreateBuffer(&m_pCameraBuffer, &data, desc);
            }

            //Init transforms
            m_TransformBuffer.Model = glm::mat4(1.0f);
            
            //Create TransformBuffer
            {
                BufferDesc desc = {};
                desc.pName          = "TransformBuffer";
                desc.Usage			= RESOURCE_USAGE_DYNAMIC;
                desc.Flags			= BUFFER_FLAGS_CONSTANT_BUFFER;
                desc.SizeInBytes	= sizeof(TransformBuffer);
                desc.StrideInBytes	= sizeof(TransformBuffer);
                
                ResourceData data = {};
                data.pData			= &m_TransformBuffer;
                data.SizeInBytes	= desc.SizeInBytes;
                
                pDevice->CreateBuffer(&m_pTransformBuffer, &data, desc);
            }

            //Create texture
            m_pAlbedo = ITexture::CreateTextureFromFile(pDevice, "revolver_albedo.png", TEXTURE_FLAGS_SHADER_RESOURCE | TEXTURE_FLAGS_GENEATE_MIPS, RESOURCE_USAGE_DEFAULT, FORMAT_R8G8B8A8_UNORM);
            m_pNormal = ITexture::CreateTextureFromFile(pDevice, "revolver_normal.png", TEXTURE_FLAGS_SHADER_RESOURCE | TEXTURE_FLAGS_GENEATE_MIPS, RESOURCE_USAGE_DEFAULT, FORMAT_R8G8B8A8_UNORM);
			m_pCurrentList->TransitionTexture(m_pAlbedo, RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0, LAMBDA_TRANSITION_ALL_MIPS);
            m_pCurrentList->TransitionTexture(m_pNormal, RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0, LAMBDA_TRANSITION_ALL_MIPS);

            //Create samplerstate
            {
                TextureDesc textureDesc = m_pAlbedo->GetDesc();
                
                SamplerStateDesc desc = {};
                desc.AdressMode = SAMPLER_ADDRESS_MODE_REPEAT;
				desc.MinMipLOD	= 0.0f;
				desc.MaxMipLOD	= float(textureDesc.MipLevels);
				desc.MipLODBias	= 0.0f;
                
                pDevice->CreateSamplerState(&m_pSamplerState, desc);
            }

            //Close and execute commandlist
            m_pCurrentList->Close();
            pDevice->ExecuteCommandList(&m_pCurrentList, 1);

            //Wait for GPU
            pDevice->WaitForGPU();
        }
	}


	void SandBox::OnUpdate(Timestep dt)
	{
        //Move camera
        constexpr float speed = 2.0f;
        if (Input::IsKeyDown(KEY_W))
            m_Camera.Translate(glm::vec3(0.0f, 0.0f, speed) * dt.AsSeconds());
        else if (Input::IsKeyDown(KEY_S))
            m_Camera.Translate(glm::vec3(0.0f, 0.0f, -speed) * dt.AsSeconds());
        
        if (Input::IsKeyDown(KEY_A))
            m_Camera.Translate(glm::vec3(speed, 0.0f, 0.0f) * dt.AsSeconds());
        else if (Input::IsKeyDown(KEY_D))
            m_Camera.Translate(glm::vec3(-speed, 0.0f, 0.0f) * dt.AsSeconds());
        
        if (Input::IsKeyDown(KEY_SPACE))
            m_Camera.Translate(glm::vec3(0.0f, speed, 0.0f) * dt.AsSeconds());
        else if (Input::IsKeyDown(KEY_LEFT_SHIFT))
            m_Camera.Translate(glm::vec3(0.0f, -speed, 0.0f) * dt.AsSeconds());
        
        //Rotate camera
         constexpr float rotation = 30.0f;
        if (Input::IsKeyDown(KEY_UP))
            m_Camera.Rotate(glm::vec3(-rotation, 0.0f, 0.0f) * dt.AsSeconds());
        else if (Input::IsKeyDown(KEY_DOWN))
            m_Camera.Rotate(glm::vec3(rotation, 0.0f, 0.0f) * dt.AsSeconds());
        
        if (Input::IsKeyDown(KEY_RIGHT))
            m_Camera.Rotate(glm::vec3(0.0f, -rotation, 0.0f) * dt.AsSeconds());
        else if (Input::IsKeyDown(KEY_LEFT))
            m_Camera.Rotate(glm::vec3(0.0f, rotation, 0.0f) * dt.AsSeconds());
        
        if (Input::IsKeyDown(KEY_Q))
            m_Camera.Rotate(glm::vec3(0.0f, 0.0f, rotation) * dt.AsSeconds());
        else if (Input::IsKeyDown(KEY_E))
            m_Camera.Rotate(glm::vec3(0.0f, 0.0f, -rotation) * dt.AsSeconds());
        
        m_Camera.CreateView();
	}


	void SandBox::OnRender(Timestep dt)
	{
        static Clock clock;
        
        //Get current device
        IGraphicsDevice* pDevice = IGraphicsDevice::Get();
        
        //Set commandlist for frame
        uint32 backBufferIndex = pDevice->GetBackBufferIndex();
        m_pCurrentList = m_pLists[backBufferIndex];
        m_pCurrentList->Reset();
        
        //Get values and reset query
        uint64 values[2] = { 0, 0 };
        m_pQueries[backBufferIndex]->GetResults(values, 2, 0);
        m_pCurrentList->ResetQuery(m_pQueries[backBufferIndex]);
        
        clock.Tick();
        if (clock.GetTotalTime().AsSeconds() > 1.0f)
        {
            uint64 ns   = values[1] - values[0];
            float ms    = float(ns) / (1000.0f * 1000.0f);
            
            LOG_SYSTEM(LOG_SEVERITY_INFO, "Renderpass time: %.2fms\n", ms);
            clock.Reset();
        }
        
        //Set and clear rendertarget
        float color[] = { 0.392f, 0.584f, 0.929f, 1.0f };
        ITexture* pRenderTarget = pDevice->GetRenderTarget();
        ITexture* pDepthBuffer	= pDevice->GetDepthStencil();
                
        //Set scissor and viewport
        Rectangle scissorrect;
        scissorrect.X       = 0.0f;
        scissorrect.Y       = 0.0f;
        scissorrect.Width   = float(pDevice->GetSwapChainWidth());
        scissorrect.Height  = float(pDevice->GetSwapChainHeight());
        
        Viewport viewport = {};
        viewport.Width      = scissorrect.Width;
        viewport.Height     = scissorrect.Height;
        viewport.TopX       = 0.0f;
        viewport.TopY       = 0.0f;
        viewport.MinDepth   = 0.0f;
        viewport.MaxDepth   = 1.0f;
        
        m_pCurrentList->SetViewport(viewport);
        m_pCurrentList->SetScissorRect(scissorrect);
        
        //Set pipelinestate and topology
        m_pCurrentList->SetGraphicsPipelineState(m_pPipelineState);
        
        //Update Colorbuffer
        ResourceData data   = {};
        glm::vec4 colorBuff = glm::vec4(RGB_F(255, 255, 255), 1.0f);
        data.pData          = &colorBuff;
        data.SizeInBytes    = sizeof(glm::vec4);
        m_pCurrentList->UpdateBuffer(m_pColorBuffer, &data);
        
        //Update camera buffer
        m_CameraBuffer.View         = m_Camera.GetView();
        m_CameraBuffer.Projection   = m_Camera.GetProjection();
        m_CameraBuffer.Position     = m_Camera.GetPosition();
        data.pData			= &m_CameraBuffer;
        data.SizeInBytes	= sizeof(CameraBuffer);
        m_pCurrentList->UpdateBuffer(m_pCameraBuffer, &data);
        
        //Update lightbuffer
        static LightBuffer lightBuffer  =
        {
            glm::vec4(RGB_F(255, 241, 224), 1.0f),
            glm::vec3(-1.5f, 0.5f, -1.0f)
        };
        
        data.pData          = &lightBuffer;
        data.SizeInBytes    = sizeof(LightBuffer);
        m_pCurrentList->UpdateBuffer(m_pLightBuffer, &data);
        
        //Set resources
		IBuffer*    buffers[]	= { m_pCameraBuffer, m_pTransformBuffer, m_pColorBuffer, m_pLightBuffer };
        ITexture*   textures[]	= { m_pAlbedo, m_pNormal };
		m_pResourceState->SetConstantBuffers(buffers, 4, 0);
		m_pResourceState->SetTextures(textures, 2, 4);
		m_pResourceState->SetSamplerStates(&m_pSamplerState, 1, 6);
		m_pCurrentList->SetGraphicsPipelineResourceState(m_pResourceState);
        
        //Set vertex- and indexbuffer
        m_pCurrentList->SetVertexBuffer(m_pVertexBuffer, 0);
        m_pCurrentList->SetIndexBuffer(m_pIndexBuffer);
        
		//Set rendertargets and clearcolors
		m_pRenderPass->SetRenderTargets(&pRenderTarget, 1, pDepthBuffer);
		m_pRenderPass->SetClearValues(color, 1.0f, 0);

        //Setup rotation
		static glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rotation = glm::rotate(rotation, glm::radians(30.0f) * dt.AsSeconds(), glm::vec3(0.0f, 1.0f, 0.0f));
        
#if !defined(SINGLE_CUBE)
        //Begin renderpass
        m_pCurrentList->BeginRenderPass(m_pRenderPass);

		constexpr uint32 cubes = 2;
        for (uint32 y = 0; y < cubes; y++)
        {
            for (uint32 x = 0; x < cubes; x++)
            {
                //Update transforms
				m_TransformBuffer.Model = glm::translate(glm::mat4(1.0f), glm::vec3(-float(cubes) + x * 2.0f, 0.0f, -float(cubes) + y * 2.0f)) * rotation;
                data.pData				= &m_TransformBuffer;
                data.SizeInBytes		= sizeof(TransformBuffer);
                m_pCurrentList->UpdateBuffer(m_pTransformBuffer, &data);
            
				//Draw
				m_pCurrentList->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
            }
        }
        
        //End renderpass
        m_pCurrentList->EndRenderPass();
#else

        //Update transforms
        m_TransformBuffer.Model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)) * rotation;
        data.pData            = &m_TransformBuffer;
        data.SizeInBytes    = sizeof(TransformBuffer);
        m_pCurrentList->UpdateBuffer(m_pTransformBuffer, &data);

		//Begin renderpass
        m_pCurrentList->WriteTimeStamp(m_pQueries[backBufferIndex], PIPELINE_STAGE_VERTEX);
		m_pCurrentList->BeginRenderPass(m_pRenderPass);
        
		//Draw first
		m_pCurrentList->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);

		//End renderpass
		m_pCurrentList->EndRenderPass();
        m_pCurrentList->WriteTimeStamp(m_pQueries[backBufferIndex], PIPELINE_STAGE_PIXEL);
#endif
     
        //Transition rendertarget to present
        m_pCurrentList->Close();
        
        //Present
        pDevice->ExecuteCommandListAndPresent(&m_pCurrentList, 1);
	}


	void SandBox::OnRelease()
	{
		IGraphicsDevice* pDevice = IGraphicsDevice::Get();
        if (pDevice)
        {
            pDevice->WaitForGPU();

            for (uint32 i = 0; i < 3; i++)
            {
                pDevice->DestroyCommandList(&m_pLists[i]);
                pDevice->DestroyQuery(&m_pQueries[i]);
            }

            pDevice->DestroyShader(&m_pVS);
            pDevice->DestroyShader(&m_pPS);

			pDevice->DestroyResourceState(&m_pResourceState);
			pDevice->DestroyRenderPass(&m_pRenderPass);
            pDevice->DestroyGraphicsPipelineState(&m_pPipelineState);
            pDevice->DestroyBuffer(&m_pVertexBuffer);
            pDevice->DestroyBuffer(&m_pIndexBuffer);
            pDevice->DestroyBuffer(&m_pColorBuffer);
            pDevice->DestroyBuffer(&m_pLightBuffer);
            pDevice->DestroyBuffer(&m_pCameraBuffer);
            pDevice->DestroyBuffer(&m_pTransformBuffer);
            pDevice->DestroyTexture(&m_pAlbedo);
            pDevice->DestroyTexture(&m_pNormal);
            pDevice->DestroySamplerState(&m_pSamplerState);
        }
	}


	void SandBox::CreateCamera(uint32 width, uint32 height)
    {
        m_Camera.SetAspect(float(width), float(height));
        m_Camera.CreateProjection();
	}


    void SandBox::OnPush()
    {
    }


    void SandBox::OnPop()
    {
    }


    uint32 SandBox::GetRecivableCategories() const
    {
        return EVENT_CATEGORY_WINDOW | EVENT_CATEGORY_KEYBOARD | EVENT_CATEGORY_MOUSE;
    }
    
    
    //Event handler function for the instance
    bool SandBox::OnEvent(const Event* pEvent)
    {
        switch (pEvent->GetType())
        {
            case EVENT_TYPE_WINDOW_RESIZE:
            {
                //Resize camera if size is not zero
                const WindowResizeEvent* pResizeEvent = static_cast<const WindowResizeEvent*>(pEvent);
                if (pResizeEvent->GetWidth() > 0 && pResizeEvent->GetHeight() > 0)
                {
                    CreateCamera(pResizeEvent->GetWidth(), pResizeEvent->GetHeight());
                }
                return false;
            }
            case EVENT_TYPE_KEY_PRESSED:
            {
                const KeyPressedEvent* pKeypressed = static_cast<const KeyPressedEvent*>(pEvent);
                
                //LOG_DEBUG_INFO("Key pressed\n");
                if (pKeypressed->GetKey() == KEY_1)
                {
                    GetWindow()->SetFullscreen(!GetWindow()->GetFullscreen());
                }
                return false;
            }
            case EVENT_TYPE_MOUSE_MOVED:
                //LOG_DEBUG_INFO("Mouse moved (x: %d, y: %d)\n", event.MouseMoveEvent.PosX, event.MouseMoveEvent.PosY);
                
                //Rotate camera
                //glm::vec2 diff = glm::vec2(m_Width / 2, m_Height / 2) - glm::vec2(event.MouseMoveEvent.PosX, event.MouseMoveEvent.PosY);
                //m_Camera.Rotate(glm::vec3(diff.y, diff.x, 0.0f));
                //m_Camera.CreateView();
                
                //Set position back to the middle
                //Input::SetMousePosition(m_Width / 2, m_Height / 2);
                return false;
                
            case EVENT_TYPE_MOUSE_BUTTON_PRESSED:
                //LOG_DEBUG_INFO("Mouse pressed\n");
                return false;
            
            case EVENT_TYPE_MOUSE_SCROLLED:
                /*if (event.MouseScrollEvent.Vertical)
                {
                    LOG_DEBUG_INFO("Vertical scroll\n");
                }
                else
                {
                    LOG_DEBUG_INFO("Horizontal scroll\n");
                }*/
                return false;
                
            case EVENT_TYPE_WINDOW_FOCUS_CHANGED:
                if (static_cast<const WindowFocusChangedEvent*>(pEvent)->HasFocus())
                {
                    LOG_DEBUG_INFO("Window got focus\n");
                }
                else
                {
                    LOG_DEBUG_INFO("Window lost focus\n");
                }
                return false;
                
            default:
                return false;
        }
    }
}
