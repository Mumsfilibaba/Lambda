#include "LambdaPch.h"
#include "Graphics/UILayer.h"
#include "Graphics/IShader.h"
#include "Graphics/ISamplerState.h"
#include "Graphics/IPipelineResourceState.h"
#include "Graphics/IPipelineState.h"
#include "Graphics/IRenderPass.h"
#include "Graphics/ITexture.h"
#include "Graphics/IBuffer.h"
#include "System/Application.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"

namespace Lambda
{
	//--------
	// SHADERS
	//--------

	// glsl_shader.vert, compiled with:
	// # glslangValidator -V -x -o glsl_shader.vert.u32 glsl_shader.vert
	/*
	#version 450 core
	layout(location = 0) in vec2 aPos;
	layout(location = 1) in vec2 aUV;
	layout(location = 2) in vec4 aColor;
	layout(push_constant) uniform uPushConstant { vec2 uScale; vec2 uTranslate; } pc;
	out gl_PerVertex { vec4 gl_Position; };
	layout(location = 0) out struct { vec4 Color; vec2 UV; } Out;
	void main()
	{
		Out.Color = aColor;
		Out.UV = aUV;
		gl_Position = vec4(aPos * pc.uScale + pc.uTranslate, 0, 1);
	}
	*/
	static uint32_t __glsl_shader_vert_spv[] =
	{
		0x07230203,0x00010000,0x00080001,0x0000002e,0x00000000,0x00020011,0x00000001,0x0006000b,
		0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
		0x000a000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000b,0x0000000f,0x00000015,
		0x0000001b,0x0000001c,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
		0x00000000,0x00030005,0x00000009,0x00000000,0x00050006,0x00000009,0x00000000,0x6f6c6f43,
		0x00000072,0x00040006,0x00000009,0x00000001,0x00005655,0x00030005,0x0000000b,0x0074754f,
		0x00040005,0x0000000f,0x6c6f4361,0x0000726f,0x00030005,0x00000015,0x00565561,0x00060005,
		0x00000019,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x00000019,0x00000000,
		0x505f6c67,0x7469736f,0x006e6f69,0x00030005,0x0000001b,0x00000000,0x00040005,0x0000001c,
		0x736f5061,0x00000000,0x00060005,0x0000001e,0x73755075,0x6e6f4368,0x6e617473,0x00000074,
		0x00050006,0x0000001e,0x00000000,0x61635375,0x0000656c,0x00060006,0x0000001e,0x00000001,
		0x61725475,0x616c736e,0x00006574,0x00030005,0x00000020,0x00006370,0x00040047,0x0000000b,
		0x0000001e,0x00000000,0x00040047,0x0000000f,0x0000001e,0x00000002,0x00040047,0x00000015,
		0x0000001e,0x00000001,0x00050048,0x00000019,0x00000000,0x0000000b,0x00000000,0x00030047,
		0x00000019,0x00000002,0x00040047,0x0000001c,0x0000001e,0x00000000,0x00050048,0x0000001e,
		0x00000000,0x00000023,0x00000000,0x00050048,0x0000001e,0x00000001,0x00000023,0x00000008,
		0x00030047,0x0000001e,0x00000002,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
		0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040017,
		0x00000008,0x00000006,0x00000002,0x0004001e,0x00000009,0x00000007,0x00000008,0x00040020,
		0x0000000a,0x00000003,0x00000009,0x0004003b,0x0000000a,0x0000000b,0x00000003,0x00040015,
		0x0000000c,0x00000020,0x00000001,0x0004002b,0x0000000c,0x0000000d,0x00000000,0x00040020,
		0x0000000e,0x00000001,0x00000007,0x0004003b,0x0000000e,0x0000000f,0x00000001,0x00040020,
		0x00000011,0x00000003,0x00000007,0x0004002b,0x0000000c,0x00000013,0x00000001,0x00040020,
		0x00000014,0x00000001,0x00000008,0x0004003b,0x00000014,0x00000015,0x00000001,0x00040020,
		0x00000017,0x00000003,0x00000008,0x0003001e,0x00000019,0x00000007,0x00040020,0x0000001a,
		0x00000003,0x00000019,0x0004003b,0x0000001a,0x0000001b,0x00000003,0x0004003b,0x00000014,
		0x0000001c,0x00000001,0x0004001e,0x0000001e,0x00000008,0x00000008,0x00040020,0x0000001f,
		0x00000009,0x0000001e,0x0004003b,0x0000001f,0x00000020,0x00000009,0x00040020,0x00000021,
		0x00000009,0x00000008,0x0004002b,0x00000006,0x00000028,0x00000000,0x0004002b,0x00000006,
		0x00000029,0x3f800000,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
		0x00000005,0x0004003d,0x00000007,0x00000010,0x0000000f,0x00050041,0x00000011,0x00000012,
		0x0000000b,0x0000000d,0x0003003e,0x00000012,0x00000010,0x0004003d,0x00000008,0x00000016,
		0x00000015,0x00050041,0x00000017,0x00000018,0x0000000b,0x00000013,0x0003003e,0x00000018,
		0x00000016,0x0004003d,0x00000008,0x0000001d,0x0000001c,0x00050041,0x00000021,0x00000022,
		0x00000020,0x0000000d,0x0004003d,0x00000008,0x00000023,0x00000022,0x00050085,0x00000008,
		0x00000024,0x0000001d,0x00000023,0x00050041,0x00000021,0x00000025,0x00000020,0x00000013,
		0x0004003d,0x00000008,0x00000026,0x00000025,0x00050081,0x00000008,0x00000027,0x00000024,
		0x00000026,0x00050051,0x00000006,0x0000002a,0x00000027,0x00000000,0x00050051,0x00000006,
		0x0000002b,0x00000027,0x00000001,0x00070050,0x00000007,0x0000002c,0x0000002a,0x0000002b,
		0x00000028,0x00000029,0x00050041,0x00000011,0x0000002d,0x0000001b,0x0000000d,0x0003003e,
		0x0000002d,0x0000002c,0x000100fd,0x00010038
	};

	// glsl_shader.frag, compiled with:
	// # glslangValidator -V -x -o glsl_shader.frag.u32 glsl_shader.frag
	/*
	#version 450 core
	layout(location = 0) out vec4 fColor;
	layout(set=0, binding=0) uniform sampler2D sTexture;
	layout(location = 0) in struct { vec4 Color; vec2 UV; } In;
	void main()
	{
		fColor = In.Color * texture(sTexture, In.UV.st);
	}
	*/
	static uint32_t __glsl_shader_frag_spv[] =
	{
		0x07230203,0x00010000,0x00080001,0x0000001e,0x00000000,0x00020011,0x00000001,0x0006000b,
		0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
		0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000d,0x00030010,
		0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
		0x00000000,0x00040005,0x00000009,0x6c6f4366,0x0000726f,0x00030005,0x0000000b,0x00000000,
		0x00050006,0x0000000b,0x00000000,0x6f6c6f43,0x00000072,0x00040006,0x0000000b,0x00000001,
		0x00005655,0x00030005,0x0000000d,0x00006e49,0x00050005,0x00000016,0x78655473,0x65727574,
		0x00000000,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000d,0x0000001e,
		0x00000000,0x00040047,0x00000016,0x00000022,0x00000000,0x00040047,0x00000016,0x00000021,
		0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
		0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,
		0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040017,0x0000000a,0x00000006,
		0x00000002,0x0004001e,0x0000000b,0x00000007,0x0000000a,0x00040020,0x0000000c,0x00000001,
		0x0000000b,0x0004003b,0x0000000c,0x0000000d,0x00000001,0x00040015,0x0000000e,0x00000020,
		0x00000001,0x0004002b,0x0000000e,0x0000000f,0x00000000,0x00040020,0x00000010,0x00000001,
		0x00000007,0x00090019,0x00000013,0x00000006,0x00000001,0x00000000,0x00000000,0x00000000,
		0x00000001,0x00000000,0x0003001b,0x00000014,0x00000013,0x00040020,0x00000015,0x00000000,
		0x00000014,0x0004003b,0x00000015,0x00000016,0x00000000,0x0004002b,0x0000000e,0x00000018,
		0x00000001,0x00040020,0x00000019,0x00000001,0x0000000a,0x00050036,0x00000002,0x00000004,
		0x00000000,0x00000003,0x000200f8,0x00000005,0x00050041,0x00000010,0x00000011,0x0000000d,
		0x0000000f,0x0004003d,0x00000007,0x00000012,0x00000011,0x0004003d,0x00000014,0x00000017,
		0x00000016,0x00050041,0x00000019,0x0000001a,0x0000000d,0x00000018,0x0004003d,0x0000000a,
		0x0000001b,0x0000001a,0x00050057,0x00000007,0x0000001c,0x00000017,0x0000001b,0x00050085,
		0x00000007,0x0000001d,0x00000012,0x0000001c,0x0003003e,0x00000009,0x0000001d,0x000100fd,
		0x00010038
	};


	//-------
	//UILayer
	//-------

    UILayer::UILayer()
        : Layer("UILayer"),
		m_pVS(nullptr),
		m_pPS(nullptr),
		m_pSamplerState(nullptr),
		m_pPipelineResourceState(nullptr),
		m_pPipelineState(nullptr),
		m_pFontTexture(nullptr),
		m_pVertexBuffer(nullptr),
		m_pIndexBuffer(nullptr)
    {
        Create();
    }


    void UILayer::Create()
    {
        //Create context for ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        IWindow* pWindow = Application::Get().GetWindow();

        ImGuiIO& io = ImGui::GetIO();
        io.BackendPlatformName = "Lambda Engine";
        io.DisplaySize = ImVec2(float(pWindow->GetWidth()), float(pWindow->GetHeight()));

        // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
        io.KeyMap[ImGuiKey_Tab]          = KEY_TAB;
        io.KeyMap[ImGuiKey_LeftArrow]    = KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow]   = KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow]      = KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow]    = KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp]       = KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown]     = KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home]         = KEY_HOME;
        io.KeyMap[ImGuiKey_End]          = KEY_END;
        io.KeyMap[ImGuiKey_Insert]       = KEY_INSERT;
        io.KeyMap[ImGuiKey_Delete]       = KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace]    = KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Space]        = KEY_SPACE;
        io.KeyMap[ImGuiKey_Enter]        = KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape]       = KEY_ESCAPE;
        io.KeyMap[ImGuiKey_KeyPadEnter]  = KEY_KEYPAD_ENTER;
        io.KeyMap[ImGuiKey_A]            = KEY_A;
        io.KeyMap[ImGuiKey_C]            = KEY_C;
        io.KeyMap[ImGuiKey_V]            = KEY_V;
        io.KeyMap[ImGuiKey_X]            = KEY_X;
        io.KeyMap[ImGuiKey_Y]            = KEY_Y;
        io.KeyMap[ImGuiKey_Z]            = KEY_Z;

#if defined(LAMBDA_PLAT_WINDOWS)
        io.ImeWindowHandle = reinterpret_cast<HWND>(pWindow->GetNativeHandle());
#endif
                
        ImGui::StyleColorsDark();
        ImGui::GetStyle().WindowRounding    = 0.0f;
        ImGui::GetStyle().ChildRounding     = 0.0f;
        ImGui::GetStyle().FrameRounding     = 0.0f;
        ImGui::GetStyle().GrabRounding      = 0.0f;
        ImGui::GetStyle().PopupRounding     = 0.0f;
        ImGui::GetStyle().ScrollbarRounding = 0.0f;
    }


	void UILayer::Init(IRenderPass* pRenderPass, ICommandList* pList)
	{
		//Create graphics objects
		IGraphicsDevice* pDevice = IGraphicsDevice::Get();

		//Create shaders
		ShaderDesc shaderDesc = {};
#if defined(LAMBDA_DEBUG)
		shaderDesc.Flags = SHADER_FLAG_COMPILE_DEBUG;
#else
		shaderDesc.Flags = 0;
#endif
		shaderDesc.pEntryPoint = "main";
		shaderDesc.Type = SHADER_STAGE_VERTEX;

		GraphicsDeviceDesc deviceDesc = pDevice->GetDesc();
		if (deviceDesc.Api == GRAPHICS_API_VULKAN)
		{
			shaderDesc.pSource		= reinterpret_cast<const char*>(__glsl_shader_vert_spv);
			shaderDesc.SourceLength = sizeof(__glsl_shader_vert_spv);
			shaderDesc.Languange	= SHADER_LANG_SPIRV;
		}

		pDevice->CreateShader(&m_pVS, shaderDesc);

		shaderDesc.Type = SHADER_STAGE_PIXEL;
		if (deviceDesc.Api == GRAPHICS_API_VULKAN)
		{
			shaderDesc.pSource		= reinterpret_cast<const char*>(__glsl_shader_frag_spv);
			shaderDesc.SourceLength = sizeof(__glsl_shader_frag_spv);
		}

		pDevice->CreateShader(&m_pPS, shaderDesc);


		//Create sampler
		SamplerStateDesc samplerDesc = {};
		samplerDesc.AdressMode	= SAMPLER_ADDRESS_MODE_REPEAT;
		samplerDesc.Anisotropy	= 1.0f;
		samplerDesc.MaxMipLOD	= 1000.0f;
		samplerDesc.MinMipLOD	= -1000.0f;
		samplerDesc.MipLODBias	= 0.0f;
		pDevice->CreateSamplerState(&m_pSamplerState, samplerDesc);


		//Create pipelineresourcestate
		ResourceSlot resourceSlots[1];
		resourceSlots[0].pSamplerState = m_pSamplerState;
		resourceSlots[0].Slot	= 0;
		resourceSlots[0].Type	= RESOURCE_TYPE_TEXTURE;
		resourceSlots[0].Usage	= RESOURCE_USAGE_DEFAULT;
		resourceSlots[0].Stage	= SHADER_STAGE_PIXEL;

        ConstantBlock constantBlocks[1];
		constantBlocks[0].Stage			= SHADER_STAGE_VERTEX;
		constantBlocks[0].SizeInBytes	= sizeof(float) * 4;

		PipelineResourceStateDesc resourceStateDesc = {};
		resourceStateDesc.NumResourceSlots	= 1;
		resourceStateDesc.pResourceSlots	= resourceSlots;
		resourceStateDesc.NumConstantBlocks	= 1;
        resourceStateDesc.pConstantBlocks	= constantBlocks;
		pDevice->CreatePipelineResourceState(&m_pPipelineResourceState, resourceStateDesc);


		//Create pipelinestate
		InputElement inputElements[3] =
		{
			{ "POSITION",   FORMAT_R32G32_FLOAT,	0, 0, sizeof(ImDrawVert), offsetof(ImDrawVert, pos),	false },
			{ "TEXCOORD",   FORMAT_R32G32_FLOAT,	0, 1, sizeof(ImDrawVert), offsetof(ImDrawVert, uv),		false },
			{ "COLOR",		FORMAT_R8G8B8A8_UNORM,	0, 2, sizeof(ImDrawVert), offsetof(ImDrawVert, col),    false },
		};

		GraphicsPipelineStateDesc pipelineStateDesc = {};
		pipelineStateDesc.pName						= "ImGui-PipelineState";
		pipelineStateDesc.pVertexShader				= m_pVS;
		pipelineStateDesc.pPixelShader				= m_pPS;
		pipelineStateDesc.pRenderPass				= pRenderPass;
		pipelineStateDesc.pResourceState			= m_pPipelineResourceState;
		pipelineStateDesc.InputElementCount			= 3;
		pipelineStateDesc.pInputElements			= inputElements;
		pipelineStateDesc.Topology					= PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		pipelineStateDesc.Cull						= CULL_MODE_NONE;
		pipelineStateDesc.FillMode					= POLYGON_MODE_FILL;
		pipelineStateDesc.FrontFaceCounterClockWise = true;
		pipelineStateDesc.DepthTest					= false;
		pipelineStateDesc.EnableBlending			= true;
		pDevice->CreateGraphicsPipelineState(&m_pPipelineState, pipelineStateDesc);


		//Create fonttexture
		uint8* pPixels	= nullptr;
		int32	width	= 0;
		int32	height	= 0;

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->GetTexDataAsRGBA32(&pPixels, &width, &height);
		uint64 uploadSize = width * height * 4 * sizeof(char);

		TextureDesc fontTextureDesc = {};
		fontTextureDesc.Type				= TEXTURE_TYPE_2D;
		fontTextureDesc.Flags				= TEXTURE_FLAGS_SHADER_RESOURCE;
		fontTextureDesc.Format				= FORMAT_R8G8B8A8_UNORM;
		fontTextureDesc.Width				= width;
		fontTextureDesc.Height				= height;
		fontTextureDesc.Depth				= 1;
		fontTextureDesc.MipLevels			= 1;
		fontTextureDesc.ArraySize			= 1;
		fontTextureDesc.SampleCount			= 1;
		fontTextureDesc.pResolveResource	= nullptr;
		fontTextureDesc.Usage				= RESOURCE_USAGE_DEFAULT;

		ResourceData initalData = {};
		initalData.pData		= pPixels;
		initalData.SizeInBytes	= uploadSize;
		pDevice->CreateTexture(&m_pFontTexture, &initalData, fontTextureDesc);
		pList->TransitionTexture(m_pFontTexture, RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0, LAMBDA_TRANSITION_ALL_MIPS);

		// Store our identifier
		io.Fonts->TexID = (ImTextureID)(intptr_t)m_pFontTexture->GetNativeHandle();


		//Create vertex and indexbuffer
		BufferDesc bufferDesc = {};
		bufferDesc.pName			= "ImGui VertexBuffer";
		bufferDesc.Flags			= BUFFER_FLAGS_VERTEX_BUFFER;
		bufferDesc.SizeInBytes		= MB(2);
		bufferDesc.StrideInBytes	= sizeof(ImDrawVert);
		bufferDesc.Usage			= RESOURCE_USAGE_DYNAMIC;
		pDevice->CreateBuffer(&m_pVertexBuffer, nullptr, bufferDesc);

		bufferDesc.pName			= "ImGui IndexBuffer";
		bufferDesc.Flags			= BUFFER_FLAGS_INDEX_BUFFER;
		bufferDesc.SizeInBytes		= MB(2);
		bufferDesc.StrideInBytes	= sizeof(ImDrawIdx);
		bufferDesc.Usage			= RESOURCE_USAGE_DYNAMIC;
		pDevice->CreateBuffer(&m_pIndexBuffer, nullptr, bufferDesc);
	}


	void UILayer::Begin(Timestep time)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = time.AsSeconds();

		ImGui::NewFrame();
	}

	
	void UILayer::OnRenderUI(Timestep dt)
	{
        //ImGui::ShowDemoWindow();
        
        const float DISTANCE = 10.0f;
        static int corner = 0;
        ImGuiIO& io = ImGui::GetIO();
        if (corner != -1)
        {
            ImVec2 window_pos		= ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
            ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        }
        
        ImGui::SetNextWindowBgAlpha(0.75f); // Transparent background
        if (ImGui::Begin("Timings", NULL, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
            ImGui::Text("Settings:");
            ImGui::Separator();
            
            IGraphicsDevice* pDevice = IGraphicsDevice::Get();
            GraphicsDeviceDesc desc = pDevice->GetDesc();
            
            if (desc.Api == GRAPHICS_API_VULKAN)
                ImGui::Text("Renderer: Vulkan");
            else if (desc.Api == GRAPHICS_API_D3D12)
                ImGui::Text("Renderer: D3D12");
            
            ImGui::Text("Resolution: %u x %u", pDevice->GetSwapChainWidth(), pDevice->GetSwapChainHeight());
            ImGui::Text("MSAA: %ux", desc.SampleCount);
            ImGui::Text("BackBufferCount: %u", desc.BackBufferCount);
            
            ImGui::Separator();
            ImGui::Text("Timings:");
            ImGui::Separator();
            
            static float timer = 0.0f;
            static int32 fps = 0;
            static int32 currentFPS = 0;
            
            float ms = io.DeltaTime * 1000.0f;
            timer += ms;
            
            fps++;
            if (timer >= 1000.0f)
            {
                timer = 0.0f;
                currentFPS = fps;
                fps = 0;
            }
            
            ImGui::Text("FPS: %d", currentFPS);
            ImGui::Text("CPU Frametime: %.2fms", ms);
            
            static float values[90]     = { 0 };
            static int   values_offset  = 0;
            values[values_offset] = ms;
            values_offset = (values_offset+1) % IM_ARRAYSIZE(values);
            {
                float average = 0.0f;
                for (int n = 0; n < IM_ARRAYSIZE(values); n++)
                    average += values[n];
                average /= (float)IM_ARRAYSIZE(values);
                char overlay[32];
                sprintf(overlay, "avg %f", average);
                ImGui::PlotLines("", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0,80));
            }
            
            ImGui::PopStyleColor();
        }
        ImGui::End();
	}

	
	void UILayer::End()
	{
		ImGui::EndFrame();
		ImGui::Render();
	}


	void UILayer::Draw(ICommandList* pList)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImDrawData* pDrawData = ImGui::GetDrawData();

		uint64 vertexSize	= pDrawData->TotalVtxCount * sizeof(ImDrawVert);
		uint64 indexSize	= pDrawData->TotalIdxCount * sizeof(ImDrawIdx);
		// Upload vertex/index data into a single contiguous GPU buffer
		{
			ImDrawVert* vtxDst  = NULL;
			ImDrawIdx* idxDst   = NULL;
			m_pVertexBuffer->Map(reinterpret_cast<void**>(&vtxDst));
			m_pIndexBuffer->Map(reinterpret_cast<void**>(&idxDst));

			for (int n = 0; n < pDrawData->CmdListsCount; n++)
			{
				const ImDrawList* pCmdList = pDrawData->CmdLists[n];
				memcpy(vtxDst, pCmdList->VtxBuffer.Data, pCmdList->VtxBuffer.Size * sizeof(ImDrawVert));
				memcpy(idxDst, pCmdList->IdxBuffer.Data, pCmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
				vtxDst += pCmdList->VtxBuffer.Size;
				idxDst += pCmdList->IdxBuffer.Size;
			}

			m_pVertexBuffer->Unmap();
			m_pIndexBuffer->Unmap();
		}

		//Setup pipelinestate
		m_pPipelineResourceState->SetTextures(&m_pFontTexture, 1, 0);
		pList->SetGraphicsPipelineResourceState(m_pPipelineResourceState);
		pList->SetGraphicsPipelineState(m_pPipelineState);
		
		//Setup vertex and indexbuffer
		pList->SetVertexBuffer(m_pVertexBuffer, 0);
		pList->SetIndexBuffer(m_pIndexBuffer, sizeof(ImDrawIdx) == 2 ? FORMAT_R16_UINT : FORMAT_R32_UINT);
		
		//Setup viewport
		Viewport viewport = {};
		viewport.TopX		= 0.0f;
		viewport.TopY		= 0.0f;
		viewport.Width		= io.DisplaySize.x;
		viewport.Height		= io.DisplaySize.y;
		viewport.MinDepth	= 0.0f;
		viewport.MaxDepth	= 1.0f;
		pList->SetViewport(viewport);

		// Setup scale and translation:
		// Our visible imgui space lies from draw_data->DisplayPps (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
		{
			float scale[2];
			scale[0] = 2.0f / pDrawData->DisplaySize.x;
			scale[1] = -2.0f / pDrawData->DisplaySize.y;
			float translate[2];
			translate[0] = -1.0f + pDrawData->DisplayPos.x * scale[0];
			translate[1] = 1.0f - pDrawData->DisplayPos.y * scale[1];

			pList->SetConstantBlocks(SHADER_STAGE_VERTEX, sizeof(float) * 0, sizeof(float) * 2, scale);
			pList->SetConstantBlocks(SHADER_STAGE_VERTEX, sizeof(float) * 2, sizeof(float) * 2, translate);
		}

		// Will project scissor/clipping rectangles into framebuffer space
		ImVec2 clipOff		= pDrawData->DisplayPos;       // (0,0) unless using multi-viewports
		ImVec2 clipScale	= pDrawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

		// Render command lists
		// (Because we merged all buffers into a single one, we maintain our own offset into them)
		int32 globalVtxOffset = 0;
		int32 globalIdxOffset = 0;
		for (int32 n = 0; n < pDrawData->CmdListsCount; n++)
		{
			const ImDrawList* pCmdList = pDrawData->CmdLists[n];
			for (int32 i = 0; i < pCmdList->CmdBuffer.Size; i++)
			{
				const ImDrawCmd* pCmd = &pCmdList->CmdBuffer[i];
				// Project scissor/clipping rectangles into framebuffer space
				ImVec4 clipRect;
				clipRect.x = (pCmd->ClipRect.x - clipOff.x) * clipScale.x;
				clipRect.y = (pCmd->ClipRect.y - clipOff.y) * clipScale.y;
				clipRect.z = (pCmd->ClipRect.z - clipOff.x) * clipScale.x;
				clipRect.w = (pCmd->ClipRect.w - clipOff.y) * clipScale.y;

				if (clipRect.x < viewport.Width && clipRect.y < viewport.Height && clipRect.z >= 0.0f && clipRect.w >= 0.0f)
				{
					// Negative offsets are illegal for vkCmdSetScissor
					if (clipRect.x < 0.0f)
						clipRect.x = 0.0f;
					if (clipRect.y < 0.0f)
						clipRect.y = 0.0f;

					// Apply scissor/clipping rectangle	
					Rectangle scissor = {};
					scissor.X		= clipRect.x;
					scissor.Y		= clipRect.y;
					scissor.Width	= clipRect.z - clipRect.x;
					scissor.Height	= clipRect.w - clipRect.y;
					pList->SetScissorRect(scissor);

					// Draw
					pList->DrawIndexedInstanced(pCmd->ElemCount, 1, pCmd->IdxOffset + globalIdxOffset, pCmd->VtxOffset + globalVtxOffset, 0);
				}
			}
			globalIdxOffset += pCmdList->IdxBuffer.Size;
			globalVtxOffset += pCmdList->VtxBuffer.Size;
		}
	}


	void UILayer::OnRelease()
    {
		//Destroy ImGui context
		ImGui::DestroyContext();

		//Destroy all objects
		IGraphicsDevice* pDevice = IGraphicsDevice::Get();
		if (pDevice != nullptr)
		{
			pDevice->DestroyShader(&m_pVS);
			pDevice->DestroyShader(&m_pPS);
			pDevice->DestroyResourceState(&m_pPipelineResourceState);
			pDevice->DestroyGraphicsPipelineState(&m_pPipelineState);
			pDevice->DestroySamplerState(&m_pSamplerState);
			pDevice->DestroyTexture(&m_pFontTexture);
			pDevice->DestroyBuffer(&m_pVertexBuffer);
			pDevice->DestroyBuffer(&m_pIndexBuffer);
		}
    }


    void UILayer::OnLoad()
    {
    }


    bool UILayer::OnEvent(const Event& event)
    {
        EventForwarder forwarder;
		forwarder.ForwardEvent(this, &UILayer::OnKeyTyped, event);
		forwarder.ForwardEvent(this, &UILayer::OnKeyPressed, event);
        forwarder.ForwardEvent(this, &UILayer::OnKeyReleased, event);
		forwarder.ForwardEvent(this, &UILayer::OnMouseScroll, event);
		forwarder.ForwardEvent(this, &UILayer::OnMousePressed, event);
		forwarder.ForwardEvent(this, &UILayer::OnMouseReleased, event);
		forwarder.ForwardEvent(this, &UILayer::OnMouseMove, event);
		forwarder.ForwardEvent(this, &UILayer::OnWindowResize, event);
        return false;
    }


    uint32 UILayer::GetRecivableCategories() const
    {
        return EVENT_CATEGORY_INPUT | EVENT_CATEGORY_WINDOW;
    }
	
	
	bool UILayer::OnKeyTyped(const KeyTypedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharacter(event.GetCharacter());
		return false;
	}


	bool UILayer::OnKeyPressed(const KeyPressedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[event.GetKey()] = true;

		// Modifiers are not reliable across systems
		io.KeyCtrl	= io.KeysDown[KEY_LEFT_CONTROL] || io.KeysDown[KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[KEY_LEFT_SHIFT]	|| io.KeysDown[KEY_RIGHT_SHIFT];
		io.KeyAlt	= io.KeysDown[KEY_LEFT_ALT]		|| io.KeysDown[KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[KEY_LEFT_SUPER]	|| io.KeysDown[KEY_RIGHT_SUPER];
		return false;
	}

	
	bool UILayer::OnKeyReleased(const KeyReleasedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[event.GetKey()] = false;

		// Modifiers are not reliable across systems
		io.KeyCtrl	= io.KeysDown[KEY_LEFT_CONTROL] || io.KeysDown[KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[KEY_LEFT_SHIFT]	|| io.KeysDown[KEY_RIGHT_SHIFT];
		io.KeyAlt	= io.KeysDown[KEY_LEFT_ALT]		|| io.KeysDown[KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[KEY_LEFT_SUPER]	|| io.KeysDown[KEY_RIGHT_SUPER];
		return false;
	}
	
	
	bool UILayer::OnMouseScroll(const MouseScrolledEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH	+= event.GetHorizontalValue();
		io.MouseWheel	+= event.GetVerticalValue();
		return false;
	}
	
	
	bool UILayer::OnMousePressed(const MouseButtonPressedEvent& event)
	{
		int32 button = 0;
		switch (event.GetButton())
		{
		case MOUSEBUTTON_LEFT:		button = 0; break;
		case MOUSEBUTTON_MIDDLE:	button = 2; break;
		case MOUSEBUTTON_RIGHT:		button = 1; break;
		case MOUSEBUTTON_FORWARD:	button = 3; break;
		case MOUSEBUTTON_BACKWARD:	button = 4; break;
		default: break;
		}

		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[button] = true;
		return false;
	}
	
	
	bool UILayer::OnMouseReleased(const MouseButtonReleasedEvent& event)
	{
		int32 button = 0;
		switch (event.GetButton())
		{
		case MOUSEBUTTON_LEFT:		button = 0; break;
		case MOUSEBUTTON_MIDDLE:	button = 2; break;
		case MOUSEBUTTON_RIGHT:		button = 1; break;
		case MOUSEBUTTON_FORWARD:	button = 3; break;
		case MOUSEBUTTON_BACKWARD:	button = 4; break;
		default: break;
		}

		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[button] = false;
		return false;
	}
	
	
	bool UILayer::OnMouseMove(const MouseMovedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(float(event.GetX()), float(event.GetY()));
		return false;
	}
	
	
	bool UILayer::OnWindowResize(const WindowResizeEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(float(event.GetWidth()), float(event.GetHeight()));
		return false;
	}
}