#include "SandBox.h"
#include "System/Log.h"
#include "System/Input.h"
#include "Events/WindowEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Graphics/MeshFactory.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

#define SINGLE_CUBE
#define RGB_F(r, g, b) float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f

namespace Lambda
{
    struct Transform
    {
        glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);
    };

    static Transform transformLight;
    static Transform transformRevolver;
	static Transform transformRevolver2;
	static std::vector<Transform> sphereTransforms;
	static glm::vec4 lightColor;

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
		m_VS(nullptr),
		m_PS(nullptr),
		m_Mesh(),
		m_SphereMesh(),
		m_Material(),
		m_RedMaterial(),
		m_AlbedoMap(nullptr),
		m_NormalMap(nullptr),
		m_SamplerState(nullptr),
		m_PipelineState(nullptr),
		m_VariableTable(nullptr),
		m_Camera(),
		m_TransformBuffer()
	{
	}


	void SandBoxLayer::OnLoad()
	{
        //Init transforms
        transformRevolver.Position.y = -1.0f;
        transformRevolver.Rotation.y = 180.0f;

		transformRevolver2.Position.x = 2.0f;
		transformRevolver2.Position.y = -1.0f;
		transformRevolver2.Rotation.y = 180.0f;
       
        transformLight.Position.x = -1.5f;
        transformLight.Position.y = 0.5f;
        transformLight.Position.z = -1.0f;

		constexpr uint32 sphereWidth = 10;
		for (uint32 y = 0; y < sphereWidth; y++)
		{
			for (uint32 x = 0; x < sphereWidth; x++)
			{
				Transform transform = {};
				transform.Position = glm::vec3(-(float(sphereWidth) / 2.0f) + float(x), -(float(sphereWidth) / 2.0f) + float(y), 2.0f);
				sphereTransforms.emplace_back(transform);
			}
		}
        
        //Init color
        lightColor = glm::vec4(255.0f / 255.0f, 241.0f / 255.0f, 224.0f / 255.0f, 1.0f);

		//Setup camera
		m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, -2.0f));
		m_Camera.CreateView();

		//Init size
		IDevice* pDevice = IDevice::Get();
        if (pDevice)
        {
            AutoRef<IDeviceContext> context = pDevice->GetImmediateContext();
			context->Begin();

            //Create shaders
			DeviceDesc deviceDesc = pDevice->GetDesc();
			if (deviceDesc.Api == GRAPHICS_API_VULKAN)
			{
				m_VS = IShader::CreateShaderFromFile(pDevice, "vert.spv", "main", SHADER_STAGE_VERTEX, SHADER_LANG_SPIRV);
				m_PS = IShader::CreateShaderFromFile(pDevice, "frag.spv", "main", SHADER_STAGE_PIXEL,	SHADER_LANG_SPIRV);
			}
			else if (deviceDesc.Api == GRAPHICS_API_D3D12)
			{
				m_VS = IShader::CreateShaderFromFile(pDevice, "Triangle.hlsl", "VSMain", SHADER_STAGE_VERTEX,	SHADER_LANG_HLSL);
				m_PS = IShader::CreateShaderFromFile(pDevice, "Triangle.hlsl", "PSMain", SHADER_STAGE_PIXEL,	SHADER_LANG_HLSL);
			}
            

            //Create pipelinestate
            {
				InputElementDesc elements[]
                {
                    { "POSITION",   FORMAT_R32G32B32_FLOAT, 0, 0, sizeof(Vertex), 0,                     false },
                    { "NORMAL",     FORMAT_R32G32B32_FLOAT, 0, 1, sizeof(Vertex), sizeof(glm::vec3),     false },
                    { "TANGENT",    FORMAT_R32G32B32_FLOAT, 0, 2, sizeof(Vertex), sizeof(glm::vec3) * 2, false },
                    { "TEXCOORD",   FORMAT_R32G32_FLOAT,    0, 3, sizeof(Vertex), sizeof(glm::vec3) * 3, false },
                };
                
				InputLayoutDesc vertexInput = {};
				vertexInput.ElementCount	= sizeof(elements) / sizeof(InputElementDesc);
				vertexInput.pElements		= elements;

				RasterizerStateDesc rasterizerState = {};
				rasterizerState.Cull		= CULL_MODE_BACK;
				rasterizerState.FillMode	= POLYGON_MODE_FILL;
				rasterizerState.FrontFaceCounterClockWise = false;
				
				BlendStateDesc blendState = {};
				blendState.EnableBlending = false;

				DepthStencilStateDesc depthStencilState = {};
				depthStencilState.DepthTest = true;

                //Create backbuffer renderpass
                RenderPassDesc renderPassDesc = {};
                renderPassDesc.NumRenderTargets                  = 1;
                renderPassDesc.RenderTargets[0].Format           = pDevice->GetBackBufferFormat();
                renderPassDesc.RenderTargets[0].Flags            = RENDER_PASS_ATTACHMENT_FLAG_RESOLVE;
                renderPassDesc.RenderTargets[0].LoadOperation    = LOAD_OP_CLEAR;
                renderPassDesc.RenderTargets[0].StoreOperation   = STORE_OP_STORE;
                renderPassDesc.RenderTargets[0].FinalState       = RESOURCE_STATE_RENDERTARGET_PRESENT;
                renderPassDesc.DepthStencil.Format               = FORMAT_D24_UNORM_S8_UINT;
                renderPassDesc.DepthStencil.Flags                = 0;
                renderPassDesc.DepthStencil.LoadOperation        = LOAD_OP_CLEAR;
                renderPassDesc.DepthStencil.StoreOperation       = STORE_OP_UNKNOWN;
                renderPassDesc.DepthStencil.FinalState           = RESOURCE_STATE_DEPTH_STENCIL;
                
                GraphicsPipelineStateDesc graphicsPipeline = {};
                graphicsPipeline.pVertexShader      = m_VS.Get();
                graphicsPipeline.pPixelShader       = m_PS.Get();
				graphicsPipeline.Topology           = PRIMITIVE_TOPOLOGY_TRIANGLELIST;
				graphicsPipeline.VertexInput		= vertexInput;
				graphicsPipeline.RasterizerState	= rasterizerState;
				graphicsPipeline.RenderPass		    = renderPassDesc;
				graphicsPipeline.BlendState			= blendState;
				graphicsPipeline.DepthStencilState	= depthStencilState;

				ShaderVariableDesc shaderVariables[] =
				{
					{ "u_CameraBuffer",		nullptr, RESOURCE_TYPE_CONSTANT_BUFFER,	SHADER_STAGE_VERTEX,	RESOURCE_USAGE_DEFAULT, 0 },
					{ "u_TransformBuffer",	nullptr, RESOURCE_TYPE_CONSTANT_BUFFER,	SHADER_STAGE_VERTEX,	RESOURCE_USAGE_DYNAMIC, 1 },
					{ "u_MaterialBuffer",	nullptr, RESOURCE_TYPE_CONSTANT_BUFFER,	SHADER_STAGE_PIXEL,		RESOURCE_USAGE_DYNAMIC, 2 },
					{ "u_LightBuffer",		nullptr, RESOURCE_TYPE_CONSTANT_BUFFER,	SHADER_STAGE_PIXEL,		RESOURCE_USAGE_DEFAULT, 3 },
					{ "u_Albedo",			nullptr, RESOURCE_TYPE_TEXTURE,			SHADER_STAGE_PIXEL,		RESOURCE_USAGE_DEFAULT, 4 },
					{ "u_Normal",			nullptr, RESOURCE_TYPE_TEXTURE,			SHADER_STAGE_PIXEL,		RESOURCE_USAGE_DEFAULT, 5 },
					{ "u_Sampler",			nullptr, RESOURCE_TYPE_SAMPLER_STATE,	SHADER_STAGE_PIXEL,		RESOURCE_USAGE_DEFAULT, 6 },
				};

				ShaderVariableTableDesc variableTableDesc = {};
				variableTableDesc.NumVariables	= 7;
				variableTableDesc.pVariables	= shaderVariables;
				variableTableDesc.NumConstantBlocks	= 0;
				variableTableDesc.pConstantBlocks	= nullptr;

				PipelineStateDesc pipelineDesc = {};
				pipelineDesc.pName	= "MainPipelineState";
				pipelineDesc.Type	= PIPELINE_TYPE_GRAPHICS;
				pipelineDesc.ShaderVariableTable	= variableTableDesc;
				pipelineDesc.GraphicsPipeline		= graphicsPipeline;
                pDevice->CreatePipelineState(&m_PipelineState, pipelineDesc);
				m_PipelineState->CreateShaderVariableTable(&m_VariableTable);
            }

            //Create vertexbuffer
			MeshData mesh = MeshFactory::CreateFromFile("revolver.obj");
			m_Mesh.IndexCount = uint32(mesh.Indices.size());
			{
                BufferDesc desc     = {};
                desc.pName          = "Mesh VertexBuffer";
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
                desc.pName          = "Mesh IndexBuffer";
                desc.Usage          = RESOURCE_USAGE_DEFAULT;
                desc.Flags          = BUFFER_FLAGS_INDEX_BUFFER;
                desc.SizeInBytes    = sizeof(uint32) * uint32(mesh.Indices.size());
                desc.StrideInBytes  = sizeof(uint32);
                
                ResourceData data   = {};
                data.pData          = mesh.Indices.data();
                data.SizeInBytes    = desc.SizeInBytes;
                
                pDevice->CreateBuffer(&m_Mesh.pIndexBuffer, &data, desc);
            }

			//Create vertexbuffer
			MeshData mesh2 = MeshFactory::CreateSphere(4, 0.45);
			m_SphereMesh.IndexCount = uint32(mesh2.Indices.size());
			{
				BufferDesc desc = {};
				desc.pName			= "Sphere VertexBuffer";
				desc.Usage			= RESOURCE_USAGE_DEFAULT;
				desc.Flags			= BUFFER_FLAGS_VERTEX_BUFFER;
				desc.SizeInBytes	= sizeof(Vertex) * uint32(mesh2.Vertices.size());
				desc.StrideInBytes	= sizeof(Vertex);

				ResourceData data = {};
				data.pData			= mesh2.Vertices.data();
				data.SizeInBytes	= desc.SizeInBytes;

				pDevice->CreateBuffer(&m_SphereMesh.pVertexBuffer, &data, desc);
			}
			//Create indexbuffer
			{
				BufferDesc desc = {};
				desc.pName			= "Sphere IndexBuffer";
				desc.Usage			= RESOURCE_USAGE_DEFAULT;
				desc.Flags			= BUFFER_FLAGS_INDEX_BUFFER;
				desc.SizeInBytes	= sizeof(uint32) * uint32(mesh2.Indices.size());
				desc.StrideInBytes	= sizeof(uint32);

				ResourceData data = {};
				data.pData			= mesh2.Indices.data();
				data.SizeInBytes	= desc.SizeInBytes;

				pDevice->CreateBuffer(&m_SphereMesh.pIndexBuffer, &data, desc);
			}

            //Init transforms
            m_TransformBuffer.Model = glm::mat4(1.0f);

            //Create texture
            m_AlbedoMap = ITexture::CreateTextureFromFile(pDevice, "revolver_albedo.png", TEXTURE_FLAGS_SHADER_RESOURCE | TEXTURE_FLAGS_GENEATE_MIPS, RESOURCE_USAGE_DEFAULT, FORMAT_R8G8B8A8_UNORM);
            m_NormalMap = ITexture::CreateTextureFromFile(pDevice, "revolver_normal.png", TEXTURE_FLAGS_SHADER_RESOURCE | TEXTURE_FLAGS_GENEATE_MIPS, RESOURCE_USAGE_DEFAULT, FORMAT_R8G8B8A8_UNORM);
			context->TransitionTexture(m_AlbedoMap.Get(), RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0, LAMBDA_TRANSITION_ALL_MIPS);
			context->TransitionTexture(m_NormalMap.Get(), RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0, LAMBDA_TRANSITION_ALL_MIPS);

            //Create samplerstate
            TextureDesc textureDesc = m_AlbedoMap->GetDesc();
            SamplerStateDesc desc = {};
            desc.AdressMode = SAMPLER_ADDRESS_MODE_REPEAT;
			desc.MinMipLOD	= 0.0f;
			desc.MaxMipLOD	= float(textureDesc.MipLevels);
			desc.MipLODBias	= 0.0f;
			desc.Anisotropy = 16.0f;
            pDevice->CreateSamplerState(&m_SamplerState, desc);

            //Close and execute commandlist
			context->End();
            context->Flush();

			//Setup materials
			m_VariableTable->GetVariableByName(SHADER_STAGE_PIXEL, "u_Albedo")->SetTexture(m_AlbedoMap.Get());
			m_VariableTable->GetVariableByName(SHADER_STAGE_PIXEL, "u_Normal")->SetTexture(m_NormalMap.Get());
			m_VariableTable->GetVariableByName(SHADER_STAGE_PIXEL, "u_Sampler")->SetSamplerState(m_SamplerState.Get());

			m_Material.pPipelineState	= m_PipelineState.Get();
			m_Material.pVariableTable	= m_VariableTable.Get();
			m_Material.HasNormalMap		= 1;
			m_Material.HasAlbedoMap		= 1;
			m_Material.Color			= glm::vec4(RGB_F(255, 255, 255), 1.0f);

			m_RedMaterial.pVariableTable	= m_VariableTable.Get();
			m_RedMaterial.pPipelineState	= m_PipelineState.Get();
			m_RedMaterial.HasNormalMap		= 0;
			m_RedMaterial.HasAlbedoMap		= 0;
			m_RedMaterial.Color				= glm::vec4(RGB_F(255, 0, 0), 1.0f);
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


	void SandBoxLayer::OnRender(Renderer3D& renderer, Timestep dt)
	{
        //Update light
        static LightBuffer lightBuffer  =
        {
            glm::vec4(lightColor[0], lightColor[1], lightColor[2], lightColor[3]),
            glm::vec3(transformLight.Position[0], transformLight.Position[1], transformLight.Position[2])
        };
        
        lightBuffer.Color       = glm::vec4(lightColor[0], lightColor[1], lightColor[2], lightColor[3]);
        lightBuffer.Position    = glm::vec3(transformLight.Position[0], transformLight.Position[1], transformLight.Position[2]);

		renderer.BeginScene(m_Camera, lightBuffer);
       
		//draw revolver
        glm::mat4 translation	= glm::translate(glm::mat4(1.0f), transformRevolver.Position);
        glm::mat4 rotation		= glm::eulerAngleYXZ(glm::radians(transformRevolver.Rotation.y), glm::radians(transformRevolver.Rotation.x), glm::radians(transformRevolver.Rotation.z));
        glm::mat4 scale			= glm::scale(glm::mat4(1.0f), transformRevolver.Scale);        
		m_TransformBuffer.Model = translation * rotation * scale;
		renderer.Submit(m_Mesh, m_Material, m_TransformBuffer);
		
		//draw spheres
		for (auto& transform : sphereTransforms)
		{
			translation = glm::translate(glm::mat4(1.0f), transform.Position);
			rotation	= glm::eulerAngleYXZ(glm::radians(transform.Rotation.y), glm::radians(transform.Rotation.x), glm::radians(transform.Rotation.z));
			scale		= glm::scale(glm::mat4(1.0f), transform.Scale);
			m_TransformBuffer.Model = translation * rotation * scale;

			renderer.Submit(m_SphereMesh, m_RedMaterial, m_TransformBuffer);
		}

		renderer.EndScene();
	}


	void SandBoxLayer::OnRelease()
	{
		IDevice* pDevice = IDevice::Get();
        if (pDevice)
        {
            pDevice->WaitForGPU();

			m_VS.Release();
			m_PS.Release();
			m_PipelineState.Release();
			m_VariableTable.Release();
			m_Mesh.pVertexBuffer->Release();
			m_Mesh.pIndexBuffer->Release();
			m_SphereMesh.pVertexBuffer->Release();
			m_SphereMesh.pIndexBuffer->Release();
			m_AlbedoMap.Release();
			m_NormalMap.Release();
			m_SamplerState.Release();
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
						ShowTransform(0, glm::value_ptr(transformLight.Position), glm::value_ptr(transformLight.Rotation), glm::value_ptr(transformLight.Scale));
						ShowLight(1, glm::value_ptr(lightColor));
					}
					else if (uid == 1)
					{
						ShowTransform(0, glm::value_ptr(transformRevolver.Position), glm::value_ptr(transformRevolver.Rotation), glm::value_ptr(transformRevolver.Scale));
					}
					else if (uid == 2)
					{
						ShowTransform(0, glm::value_ptr(transformRevolver2.Position), glm::value_ptr(transformRevolver2.Rotation), glm::value_ptr(transformRevolver2.Scale));
					}
					else if (uid > 2)
					{
						auto& transform = sphereTransforms[uid - 3];
						ShowTransform(0, glm::value_ptr(transform.Position), glm::value_ptr(transform.Rotation), glm::value_ptr(transform.Scale));
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
		};

		funcs::ShowObject("PointLight", 0);
		funcs::ShowObject("Entity", 1);

		//draw spheres
		uint32 index = 3;
		for (auto& transform : sphereTransforms)
		{
			funcs::ShowObject("Entity", index++);
		}

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
