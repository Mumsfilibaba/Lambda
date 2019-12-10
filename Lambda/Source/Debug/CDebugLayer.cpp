#include "LambdaPch.h"
#include "Debug/CDebugLayer.h"
#include "Core/Input/EKey.h"
#include "Graphics/Core/IShader.h"
#include "Graphics/Core/ISamplerState.h"
#include "Graphics/Core/IPipelineState.h"
#include "Graphics/Core/ITexture.h"
#include "Graphics/Core/IBuffer.h"
#include "Graphics/Core/IDevice.h"
#include "Graphics/Core/ISwapChain.h"
#include "Graphics/Renderer3D.h"

namespace Lambda
{
	//-------
	//SHADERS
	//-------

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


	//-----------
	//CDebugLayer
	//-----------

    CDebugLayer::CDebugLayer()
        : CLayer("DebugLayer"),
		m_VS(nullptr),
		m_PS(nullptr),
		m_SamplerState(nullptr),
		m_PipelineState(nullptr),
		m_FontTexture(nullptr),
		m_VertexBuffer(nullptr),
		m_IndexBuffer(nullptr),
		m_VariableTable(nullptr)
    {
		Init();
    }


    void CDebugLayer::Init()
    {
        //Create context for ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.BackendPlatformName = "Lambda Engine";
        //io.DisplaySize = ImVec2(float(pWindow->GetWidth()), float(pWindow->GetHeight()));

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
        //io.ImeWindowHandle = reinterpret_cast<HWND>(pWindow->GetNativeHandle());
#endif
                
        ImGui::StyleColorsDark();
        ImGui::GetStyle().WindowRounding    = 0.0f;
        ImGui::GetStyle().ChildRounding     = 0.0f;
        ImGui::GetStyle().FrameRounding     = 0.0f;
        ImGui::GetStyle().GrabRounding      = 0.0f;
        ImGui::GetStyle().PopupRounding     = 0.0f;
        ImGui::GetStyle().ScrollbarRounding = 0.0f;

		//Create graphics objects
		//Application& app = nullptr; Application::Get();
		IDevice* pDevice = nullptr;// app.GetGraphicsDevice();

		//Create shaders
		SShaderDesc shaderDesc = {};
#if defined(LAMBDA_DEBUG)
		shaderDesc.Flags = SHADER_FLAG_COMPILE_DEBUG;
#else
		shaderDesc.Flags = 0;
#endif
		shaderDesc.pEntryPoint = "main";
		shaderDesc.Type = SHADER_STAGE_VERTEX;

		const SDeviceProperties& deviceProps = pDevice->GetProperties();
		if (deviceProps.Api == GRAPHICS_API_VULKAN)
		{
			shaderDesc.pSource = reinterpret_cast<const char*>(__glsl_shader_vert_spv);
			shaderDesc.SourceLength = sizeof(__glsl_shader_vert_spv);
			shaderDesc.Languange = SHADER_LANG_SPIRV;
		}
		pDevice->CreateShader(&m_VS, shaderDesc);

		shaderDesc.Type = SHADER_STAGE_PIXEL;
		if (deviceProps.Api == GRAPHICS_API_VULKAN)
		{
			shaderDesc.pSource = reinterpret_cast<const char*>(__glsl_shader_frag_spv);
			shaderDesc.SourceLength = sizeof(__glsl_shader_frag_spv);
		}
		pDevice->CreateShader(&m_PS, shaderDesc);


		//Create sampler
		SStaticSamplerStateDesc samplerDesc = {};
		samplerDesc.pName		= "sFontSampler";
		samplerDesc.AdressMode	= SAMPLER_ADDRESS_MODE_REPEAT;
		samplerDesc.Anisotropy	= 1.0f;
		samplerDesc.MaxMipLOD	= 1000.0f;
		samplerDesc.MinMipLOD	= -1000.0f;
		samplerDesc.MipLODBias	= 0.0f;

		//Create pipelineresourcestate
		SShaderVariableDesc shaderVariables[1];
		shaderVariables[0].pName				= "sTexture";
		shaderVariables[0].pStaticSamplerName	= "sFontSampler";
		shaderVariables[0].Slot					= 0;
		shaderVariables[0].Type					= RESOURCE_TYPE_TEXTURE;
		shaderVariables[0].Usage				= USAGE_DEFAULT;
		shaderVariables[0].Stage				= SHADER_STAGE_PIXEL;

		SConstantBlockDesc constantBlocks[1];
		constantBlocks[0].Stage			= SHADER_STAGE_VERTEX;
		constantBlocks[0].SizeInBytes	= sizeof(float) * 4;

		SShaderVariableTableDesc variableTableDesc = {};
		variableTableDesc.NumVariables				= 1;
		variableTableDesc.pVariables				= shaderVariables;
		variableTableDesc.NumConstantBlocks			= 1;
		variableTableDesc.pConstantBlocks			= constantBlocks;
		variableTableDesc.NumStaticSamplerStates	= 1;
		variableTableDesc.pStaticSamplerStates		= &samplerDesc;

		//Create pipelinestate
		SInputElementDesc inputElements[3] =
		{
			{ "POSITION",   FORMAT_R32G32_FLOAT,	0, 0, sizeof(ImDrawVert), offsetof(ImDrawVert, pos),	false },
			{ "TEXCOORD",   FORMAT_R32G32_FLOAT,	0, 1, sizeof(ImDrawVert), offsetof(ImDrawVert, uv),		false },
			{ "COLOR",		FORMAT_R8G8B8A8_UNORM,	0, 2, sizeof(ImDrawVert), offsetof(ImDrawVert, col),    false },
		};

		SInputLayoutDesc vertexInput = {};
		vertexInput.ElementCount = 3;
		vertexInput.pElements = inputElements;

		SRasterizerStateDesc rasterizerState = {};
		rasterizerState.CullMode    = CULL_MODE_NONE;
		rasterizerState.PolygonMode = POLYGON_MODE_FILL;
		rasterizerState.FrontFaceCounterClockWise = true;

		SBlendStateDesc blendState = {};
		blendState.EnableBlending = true;

		SDepthStencilStateDesc depthStencilState = {};
		depthStencilState.DepthTest = false;

		SGraphicsPipelineStateDesc graphicsPipeline = {};
		graphicsPipeline.pVertexShader			= m_VS.Get();
		graphicsPipeline.pPixelShader			= m_PS.Get();
		graphicsPipeline.InputLayout			= vertexInput;
		graphicsPipeline.RasterizerState		= rasterizerState;
		graphicsPipeline.BlendState				= blendState;
		graphicsPipeline.DepthStencilState		= depthStencilState;
		graphicsPipeline.NumRenderTargets		= 1;
		graphicsPipeline.RenderTargetFormats[0] = FORMAT_B8G8R8A8_UNORM;
		graphicsPipeline.DepthStencilFormat		= FORMAT_D24_UNORM_S8_UINT;
		graphicsPipeline.Topology				= PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		graphicsPipeline.SampleCount = 1;// app.GetEngineParams().SampleCount;

		SPipelineStateDesc pipelineDesc = {};
		pipelineDesc.pName				 = "ImGui PipelineState";
		pipelineDesc.Type				 = PIPELINE_TYPE_GRAPHICS;
		pipelineDesc.ShaderVariableTable = variableTableDesc;
		pipelineDesc.GraphicsPipeline	 = graphicsPipeline;
		pDevice->CreatePipelineState(&m_PipelineState, pipelineDesc);
		m_PipelineState->CreateShaderVariableTable(&m_VariableTable);

		//Create fonttexture
		uint8* pPixels	= nullptr;
		int32 width		= 0;
		int32 height	= 0;


		io.Fonts->GetTexDataAsRGBA32(&pPixels, &width, &height);
		uint64 uploadSize = width * height * 4 * sizeof(char);

		STextureDesc fontTextureDesc = {};
		fontTextureDesc.pName		= "FontTexture";
		fontTextureDesc.Type		= TEXTURE_TYPE_2D;
		fontTextureDesc.Flags		= TEXTURE_FLAGS_SHADER_RESOURCE;
		fontTextureDesc.Format		= FORMAT_R8G8B8A8_UNORM;
		fontTextureDesc.Width		= width;
		fontTextureDesc.Height		= height;
		fontTextureDesc.Depth		= 1;
		fontTextureDesc.MipLevels	= 1;
		fontTextureDesc.ArraySize	= 1;
		fontTextureDesc.SampleCount = 1;
		fontTextureDesc.Usage		= USAGE_DEFAULT;

		SResourceData initalData = {};
		initalData.pData		= pPixels;
		initalData.SizeInBytes	= uploadSize;
		pDevice->CreateTexture(&m_FontTexture, &initalData, fontTextureDesc);

		//Transition texture
		STextureTransitionBarrier barrier = {};
		barrier.AfterState	= RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrier.MipLevel	= LAMBDA_ALL_MIP_LEVELS;
		barrier.pTexture	= m_FontTexture.Get();
		
		IDeviceContext* pContext = pDevice->GetImmediateContext();
		pContext->TransitionTextureStates(&barrier, 1);
		pContext->Release();

		m_VariableTable->GetVariableByIndex(SHADER_STAGE_PIXEL, 0)->SetTexture(m_FontTexture.Get());

		//Create vertex and indexbuffer
		SBufferDesc bufferDesc = {};
		bufferDesc.pName			= "ImGui VertexBuffer";
		bufferDesc.Flags			= BUFFER_FLAGS_VERTEX_BUFFER;
		bufferDesc.SizeInBytes		= MB(2);
		bufferDesc.StrideInBytes	= sizeof(ImDrawVert);
		bufferDesc.Usage			= USAGE_DYNAMIC;
		pDevice->CreateBuffer(&m_VertexBuffer, nullptr, bufferDesc);

		bufferDesc.pName			= "ImGui IndexBuffer";
		bufferDesc.Flags			= BUFFER_FLAGS_INDEX_BUFFER;
		bufferDesc.SizeInBytes		= MB(2);
		bufferDesc.StrideInBytes	= sizeof(ImDrawIdx);
		bufferDesc.Usage			= USAGE_DYNAMIC;
		pDevice->CreateBuffer(&m_IndexBuffer, nullptr, bufferDesc);
    }


	void CDebugLayer::Begin(const CTime& time)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = time.AsSeconds();

		ImGui::NewFrame();
	}

	
	void CDebugLayer::OnRenderUI(const CTime& time)
	{
        //Application& app = Application::Get();
        
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

			IDevice* pDevice = nullptr;// app.GetGraphicsDevice();
			const SDeviceProperties props = pDevice->GetProperties();
			if (props.Api == GRAPHICS_API_VULKAN)
				ImGui::Text("Renderer [Vulkan]");
			else if (props.Api == GRAPHICS_API_D3D12)
				ImGui::Text("Renderer [D3D12]");
			ImGui::Separator();

			ISwapChain* pSwapChain = nullptr;// app.GetSwapChain();
			const SwapChainDesc& swapChainDesc = pSwapChain->GetDesc();
			ImGui::Text("Vendor: %s", props.VendorString);
			ImGui::Text("Adapter: %s", props.AdapterString);
			ImGui::Text("Resolution: %u x %u", swapChainDesc.BufferWidth, swapChainDesc.BufferHeight);
			ImGui::Text("MSAA: %ux", swapChainDesc.BufferSampleCount);
			ImGui::Text("BackBufferCount: %u", swapChainDesc.BufferCount);
			ImGui::Text("VerticalSync: %s", swapChainDesc.VerticalSync ? "On" : "Off");

			ImGui::Separator();
			ImGui::Text("Timings:");
			ImGui::Separator();

			FrameStatistics stats = {};//sApplication::Get().GetRenderer().GetFrameStatistics();
            static float timer = 0.0f;
            timer += dt.AsMilliSeconds();

			constexpr uint32 valueCount = 90;
			static float cpuValues[valueCount]  = { 0 };
			static float gpuValues[valueCount] = { 0 };
            static int   valuesOffset  = 0;
			if (timer >= 8.0f)
			{
				cpuValues[valuesOffset] = dt.AsMilliSeconds();
				gpuValues[valuesOffset] = stats.GPUTime.AsMilliSeconds();
				valuesOffset = (valuesOffset+1) % valueCount;
				timer = 0.0f;
			}

            {
				static float timer2 = 0.0f;
				timer2 += stats.CPUTime.AsMilliSeconds();

				static float cpuMax = 1.0f;
				static float gpuMax = 1.0f;
                float averageCPU = 0.0f;
				float averageGPU = 0.0f;
				for (int n = 0; n < valueCount; n++)
				{
					averageCPU += cpuValues[n];
					averageGPU += gpuValues[n];
				}
				averageCPU /= (float)valueCount;
				averageGPU /= (float)valueCount;
				
				if (averageCPU > cpuMax)
					cpuMax = averageCPU * 1.5f;

				if (averageGPU > gpuMax)
					gpuMax = averageGPU * 1.5f;

				if (timer2 >= 100.0f)
				{
					cpuMax = averageCPU * 1.5f;
					gpuMax = averageGPU * 1.5f;
					timer2 = 0;
				}
				
				ImGui::Text("FPS: %d", stats.FPS);
				ImGui::Text("CPU Frametime (ms):");
                
				char overlay[32];
                sprintf(overlay, "Avg %f", averageCPU);
                ImGui::PlotLines("", cpuValues, valueCount, valuesOffset, overlay, 0.0f, cpuMax, ImVec2(0,80));

				ImGui::Text("GPU Frametime (ms):");

				sprintf(overlay, "Avg %f", averageGPU);
				ImGui::PlotLines("", gpuValues, valueCount, valuesOffset, overlay, 0.0f, gpuMax, ImVec2(0, 80));
            }
            
			ImGui::ShowDemoWindow();
            ImGui::PopStyleColor();
        }
        ImGui::End();
	}

	
	void CDebugLayer::End()
	{
		ImGui::EndFrame();
		ImGui::Render();
	}


	void CDebugLayer::Draw(IDeviceContext* pContext)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImDrawData* pDrawData = ImGui::GetDrawData();

		//uint64 vertexSize	= pDrawData->TotalVtxCount * sizeof(ImDrawVert);
		//uint64 indexSize	= pDrawData->TotalIdxCount * sizeof(ImDrawIdx);
		// pload vertex/index data into a single contiguous GPU buffer
		{
			ImDrawVert* vtxDst  = NULL;
			ImDrawIdx* idxDst   = NULL;
			pContext->MapBuffer(m_VertexBuffer.Get(), MAP_FLAG_WRITE | MAP_FLAG_WRITE_DISCARD, reinterpret_cast<void**>(&vtxDst));
			pContext->MapBuffer(m_IndexBuffer.Get(), MAP_FLAG_WRITE | MAP_FLAG_WRITE_DISCARD, reinterpret_cast<void**>(&idxDst));

			for (int n = 0; n < pDrawData->CmdListsCount; n++)
			{
				const ImDrawList* pCmdList = pDrawData->CmdLists[n];
				memcpy(vtxDst, pCmdList->VtxBuffer.Data, pCmdList->VtxBuffer.Size * sizeof(ImDrawVert));
				memcpy(idxDst, pCmdList->IdxBuffer.Data, pCmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
				vtxDst += pCmdList->VtxBuffer.Size;
				idxDst += pCmdList->IdxBuffer.Size;
			}

			pContext->UnmapBuffer(m_VertexBuffer.Get());
			pContext->UnmapBuffer(m_IndexBuffer.Get());
		}

		//Setup pipelinestate
		pContext->SetPipelineState(m_PipelineState.Get());
		//Set shader variable list
		pContext->SetShaderVariableTable(m_VariableTable.Get());

		//Setup vertex and indexbuffer
		pContext->SetVertexBuffers(&m_VertexBuffer, 1, 0);
		pContext->SetIndexBuffer(m_IndexBuffer.Get(), sizeof(ImDrawIdx) == 2 ? FORMAT_R16_UINT : FORMAT_R32_UINT);
		
		//Setup viewport
		SViewport viewport = {};
		viewport.TopX		= 0.0f;
		viewport.TopY		= 0.0f;
		viewport.Width		= io.DisplaySize.x;
		viewport.Height		= io.DisplaySize.y;
		viewport.MinDepth	= 0.0f;
		viewport.MaxDepth	= 1.0f;
		pContext->SetViewports(&viewport, 1);

		// Setup scale and translation:
		// Our visible imgui space lies from draw_data->DisplayPps (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
		{
			float scale[2];
			scale[0] = 2.0f / pDrawData->DisplaySize.x;
			scale[1] = -2.0f / pDrawData->DisplaySize.y;
			float translate[2];
			translate[0] = -1.0f + pDrawData->DisplayPos.x * scale[0];
			translate[1] = 1.0f - pDrawData->DisplayPos.y * scale[1];

			pContext->SetConstantBlocks(SHADER_STAGE_VERTEX, sizeof(float) * 0, sizeof(float) * 2, scale);
			pContext->SetConstantBlocks(SHADER_STAGE_VERTEX, sizeof(float) * 2, sizeof(float) * 2, translate);
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
					SRectangle scissor = {};
					scissor.X		= clipRect.x;
					scissor.Y		= clipRect.y;
					scissor.Width	= clipRect.z - clipRect.x;
					scissor.Height	= clipRect.w - clipRect.y;
					pContext->SetScissorRects(&scissor, 1);

					// Draw
					pContext->DrawIndexedInstanced(pCmd->ElemCount, 1, pCmd->IdxOffset + globalIdxOffset, pCmd->VtxOffset + globalVtxOffset, 0);
				}
			}
			globalIdxOffset += pCmdList->IdxBuffer.Size;
			globalVtxOffset += pCmdList->VtxBuffer.Size;
		}
	}


	void CDebugLayer::OnRelease()
    {
		//Destroy ImGui context
		ImGui::DestroyContext();

		//Destroy all objects
		m_VS.Release();
		m_PS.Release();
		m_PipelineState.Release();
		m_SamplerState.Release();
		m_FontTexture.Release();
		m_VertexBuffer.Release();
		m_IndexBuffer.Release();
    }


    void CDebugLayer::OnLoad()
    {
    }
}
