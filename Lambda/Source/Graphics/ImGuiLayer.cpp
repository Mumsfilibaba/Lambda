#include "LambdaPch.h"
#include "Graphics/ImGuiLayer.h"
#include "System/Application.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"
#include <imgui.h>

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


	//----------
	//ImGuiLayer
	//----------

    ImGuiLayer::ImGuiLayer()
        : EventLayer("ImGuiLayer")
    {
    }


    void ImGuiLayer::OnPop()
    {
		//Destroy ImGui context
		ImGui::DestroyContext();
    }


    void ImGuiLayer::OnPush()
    {
		//Create context for ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
		io.BackendPlatformName = "Lambda Engine";

		// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
		io.KeyMap[ImGuiKey_Tab]			= KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow]	= KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow]	= KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow]		= KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow]	= KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp]		= KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown]	= KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home]		= KEY_HOME;
		io.KeyMap[ImGuiKey_End]			= KEY_END;
		io.KeyMap[ImGuiKey_Insert]		= KEY_INSERT;
		io.KeyMap[ImGuiKey_Delete]		= KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace]	= KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Space]		= KEY_SPACE;
		io.KeyMap[ImGuiKey_Enter]		= KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape]		= KEY_ESCAPE;
		io.KeyMap[ImGuiKey_KeyPadEnter]	= KEY_KEYPAD_ENTER;
		io.KeyMap[ImGuiKey_A]			= KEY_A;
		io.KeyMap[ImGuiKey_C]			= KEY_C;
		io.KeyMap[ImGuiKey_V]			= KEY_V;
		io.KeyMap[ImGuiKey_X]			= KEY_X;
		io.KeyMap[ImGuiKey_Y]			= KEY_Y;
		io.KeyMap[ImGuiKey_Z]			= KEY_Z;

#if defined(LAMBDA_PLAT_WINDOWS)
		IWindow* pWindow = Application::Get().GetWindow();
		io.ImeWindowHandle = reinterpret_cast<HWND>(pWindow->GetNativeHandle());
#endif
    }


    bool ImGuiLayer::OnEvent(const Event& event)
    {
		EventDispatcher::ForwardEvent<ImGuiLayer, KeyTypedEvent>(this, &ImGuiLayer::OnKeyTyped, event);
		EventDispatcher::ForwardEvent<ImGuiLayer, KeyPressedEvent>(this, &ImGuiLayer::OnKeyPressed, event);
		EventDispatcher::ForwardEvent<ImGuiLayer, MouseScrolledEvent>(this, &ImGuiLayer::OnMouseScroll, event);
		EventDispatcher::ForwardEvent<ImGuiLayer, MouseButtonPressedEvent>(this, &ImGuiLayer::OnMousePressed, event);
		EventDispatcher::ForwardEvent<ImGuiLayer, MouseButtonReleasedEvent>(this, &ImGuiLayer::OnMouseReleased, event);
		EventDispatcher::ForwardEvent<ImGuiLayer, MouseMovedEvent>(this, &ImGuiLayer::OnMouseMove, event);
		EventDispatcher::ForwardEvent<ImGuiLayer, WindowResizeEvent>(this, &ImGuiLayer::OnWindowResize, event);
        return false;
    }


    uint32 ImGuiLayer::GetRecivableCategories() const
    {
        return EVENT_CATEGORY_INPUT | EVENT_CATEGORY_WINDOW;
    }
	
	
	bool ImGuiLayer::OnKeyTyped(const KeyTypedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharacter(event.GetCharacter());
		return false;
	}


	bool ImGuiLayer::OnKeyPressed(const KeyPressedEvent& event)
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

	
	bool ImGuiLayer::OnKeyReleased(const KeyReleasedEvent& event)
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
	
	
	bool ImGuiLayer::OnMouseScroll(const MouseScrolledEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH	+= event.GetHorizontalValue();
		io.MouseWheel	+= event.GetVerticalValue();
		return false;
	}
	
	
	bool ImGuiLayer::OnMousePressed(const MouseButtonPressedEvent& event)
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
	
	
	bool ImGuiLayer::OnMouseReleased(const MouseButtonReleasedEvent& event)
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
	
	
	bool ImGuiLayer::OnMouseMove(const MouseMovedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(float(event.GetX()), float(event.GetY()));
		return false;
	}
	
	
	bool ImGuiLayer::OnWindowResize(const WindowResizeEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(float(event.GetWidth()), float(event.GetHeight()));
		return false;
	}
}
