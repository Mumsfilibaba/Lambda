#include "SandBox.h"
#include "System/Log.h"
#include "System/Input.h"
#include "glm/gtc/matrix_transform.hpp"

namespace Lambda
{
    //Declare vertex
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 UV;
    };
    
    
    //Create cubeverts
    std::vector<Vertex> CreateCubeVertices()
    {
        using namespace glm;
        
        std::vector<Vertex> vertices =
        {
            //FRONT FACE
            { vec3(-0.5f,  0.5f, -0.5f), vec3( 0.0f,  0.0f, -1.0f), vec2(0.0f, 0.0f) },
            { vec3( 0.5f,  0.5f, -0.5f), vec3( 0.0f,  0.0f, -1.0f), vec2(1.0f, 0.0f) },
            { vec3(-0.5f, -0.5f, -0.5f), vec3( 0.0f,  0.0f, -1.0f), vec2(0.0f, 1.0f) },
            { vec3( 0.5f, -0.5f, -0.5f), vec3( 0.0f,  0.0f, -1.0f), vec2(1.0f, 1.0f) },
            
            //BACK FACE
            { vec3( 0.5f,  0.5f,  0.5f), vec3( 0.0f,  0.0f,  1.0f), vec2(0.0f, 0.0f) },
            { vec3(-0.5f,  0.5f,  0.5f), vec3( 0.0f,  0.0f,  1.0f), vec2(1.0f, 0.0f) },
            { vec3( 0.5f, -0.5f,  0.5f), vec3( 0.0f,  0.0f,  1.0f), vec2(0.0f, 1.0f) },
            { vec3(-0.5f, -0.5f,  0.5f), vec3( 0.0f,  0.0f,  1.0f), vec2(1.0f, 1.0f) },
            
            //RIGHT FACE
            { vec3( 0.5f,  0.5f, -0.5f), vec3( 1.0f,  0.0f,  0.0f), vec2(0.0f, 0.0f) },
            { vec3( 0.5f,  0.5f,  0.5f), vec3( 1.0f,  0.0f,  0.0f), vec2(1.0f, 0.0f) },
            { vec3( 0.5f, -0.5f, -0.5f), vec3( 1.0f,  0.0f,  0.0f), vec2(0.0f, 1.0f) },
            { vec3( 0.5f, -0.5f,  0.5f), vec3( 1.0f,  0.0f,  0.0f), vec2(1.0f, 1.0f) },
            
            //LEFT FACE
            { vec3(-0.5f,  0.5f, -0.5f), vec3(-1.0f,  0.0f,  0.0f), vec2(0.0f, 0.0f) },
            { vec3(-0.5f,  0.5f,  0.5f), vec3(-1.0f,  0.0f,  0.0f), vec2(1.0f, 0.0f) },
            { vec3(-0.5f, -0.5f, -0.5f), vec3(-1.0f,  0.0f,  0.0f), vec2(0.0f, 1.0f) },
            { vec3(-0.5f, -0.5f,  0.5f), vec3(-1.0f,  0.0f,  0.0f), vec2(1.0f, 1.0f) },
            
            //ROOF FACE
            { vec3(-0.5f,  0.5f,  0.5f), vec3( 0.0f,  1.0f,  0.0f), vec2(0.0f, 0.0f) },
            { vec3( 0.5f,  0.5f,  0.5f), vec3( 0.0f,  1.0f,  0.0f), vec2(1.0f, 0.0f) },
            { vec3(-0.5f,  0.5f, -0.5f), vec3( 0.0f,  1.0f,  0.0f), vec2(0.0f, 1.0f) },
            { vec3( 0.5f,  0.5f, -0.5f), vec3( 0.0f,  1.0f,  0.0f), vec2(1.0f, 1.0f) },
            
            //FLOOR FACE
            { vec3(-0.5f, -0.5f, -0.5f), vec3( 0.0f, -1.0f,  0.0f), vec2(0.0f, 0.0f) },
            { vec3( 0.5f, -0.5f, -0.5f), vec3( 0.0f, -1.0f,  0.0f), vec2(1.0f, 0.0f) },
            { vec3(-0.5f, -0.5f,  0.5f), vec3( 0.0f, -1.0f,  0.0f), vec2(0.0f, 1.0f) },
            { vec3( 0.5f, -0.5f,  0.5f), vec3( 0.0f, -1.0f,  0.0f), vec2(1.0f, 1.0f) },
        };
        

        return vertices;
    }
    
    
    //Create cube-indices
    std::vector<uint32> CreateCubeIndices()
    {
        std::vector<uint32> indices =
        {
            //FRONT FACE
            0, 1, 2,
            1, 3, 2,
            
            //BACK FACE
            4, 5, 6,
            5, 7, 6,
            
            //RIGHT FACE
            8, 9, 10,
            9, 11, 10,
            
            //LEFT FACE
            14, 13, 12,
            14, 15, 13,
            
            //ROOF FACE
            16, 17, 18,
            17, 19, 18,
            
            //FLOOR FACE
            20, 21, 22,
            21, 23, 22
        };
        
        return indices;
    }
    
    
    //Create planeverts
    std::vector<Vertex> CreatePlaneVertices()
    {
        using namespace glm;
        
        std::vector<Vertex> vertices =
        {
            { vec3(-0.5f, -0.5f, 0.0f),    vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f) },
            { vec3(0.5f, -0.5f, 0.0f),     vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f) },
            { vec3(0.5f, 0.5f, 0.0f),      vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f) },
            { vec3(-0.5f, 0.5f, 0.0f),     vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f) }
        };
        
        
        return vertices;
    }
    
    
    //Create plane-indices
    std::vector<uint32> CreatePlaneIndices()
    {
        std::vector<uint32> indices =
        {
            0, 1, 2,
            2, 3, 0
        };
        
        return indices;
    }
    
    
    //Sandbox definitions
	SandBox::SandBox(const EngineParams& params)
		: Application(params),
		m_pLists(),
        m_pCurrentList(nullptr),
		m_pVS(nullptr),
		m_pPS(nullptr),
        m_pCompute(nullptr),
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
		EventLayer sandboxLayer = { SandBox::OnEvent, "SandBox" };
		EventDispatcher::PushEventLayer(sandboxLayer);

		//Init size
		m_Width = (float)GetWindow()->GetWidth();
		m_Height = (float)GetWindow()->GetHeight();

		IGraphicsDevice* pDevice = IGraphicsDevice::GetInstance();
        if (pDevice)
        {
            //Create commandlist
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

            //Create shaders
            m_pVS = IShader::CreateShaderFromFile(pDevice, "vert.spv", "main", SHADER_TYPE_VERTEX, SHADER_LANG_SPIRV);
            m_pPS = IShader::CreateShaderFromFile(pDevice, "frag.spv", "main", SHADER_TYPE_PIXEL, SHADER_LANG_SPIRV);
            //m_pVS = IShader::CreateShaderFromFile(pDevice, "Triangle.hlsl", "VSMain", SHADER_TYPE_VERTEX);
            //m_pPS = IShader::CreateShaderFromFile(pDevice, "Triangle.hlsl", "PSMain", SHADER_TYPE_PIXEL);
            //m_pCompute = IShader::CreateShaderFromFile(pDevice, "Texture2DMipMapGen.cso", "main", SHADER_TYPE_COMPUTE, SHADER_LANG_HLSL_COMPILED);
            
            //Define depthformat
            ResourceFormat depthFormat = FORMAT_D24_UNORM_S8_UINT;
            
            //Create pipelinestate
            {
                InputElement elements[]
                {
                    { "POSITION",   FORMAT_R32G32B32_FLOAT, 0, 0, sizeof(Vertex), 0,                     false },
                    { "NORMAL",     FORMAT_R32G32B32_FLOAT, 0, 1, sizeof(Vertex), sizeof(glm::vec3),     false },
                    { "TEXCOORD",   FORMAT_R32G32_FLOAT,    0, 2, sizeof(Vertex), sizeof(glm::vec3) * 2, false }
                };
                
                GraphicsPipelineStateDesc desc = {};
                desc.pVertexShader          = m_pVS;
                desc.pPixelShader           = m_pPS;
                desc.pInputElements         = elements;
                desc.InputElementCount      = sizeof(elements) / sizeof(InputElement);
                desc.Topology               = PRIMITIVE_TOPOLOGY_TRIANGLELIST;
                desc.Cull                   = CULL_MODE_BACK;
                desc.RenderTargetFormats[0] = pDevice->GetBackBufferFormat();
                desc.DepthStencilFormat     = depthFormat;
                desc.RenderTargetCount      = 1;
                desc.DepthTest              = false;
                
                pDevice->CreateGraphicsPipelineState(&m_pPipelineState, desc);
            }

            //Create vertexbuffer
            {
                std::vector<Vertex> vertices = CreateCubeVertices();

                BufferDesc desc = {};
                desc.Usage          = RESOURCE_USAGE_DEFAULT;
                desc.Flags          = BUFFER_FLAGS_VERTEX_BUFFER;
                desc.SizeInBytes    = sizeof(Vertex) * vertices.size();
                desc.StrideInBytes  = sizeof(Vertex);

                ResourceData data = {};
                data.pData          = vertices.data();
                data.SizeInBytes    = desc.SizeInBytes;

                pDevice->CreateBuffer(&m_pVertexBuffer, &data, desc);
            }
            
            //Create indexbuffer
            {
                std::vector<uint32> indices = CreateCubeIndices();
                
                BufferDesc desc = {};
                desc.Usage          = RESOURCE_USAGE_DEFAULT;
                desc.Flags          = BUFFER_FLAGS_INDEX_BUFFER;
                desc.SizeInBytes    = sizeof(uint32) * indices.size();
                desc.StrideInBytes  = sizeof(uint32);
                
                ResourceData data = {};
                data.pData          = indices.data();
                data.SizeInBytes    = desc.SizeInBytes;
                
                pDevice->CreateBuffer(&m_pIndexBuffer, &data, desc);
            }

            //Create colorbuffer
            {
                glm::vec4 color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

                BufferDesc desc = {};
                desc.Usage = RESOURCE_USAGE_DEFAULT;
                desc.Flags = BUFFER_FLAGS_CONSTANT_BUFFER;
                desc.SizeInBytes = sizeof(glm::vec4);//(uint32)Math::AlignUp(sizeof(Vec4f), 256);
                desc.StrideInBytes = sizeof(glm::vec4);

                ResourceData data = {};
                data.pData = &color;
                data.SizeInBytes = sizeof(color);

                pDevice->CreateBuffer(&m_pColorBuffer, &data, desc);
            }

            //Create camerabuffer
            {
                //Set camera
                m_Camera.SetPosition(glm::vec3(0.0f, 2.0f, -2.0f));
                m_Camera.SetRotation(glm::vec3(45.0f, 0.0f, 0.0f));
                m_Camera.CreateView();
                
                BufferDesc desc = {};
                desc.Usage = RESOURCE_USAGE_DEFAULT;
                desc.Flags = BUFFER_FLAGS_CONSTANT_BUFFER;
                desc.SizeInBytes = sizeof(CameraBuffer);
                desc.StrideInBytes = sizeof(CameraBuffer);

                ResourceData data = {};
                data.pData = &m_Camera;
                data.SizeInBytes = desc.SizeInBytes;

                pDevice->CreateBuffer(&m_pCameraBuffer, &data, desc);
            }

            //Init transforms
            m_TransformBuffer.Model = glm::mat4(1.0f);
            
            //Create TransformBuffer
            {
                BufferDesc desc = {};
                desc.Usage = RESOURCE_USAGE_DEFAULT;
                desc.Flags = BUFFER_FLAGS_CONSTANT_BUFFER;
                desc.SizeInBytes = sizeof(TransformBuffer);
                desc.StrideInBytes = sizeof(TransformBuffer);
                
                ResourceData data = {};
                data.pData = &m_TransformBuffer;
                data.SizeInBytes = desc.SizeInBytes;
                
                pDevice->CreateBuffer(&m_pTransformBuffer, &data, desc);
            }

            //Create texture
            m_pTexture = ITexture2D::CreateTextureFromFile(pDevice, "texture.jpg", TEXTURE_FLAGS_SHADER_RESOURCE, RESOURCE_USAGE_DEFAULT, FORMAT_R8G8B8A8_UNORM);

            //Create samplerstate
            {
                SamplerDesc desc = {};
                desc.AdressMode = SAMPLER_ADDRESS_MODE_REPEAT;
                
                pDevice->CreateSamplerState(&m_pSampler, desc);
            }

            //Close and execute commandlist
            m_pCurrentList->Close();
            pDevice->ExecuteCommandList(&m_pCurrentList, 1);

            //Wait for GPU
            pDevice->GPUWaitForFrame();
        }
	}


	void SandBox::OnUpdate(Time dt)
	{
        //Move camera
        constexpr float speed = 2.0f;
        if (Input::IsKeyDown(KEY_W))
            m_Camera.Translate(glm::vec3(0.0f, 0.0f, speed) * dt.AsSeconds());
        else if (Input::IsKeyDown(KEY_S))
            m_Camera.Translate(glm::vec3(0.0f, 0.0f, -speed) * dt.AsSeconds());
        
        if (Input::IsKeyDown(KEY_A))
            m_Camera.Translate(glm::vec3(-speed, 0.0f, 0.0f) * dt.AsSeconds());
        else if (Input::IsKeyDown(KEY_D))
            m_Camera.Translate(glm::vec3(speed, 0.0f, 0.0f) * dt.AsSeconds());
        
        //Rotate camera
         constexpr float rotation = 30.0f;
        if (Input::IsKeyDown(KEY_UP))
            m_Camera.Rotate(glm::vec3(rotation, 0.0f, 0.0f) * dt.AsSeconds());
        else if (Input::IsKeyDown(KEY_DOWN))
            m_Camera.Rotate(glm::vec3(-rotation, 0.0f, 0.0f) * dt.AsSeconds());
        
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


	void SandBox::OnRender(Time dt)
	{
        //Get current device
        IGraphicsDevice* pDevice = IGraphicsDevice::GetInstance();
        
        //Set commandlist for frame
        m_pCurrentList = m_pLists[pDevice->GetCurrentBackBufferIndex()];
        m_pCurrentList->Reset();
        
        //Set and clear rendertarget
        float color[] = { 0.392f, 0.584f, 0.929f, 1.0f };
        ITexture2D* pRenderTarget = pDevice->GetCurrentRenderTarget();
        ITexture2D* pDepthBuffer = pDevice->GetDepthStencil();
        
        //Transition and clear rendertarget
        m_pCurrentList->TransitionTexture(pRenderTarget, RESOURCE_STATE_RENDERTARGET_CLEAR);
        m_pCurrentList->ClearRenderTarget(pRenderTarget, color);
        
        //Transition and clear depthstencil
        //m_pCurrentList->TransitionTexture(pDepthBuffer, RESORUCE_STATE_DEPTH_STENCIL_CLEAR);
        //m_pCurrentList->ClearDepthStencil(pDepthBuffer, 1.0f, 0);
        
        //Transition and set rendertargets
        m_pCurrentList->TransitionTexture(pRenderTarget, RESOURCE_STATE_RENDERTARGET);
        m_pCurrentList->TransitionTexture(pDepthBuffer, RESOURCE_STATE_DEPTH_WRITE);
        m_pCurrentList->SetRenderTarget(pRenderTarget, pDepthBuffer);
        
        //Set scissor and viewport
        Rectangle scissorrect;
        scissorrect.X       = 0.0f;
        scissorrect.Y       = 0.0f;
        scissorrect.Width   = pDevice->GetCurrentSwapChainWidth();
        scissorrect.Height  = pDevice->GetCurrentSwapChainHeight();
        
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
        glm::vec4 colorBuff = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        
        ResourceData data = {};
        data.pData = &colorBuff;
        data.SizeInBytes = sizeof(glm::vec4);
        
        m_pCurrentList->UpdateBuffer(m_pColorBuffer, &data);
        
        //Update camera buffer
        m_CameraBuffer.View = m_Camera.GetView();
        m_CameraBuffer.Proj = m_Camera.GetProjection();

        data.pData = &m_CameraBuffer;
        data.SizeInBytes = sizeof(CameraBuffer);
        m_pCurrentList->UpdateBuffer(m_pCameraBuffer, &data);
        
        //Set buffers
        IBuffer* vsBuffers[] = { m_pCameraBuffer, m_pTransformBuffer };
        m_pCurrentList->VSSetConstantBuffers(vsBuffers, 2, 0);
        m_pCurrentList->PSSetConstantBuffers(&m_pColorBuffer, 1, 0);
        
        //Set texture and samplers
        m_pCurrentList->PSSetTextures(&m_pTexture, 1, 0);
        m_pCurrentList->PSSetSamplers(&m_pSampler, 1, 0);
        
        //Set vertex- and indexbuffer
        m_pCurrentList->SetVertexBuffer(m_pVertexBuffer, 0);
        m_pCurrentList->SetIndexBuffer(m_pIndexBuffer);
        
        //Setup rotation
        static glm::mat4 rotation = glm::mat4(1.0f);
        rotation = glm::rotate(rotation, glm::radians(45.0f) * dt.AsSeconds(), glm::vec3(0.0f, 1.0f, 0.0f));
        
        //Draw cube 1
        //Update transforms
        m_TransformBuffer.Model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * rotation;
        
        data.pData = &m_TransformBuffer;
        data.SizeInBytes = sizeof(TransformBuffer);
        m_pCurrentList->UpdateBuffer(m_pTransformBuffer, &data);
        
        //Draw
        m_pCurrentList->DrawIndexedInstanced(36, 1, 0, 0, 0);
        
        //Draw cube 2
        //Update transforms
        m_TransformBuffer.Model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)) * rotation;
        
        data.pData = &m_TransformBuffer;
        data.SizeInBytes = sizeof(TransformBuffer);
        m_pCurrentList->UpdateBuffer(m_pTransformBuffer, &data);
        
        //Draw
        m_pCurrentList->DrawIndexedInstanced(36, 1, 0, 0, 0);
        
        //Transition rendertarget to present
        m_pCurrentList->TransitionTexture(pRenderTarget, RESOURCE_STATE_RENDERTARGET_PRESENT);
        m_pCurrentList->Close();
        
        //Present
        pDevice->ExecuteCommandList(&m_pCurrentList, 1);
        pDevice->Present();
	}


	void SandBox::OnRelease()
	{
		IGraphicsDevice* pDevice = IGraphicsDevice::GetInstance();
        if (pDevice)
        {
            pDevice->WaitForGPU();

            for (uint32 i = 0; i < 3; i++)
            {
                pDevice->DestroyCommandList(&(m_pLists[i]));
            }

            pDevice->DestroyShader(&m_pVS);
            pDevice->DestroyShader(&m_pPS);
            //pDevice->DestroyShader(&m_pCompute);

            pDevice->DestroyGraphicsPipelineState(&m_pPipelineState);
            pDevice->DestroyBuffer(&m_pVertexBuffer);
            pDevice->DestroyBuffer(&m_pIndexBuffer);
            pDevice->DestroyBuffer(&m_pColorBuffer);
            pDevice->DestroyBuffer(&m_pCameraBuffer);
            pDevice->DestroyBuffer(&m_pTransformBuffer);
            pDevice->DestroyTexture2D(&m_pTexture);
            pDevice->DestroySamplerState(&m_pSampler);
        }
	}


	void SandBox::CreateCamera(uint32 width, uint32 height)
    {
        m_Camera.SetAspect(float(width), float(height));
        m_Camera.CreateProjection();
	}
    
    
    //Event handler function for the instance
    bool SandBox::EventHandler(const Event &event)
    {
        switch (event.Type)
        {
            case EVENT_TYPE_WINDOW_RESIZE:
                //Resize camera if size is not zero
                if (event.WindowResize.Width > 0 && event.WindowResize.Height > 0)
                {
                    CreateCamera(event.WindowResize.Width, event.WindowResize.Height);
                }
                return false;
                
            case EVENT_TYPE_KEYDOWN:
                //LOG_DEBUG_INFO("Key pressed\n");
                return false;
                
            case EVENT_TYPE_MOUSE_MOVED:
                //LOG_DEBUG_INFO("Mouse moved (x: %d, y: %d)\n", event.MouseMoveEvent.PosX, event.MouseMoveEvent.PosY);
                
                //Rotate camera
                //glm::vec2 diff = glm::vec2(m_Width / 2, m_Height / 2) - glm::vec2(event.MouseMoveEvent.PosX, event.MouseMoveEvent.PosY);
                //m_Camera.Rotate(glm::vec3(diff.y, diff.x, 0.0f));
                //m_Camera.CreateView();
                
                //Set position back to the middle
                //Input::SetMousePosition(m_Width / 2, m_Height / 2);
                return false;
                
            case EVENT_TYPE_MOUSE_BUTTONDOWN:
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
                
            case EVENT_TYPE_FOCUS_CHANGED:
                if (event.FocusChanged.HasFocus)
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
        
        return false;
    }

    
    //Call the instance eventhandler
	bool SandBox::OnEvent(const Event& event)
	{
        SandBox& instance = (SandBox&)GetInstance();
        return instance.EventHandler(event);
	}
}
