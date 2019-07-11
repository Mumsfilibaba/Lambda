#include "SandBox.h"
#include "System/Log.h"
#include "System/Input.h"
#include "glm/gtc/matrix_transform.hpp"

namespace Lambda
{
	SandBox::SandBox(const EngineParams& params)
		: Application(params),
		m_pLists(),
        m_pCurrentList(nullptr),
		m_pVS(nullptr),
		m_pPS(nullptr),
        m_pCompute(nullptr),
		m_pVertexBuffer(nullptr),
        m_pIndexBuffer(nullptr),
		m_pDepthBuffer(nullptr),
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

            //Declare vertex
            struct Vertex
            {
                glm::vec3 Position;
                glm::vec2 UV;
            };
            
            //Create pipelinestate
            {
                InputElement elements[]
                {
                    { "POSITION",   FORMAT_R32G32B32_FLOAT, 0, 0, sizeof(Vertex), 0, false },
                    { "TEXCOORD",   FORMAT_R32G32_FLOAT,    0, 1, sizeof(Vertex), sizeof(glm::vec3), false }
                };
                
                GraphicsPipelineStateDesc desc = {};
                desc.pVertexShader      = m_pVS;
                desc.pPixelShader       = m_pPS;
                desc.pInputElements     = elements;
                desc.InputElementCount  = sizeof(elements) / sizeof(InputElement);
                desc.Topology           = PRIMITIVE_TOPOLOGY_TRIANGLELIST;

                pDevice->CreateGraphicsPipelineState(&m_pPipelineState, desc);
            }

            //Create vertexbuffer
            {
                Vertex vertices[] =
                {
                    { glm::vec3(-0.5f, -0.5f, 0.0f),    glm::vec2(0.0f, 0.0f) },
                    { glm::vec3(0.5f, -0.5f, 0.0f),     glm::vec2(1.0f, 0.0f) },
                    { glm::vec3(0.5f, 0.5f, 0.0f),      glm::vec2(1.0f, 1.0f) },
                    { glm::vec3(-0.5f, 0.5f, 0.0f),     glm::vec2(0.0f, 1.0f) }
                };

                BufferDesc desc = {};
                desc.Usage = RESOURCE_USAGE_DEFAULT;
                desc.Flags = BUFFER_FLAGS_VERTEX_BUFFER;
                desc.SizeInBytes = sizeof(vertices);
                desc.StrideInBytes = sizeof(Vertex);

                ResourceData data = {};
                data.pData = &vertices;
                data.SizeInBytes = desc.SizeInBytes;

                pDevice->CreateBuffer(&m_pVertexBuffer, &data, desc);
                m_pCurrentList->TransitionBuffer(m_pVertexBuffer, RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
            }
            
            //Create indexbuffer
            {
                uint32 indices[] =
                {
                    0, 1, 2,
                    2, 3, 0
                };
                
                BufferDesc desc = {};
                desc.Usage = RESOURCE_USAGE_DEFAULT;
                desc.Flags = BUFFER_FLAGS_INDEX_BUFFER;
                desc.SizeInBytes = sizeof(indices);
                desc.StrideInBytes = sizeof(uint32);
                
                ResourceData data = {};
                data.pData = &indices;
                data.SizeInBytes = desc.SizeInBytes;
                
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
                CreateCamera(GetWindow()->GetWidth(), GetWindow()->GetHeight());

                BufferDesc desc = {};
                desc.Usage = RESOURCE_USAGE_DEFAULT;
                desc.Flags = BUFFER_FLAGS_CONSTANT_BUFFER;
                desc.SizeInBytes = sizeof(CameraBuffer);
                desc.StrideInBytes = sizeof(CameraBuffer);

                ResourceData data = {};
                data.pData = &m_Camera;
                data.SizeInBytes = desc.SizeInBytes;

                pDevice->CreateBuffer(&m_pCameraBuffer, &data, desc);
                m_pCurrentList->TransitionBuffer(m_pCameraBuffer, RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
            }

            //Create depthbuffer
            /*{
                Texture2DDesc desc = {};
                desc.Usage = RESOURCE_USAGE_DEFAULT;
                desc.Flags = TEXTURE_FLAGS_DEPTH_STENCIL;
                desc.ArraySize = 1;
                desc.Width = GetWindow()->GetWidth();
                desc.Height = GetWindow()->GetHeight();
                desc.Format = FORMAT_D24_UNORM_S8_UINT;
                desc.SampleCount = 1;
                desc.MipLevels = 0;
                desc.ClearValue.Depth = 1.0f;
                desc.ClearValue.Stencil = 0;

                pDevice->CreateTexture2D(&m_pDepthBuffer, nullptr, desc);
            }*/

            //Create texture
            m_pTexture = ITexture2D::CreateTextureFromFile(pDevice, "texture.jpg", TEXTURE_FLAGS_SHADER_RESOURCE, RESOURCE_USAGE_DEFAULT, FORMAT_R8G8B8A8_UNORM);
            m_pCurrentList->TransitionTexture(m_pTexture, RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

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
	}


	void SandBox::OnRender(Time dt)
	{
		IGraphicsDevice* pDevice = IGraphicsDevice::GetInstance();
        if (pDevice)
        {
            //Set commandlist for frame
            m_pCurrentList = m_pLists[pDevice->GetCurrentBackBufferIndex()];
            if (m_pCurrentList)
            {
                m_pCurrentList->Reset();
                
                //Set and clear rendertarget
                float color[] = { 0.392f, 0.584f, 0.929f, 1.0f };
                ITexture2D* pRenderTarget = pDevice->GetCurrentRenderTarget();
                
                //m_pCurrentList->TransitionTexture(pRenderTarget, RESOURCE_STATE_RENDERTARGET);
                m_pCurrentList->ClearRenderTarget(pRenderTarget, color);
                //m_pCurrentList->TransitionTexture(m_pDepthBuffer, RESOURCE_STATE_DEPTH_WRITE);
                m_pCurrentList->ClearDepthStencil(m_pDepthBuffer, 1.0f, 0);
                m_pCurrentList->SetRenderTarget(pRenderTarget, m_pDepthBuffer);
                
                //Set scissor and viewport
                Rectangle scissorrect;
                scissorrect.X       = 0.0f;
                scissorrect.Y       = 0.0f;
                scissorrect.Width   = m_Width;
                scissorrect.Height  = m_Height;
                
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
                
                //Set constantbuffers
                /*static Random random;
                static Vec4f colorBuff    = random.GenerateVector4();
                static Vec4f beginBuff    = random.GenerateVector4();
                static Vec4f endBuff      = random.GenerateVector4();
                static float timer        = 0.0f;
                timer += dt.AsSeconds();
                
                //Lerp color
                colorBuff = Store(Lerp(timer, Load(beginBuff), Load(endBuff)));
                if (timer >= 1.0f)
                {
                    beginBuff = endBuff;
                    endBuff = random.GenerateVector4();
                    timer = 0.0f;
                }*/
                
                //Update data
                glm::vec4 colorBuff = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                
                ResourceData data = {};
                data.pData = &colorBuff;
                data.SizeInBytes = sizeof(glm::vec4);
                
                m_pCurrentList->TransitionBuffer(m_pColorBuffer, RESOURCE_STATE_COPY_DEST);
                m_pCurrentList->UpdateBuffer(m_pColorBuffer, &data);
                m_pCurrentList->TransitionBuffer(m_pColorBuffer, RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
                
                m_pCurrentList->VSSetConstantBuffers(&m_pCameraBuffer, 1, 0);
                m_pCurrentList->PSSetConstantBuffers(&m_pColorBuffer, 1, 0);
                
                //Set texture and samplers
                m_pCurrentList->PSSetTextures(&m_pTexture, 1, 0);
                m_pCurrentList->PSSetSamplers(&m_pSampler, 1, 0);
                
                //Set vertex- and indexbuffer
                m_pCurrentList->SetVertexBuffer(m_pVertexBuffer, 0);
                m_pCurrentList->SetIndexBuffer(m_pIndexBuffer);
                
                m_pCurrentList->DrawIndexedInstanced(6, 1, 0, 0, 0);
                
                //m_pCurrentList->TransitionTexture(pRenderTarget, RESOURCE_STATE_PRESENT_COMMON);
                
                m_pCurrentList->Close();
                
                //Present
                pDevice->ExecuteCommandList(&m_pCurrentList, 1);
                pDevice->Present(0);
                pDevice->GPUWaitForFrame();
            }
        }
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
            //pDevice->DestroyTexture2D(&m_pDepthBuffer);
            pDevice->DestroyTexture2D(&m_pTexture);
            pDevice->DestroySamplerState(&m_pSampler);
        }
	}


	void SandBox::CreateCamera(uint32 width, uint32 height)
	{
        m_Camera.View = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        m_Camera.Proj = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 100.0f);
	}


	bool SandBox::OnEvent(const Event& event)
	{
		if (event.Type == EVENT_TYPE_WINDOW_RESIZE)
		{
			/*SandBox& instance = (SandBox&)GetInstance();

			//Set size variable
			instance.m_Width = (float)event.WindowResize.Width;
			instance.m_Height = (float)event.WindowResize.Height;

			//if size is zero then do not resize
			if (event.WindowResize.Width > 0 && event.WindowResize.Height > 0)
			{
				IGraphicsDevice* pDevice = IGraphicsDevice::GetInstance();
                if (!pDevice)
                {
                    return false;
                }
                
				pDevice->WaitForGPU();

				//Release depthbuffer
				pDevice->DestroyTexture2D(&instance.m_pDepthBuffer);

				//Create depthbuffer
				Texture2DDesc desc = {};
				desc.Usage = RESOURCE_USAGE_DEFAULT;
				desc.Flags = TEXTURE_FLAGS_DEPTH_STENCIL;
				desc.ArraySize = 1;
				desc.Width = event.WindowResize.Width;
				desc.Height = event.WindowResize.Height;
				desc.Format = FORMAT_D24_UNORM_S8_UINT;
				desc.SampleCount = 1;
				desc.MipLevels = 0;
                desc.ClearValue.Depth = 1.0f;
                desc.ClearValue.Stencil = 0;

				//TODO: needs to release the descriptor in DX12-backend
				pDevice->CreateTexture2D(&instance.m_pDepthBuffer, nullptr, desc);
                if (instance.m_pDepthBuffer)
                {
                    LOG_DEBUG_INFO("Resized depthbuffer\n");
                }

				//Update camera
				instance.CreateCamera(event.WindowResize.Width, event.WindowResize.Height);

				ResourceData data = {};
				data.pData = &instance.m_Camera;
				data.SizeInBytes = sizeof(CameraBuffer);

                //Update camerabuffer
                if (instance.m_pCurrentList)
                {
                    instance.m_pCurrentList->Reset();
                    
                    instance.m_pCurrentList->TransitionResource(instance.m_pCameraBuffer, RESOURCE_STATE_COPY_DEST);
                    instance.m_pCurrentList->UpdateBuffer(instance.m_pCameraBuffer, &data);
                    instance.m_pCurrentList->TransitionResource(instance.m_pCameraBuffer, RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
                    
                    //Copy new camera
                    instance.m_pCurrentList->Close();
                    pDevice->ExecuteCommandList(&instance.m_pCurrentList, 1);
                }

				pDevice->WaitForGPU();
			}*/
		}
        else if (event.Type == EVENT_TYPE_KEYDOWN)
        {
            LOG_DEBUG_INFO("Key pressed\n");
        }
        else if (event.Type == EVENT_TYPE_MOUSE_MOVED)
        {
            //LOG_DEBUG_INFO("Mouse moved (x: %d, y: %d)\n", event.MouseMoveEvent.PosX, event.MouseMoveEvent.PosY);
        }
        else if (event.Type == EVENT_TYPE_MOUSE_BUTTONDOWN)
        {
            //LOG_DEBUG_INFO("Mouse pressed\n");
        }
        else if (event.Type == EVENT_TYPE_MOUSE_SCROLLED)
        {
            if (event.MouseScrollEvent.Vertical)
            {
                //LOG_DEBUG_INFO("Vertical scroll\n");
            }
            else
            {
                //LOG_DEBUG_INFO("Horizontal scroll\n");
            }
        }
        else if (event.Type == EVENT_TYPE_FOCUS_CHANGED)
        {
            if (event.FocusChanged.HasFocus)
            {
                LOG_DEBUG_INFO("Window got focus\n");
            }
            else
            {
                LOG_DEBUG_INFO("Window lost focus\n");
            }
        }

		return false;
	}
}
