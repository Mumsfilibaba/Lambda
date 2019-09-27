#include "SandBox.h"
#include "System/Log.h"
#include "System/Input.h"
#include "Events/WindowEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Graphics/MeshFactory.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#define SINGLE_CUBE
#define RGB_F(r, g, b) float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f

namespace Lambda
{
    struct Transform
    {
        float Position[3] = { 0.0f, 0.0f, 0.0f };
        float Rotation[3] = { 0.0f, 0.0f, 0.0f };
        float Scale[3] = { 1.0f, 1.0f, 1.0f };
    };

    static Transform transformLight;
    static Transform transformRevolver;
    static float lightColor[4];

    //-------
    //SandBox
    //-------

    SandBox::SandBox(const EngineParams& params)
        : Application(params)
    {
        PushLayer(DBG_NEW SandBoxLayer());
    }


    //------------
    //SandBoxLayer
    //------------

	SandBoxLayer::SandBoxLayer()
         : Layer("SandBoxLayer"),
		m_pVS(nullptr),
		m_pPS(nullptr),
        m_pPipelineState(nullptr),
		m_Mesh()
	{
	}


	void SandBoxLayer::OnLoad()
	{
        //Init transforms
        transformRevolver.Position[1] = -1.0f;
        transformRevolver.Rotation[1] = 180.0f;
        
        transformLight.Position[0] = -1.5f;
        transformLight.Position[1] = 0.5f;
        transformLight.Position[2] = -1.0f;
        
        //Init color
        lightColor[0] = 255.0f / 255.0f;
        lightColor[1] = 241.0f / 255.0f;
        lightColor[2] = 224.0f / 255.0f;
        lightColor[3] = 1.0f;

		//Setup camera
		m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, -2.0f));
		m_Camera.CreateView();

		//Init size
        Application& app = Application::Get();
		IGraphicsDevice* pDevice = IGraphicsDevice::Get();
        if (pDevice)
        {
			ICommandList* pTempList = nullptr;
			pDevice->CreateCommandList(&pTempList, COMMAND_LIST_TYPE_GRAPHICS);
			pTempList->Reset();

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
				desc.NumConstantBlocks	= 0;
				desc.pConstantBlocks	= nullptr;
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
				desc.pName						= "MainPipelineState";
                desc.pVertexShader				= m_pVS;
                desc.pPixelShader				= m_pPS;
                desc.pInputElements				= elements;
                desc.InputElementCount			= sizeof(elements) / sizeof(InputElement);
                desc.Cull						= CULL_MODE_BACK;
				desc.FillMode					= POLYGON_MODE_FILL;
                desc.Topology					= PRIMITIVE_TOPOLOGY_TRIANGLELIST;
				desc.pRenderPass				= app.GetRenderer().GetRenderPass();
				desc.pResourceState				= m_pResourceState;
                desc.DepthTest					= true;
				desc.EnableBlending				= false;
				desc.FrontFaceCounterClockWise	= false;                
                pDevice->CreateGraphicsPipelineState(&m_pPipelineState, desc);
            }

            //Create vertexbuffer
			MeshData mesh = MeshFactory::CreateFromFile("revolver.obj");
			m_Mesh.IndexCount = uint32(mesh.Indices.size());
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

                pDevice->CreateBuffer(&m_Mesh.pVertexBuffer, &data, desc);
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
                
                pDevice->CreateBuffer(&m_Mesh.pIndexBuffer, &data, desc);
            }

            //Init transforms
            m_TransformBuffer.Model = glm::mat4(1.0f);

            //Create texture
            m_pAlbedoMap = ITexture::CreateTextureFromFile(pDevice, "revolver_albedo.png", TEXTURE_FLAGS_SHADER_RESOURCE | TEXTURE_FLAGS_GENEATE_MIPS, RESOURCE_USAGE_DEFAULT, FORMAT_R8G8B8A8_UNORM);
            m_pNormalMap = ITexture::CreateTextureFromFile(pDevice, "revolver_normal.png", TEXTURE_FLAGS_SHADER_RESOURCE | TEXTURE_FLAGS_GENEATE_MIPS, RESOURCE_USAGE_DEFAULT, FORMAT_R8G8B8A8_UNORM);
			pTempList->TransitionTexture(m_pAlbedoMap, RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0, LAMBDA_TRANSITION_ALL_MIPS);
			pTempList->TransitionTexture(m_pNormalMap, RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0, LAMBDA_TRANSITION_ALL_MIPS);

            //Create samplerstate
            TextureDesc textureDesc = m_pAlbedoMap->GetDesc();
            SamplerStateDesc desc = {};
            desc.AdressMode = SAMPLER_ADDRESS_MODE_REPEAT;
			desc.MinMipLOD	= 0.0f;
			desc.MaxMipLOD	= float(textureDesc.MipLevels);
			desc.MipLODBias	= 0.0f;
			desc.Anisotropy = 16.0f;
            pDevice->CreateSamplerState(&m_pSamplerState, desc);

            //Close and execute commandlist
			pTempList->Close();
            pDevice->ExecuteCommandList(&pTempList, 1);
            //Wait for GPU
            pDevice->WaitForGPU();
			//Destroy temp commandlist
			pDevice->DestroyCommandList(&pTempList);
        }
	}


	void SandBoxLayer::OnUpdate(Timestep dt)
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


	void SandBoxLayer::OnRender(Timestep dt, ICommandList* pCurrentList)
	{
        static Clock clock;
        
        //Get current device
        IGraphicsDevice* pDevice = IGraphicsDevice::Get();
		uint32 backBufferIndex = pDevice->GetBackBufferIndex();
               
        //Update light
        static LightBuffer lightBuffer  =
        {
            glm::vec4(lightColor[0], lightColor[1], lightColor[2], lightColor[3]),
            glm::vec3(transformLight.Position[0], transformLight.Position[1], transformLight.Position[2])
        };
        
        lightBuffer.Color       = glm::vec4(lightColor[0], lightColor[1], lightColor[2], lightColor[3]);
        lightBuffer.Position    = glm::vec3(transformLight.Position[0], transformLight.Position[1], transformLight.Position[2]);

		m_Material.pAlbedoMap		= m_pAlbedoMap;
		m_Material.pNormalMap		= m_pNormalMap;
		m_Material.pPipelineState	= m_pPipelineState;
		m_Material.pResourceState	= m_pResourceState;
		m_Material.pSamplerState	= m_pSamplerState;
		m_Material.Color			= glm::vec4(RGB_F(255, 255, 255), 1.0f);

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
		//Draw
		Application::Get().GetRenderer().BeginScene(m_Camera, lightBuffer);
        //Setup transforms
        glm::mat4 translation	= glm::translate(glm::mat4(1.0f), glm::vec3(transformRevolver.Position[0], transformRevolver.Position[1], transformRevolver.Position[2]));
        glm::mat4 rotation		= glm::eulerAngleYXZ(glm::radians(transformRevolver.Rotation[1]), glm::radians(transformRevolver.Rotation[0]), glm::radians(transformRevolver.Rotation[2]));
        glm::mat4 scale			= glm::scale(glm::mat4(1.0f), glm::vec3(transformRevolver.Scale[0], transformRevolver.Scale[1], transformRevolver.Scale[2]));        
		m_TransformBuffer.Model = translation * rotation * scale;

        Application::Get().GetRenderer().Submit(m_Mesh, m_Material, m_TransformBuffer);
		
		translation = glm::translate(glm::mat4(1.0f), glm::vec3(transformRevolver.Position[0] - 2.0f, transformRevolver.Position[1], transformRevolver.Position[2]));
		m_TransformBuffer.Model = translation * rotation * scale;

		Application::Get().GetRenderer().Submit(m_Mesh, m_Material, m_TransformBuffer);

		Application::Get().GetUILayer()->Draw(pCurrentList);
		Application::Get().GetRenderer().EndScene();
#endif
	}


	void SandBoxLayer::OnRelease()
	{
		IGraphicsDevice* pDevice = IGraphicsDevice::Get();
        if (pDevice)
        {
            pDevice->WaitForGPU();

            pDevice->DestroyShader(&m_pVS);
            pDevice->DestroyShader(&m_pPS);
			pDevice->DestroyResourceState(&m_pResourceState);
            pDevice->DestroyGraphicsPipelineState(&m_pPipelineState);
            pDevice->DestroyBuffer(&m_Mesh.pVertexBuffer);
            pDevice->DestroyBuffer(&m_Mesh.pIndexBuffer);
            pDevice->DestroyTexture(&m_pAlbedoMap);
            pDevice->DestroyTexture(&m_pNormalMap);
            pDevice->DestroySamplerState(&m_pSamplerState);
        }
	}


    void SandBoxLayer::OnRenderUI(Timestep dt)
    {
        //Update userinterface
		ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Entities", NULL))
		{
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();

		struct funcs
		{
			static void ShowLight(uint32 uid, float* pColor)
			{
				ImGui::PushID(uid);
				ImGui::AlignTextToFramePadding();
				bool nodeOpen = ImGui::TreeNode("Light", "Light");
				ImGui::NextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::NextColumn();

				if (nodeOpen)
				{
					ImGui::PushID(0);
					{
						ImGui::AlignTextToFramePadding();
						ImGui::TreeNodeEx("Color", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Color");
						ImGui::NextColumn();
						ImGui::ColorPicker4("##picker", pColor, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
						ImGui::NextColumn();
					}
					ImGui::PopID();
					ImGui::TreePop();
				}
				ImGui::PopID();
			}

			static void ShowTransform(uint32 uid, float* pPosition, float* pRotation, float* pScale)
			{
				ImGui::AlignTextToFramePadding();
				ImGui::PushID(uid);
				bool nodeOpen = ImGui::TreeNode("Transform", "Transform");
				ImGui::NextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::NextColumn();

				if (nodeOpen)
				{
					ImGui::AlignTextToFramePadding();
					ImGui::PushID(0); // Use field index as identifier.
					{
						// Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
						ImGui::TreeNodeEx("Position", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, "Position");
						ImGui::NextColumn();
						ImGui::SetNextItemWidth(-1);
						ImGui::DragFloat3("", pPosition, 0.05f);
						ImGui::NextColumn();
					}
					ImGui::PopID();

					ImGui::PushID(1); // Use field index as identifier.
					{
						ImGui::TreeNodeEx("Rotation", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, "Rotation");
						ImGui::NextColumn();
						ImGui::SetNextItemWidth(-1);
						ImGui::DragFloat3("", pRotation);
						ImGui::NextColumn();
					}
					ImGui::PopID();

					ImGui::PushID(2); // Use field index as identifier.
					{
						ImGui::TreeNodeEx("Scale", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, "Scale");
						ImGui::NextColumn();
						ImGui::SetNextItemWidth(-1);
						ImGui::DragFloat3("", pScale, 0.05f);
						ImGui::NextColumn();
					}
					ImGui::PopID();
					ImGui::TreePop();
				}
				ImGui::PopID();
			}

			static void ShowObject(const char* prefix, int uid)
			{
				ImGui::PushID(uid);                      // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
				ImGui::AlignTextToFramePadding();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.
				bool nodeOpen = ImGui::TreeNode("Object", "%s_%u", prefix, uid);
				ImGui::NextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::NextColumn();
				if (nodeOpen)
				{
					if (uid == 0)
					{
						ShowTransform(0, transformRevolver.Position, transformRevolver.Rotation, transformRevolver.Scale);
					}
					else if (uid == 1)
					{
						ShowLight(0, lightColor);
						ShowTransform(1, transformLight.Position, transformLight.Rotation, transformLight.Scale);
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
		};


		funcs::ShowObject("Entity", 0);
		funcs::ShowObject("Entity", 1);

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();
		ImGui::End();
    }


    bool SandBoxLayer::OnEvent(const Event& event)
    {
        EventForwarder forwarder;
        forwarder.ForwardEvent(this, &SandBoxLayer::OnKeyPressed, event);
        forwarder.ForwardEvent(this, &SandBoxLayer::OnWindowResize, event);
        return false;
    }


    uint32 SandBoxLayer::GetRecivableCategories() const
    {
        return EVENT_CATEGORY_ALL;
    }


	bool SandBoxLayer::OnWindowResize(const WindowResizeEvent& event)
	{
		if (event.GetWidth() > 0 && event.GetHeight() > 0)
		{
			CreateCamera(event.GetWidth(), event.GetHeight());
		}
		return false;
	}


	bool SandBoxLayer::OnKeyPressed(const KeyPressedEvent& event)
	{
		if (event.GetKey() == KEY_1)
		{
            IWindow* pWindow = Application::Get().GetWindow();
			pWindow->SetFullscreen(!pWindow->GetFullscreen());
		}
		return false;
	}


	void SandBoxLayer::CreateCamera(uint32 width, uint32 height)
    {
        m_Camera.SetAspect(float(width), float(height));
        m_Camera.CreateProjection();
	}
}
