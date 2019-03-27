#include <System/Application.h>
#include <System/Log.h>
#include <System/Clock.h>
#include <Math/Math.h>
#include <Math/Float3.h>
#include <crtdbg.h>
#include <string>

bool OnEvent(const Lambda::Event& event)
{
	using namespace Lambda;

	if (event.Type == EVENT_TYPE_KEYDOWN)
	{
		LOG_SYSTEM_PRINT("Key down. Repeat: %d\n", event.KeyEvent.KeyCode);
		return true;
	}
	else if (event.Type == EVENT_TYPE_TEXT)
	{
		LOG_SYSTEM_PRINT("Text event. Repeat: %c\n", (char)event.TextEvent.Character);
		return true;
	}
	else if (event.Type == EVENT_TYPE_MOUSE_MOVED)
	{
		LOG_SYSTEM_PRINT("Mouse moved. x: %d, y: %d\n", event.MouseMoveEvent.PosX, event.MouseMoveEvent.PosY);
		return true;
	}
	else if (event.Type == EVENT_TYPE_JOYSTICK_CHANGED)
	{
		LOG_SYSTEM_PRINT("Joystick Changed:\nID: %d\nLeftTrigger: %d, RightTrigger: %d\nLeft Thumb x: %d, y: %d\nRight Thumb x: %d, y: %d\n",
			event.JoystickChanged.ControllerID, 
			event.JoystickChanged.LeftTrigger, event.JoystickChanged.RightTrigger,
			event.JoystickChanged.LeftX, event.JoystickChanged.LeftY, event.JoystickChanged.RightX, event.JoystickChanged.RightY);
		return true;
	}

	return false;
}

int main()
{
	using namespace Lambda;
	using namespace Math;

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Application* pApp = new Application();

	EventLayer uiLayer = { OnEvent, "UILayer" };
	EventDispatcher::PushEventLayer(uiLayer);

	LOG_SYSTEM_PRINT("\n--TESTING FLOAT4X4--\n");
	Float4 f4 = Float4(6.0f, 5.0f, 7.0f, 1.0f);
	Float4x4 f4x4 = Float4x4(2.0f);
	Float4x4 f4x42 = Float4x4(Float4(1.0f, 2.0f, 3.0f, 4.0f), Float4(5.0f, 6.0f, 7.0f, 8.0f), Float4(9.0f, 10.0f, 11.0f, 12.0f), Float4(13.0f, 14.0f, 15.0f, 16.0f));
	LOG_SYSTEM_PRINT("Expect:\n[2.0f, 0.0f, 0.0f, 0.0f]\n[0.0f, 2.0f, 0.0f, 0.0f]\n[0.0f, 0.0f, 2.0f, 0.0f]\n[0.0f, 0.0f, 0.0f, 2.0f]\n%s", ToString(f4x4).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[1.0f, 2.0f, 3.0f, 4.0f]\n[5.0f, 6.0f, 7.0f, 8.0f]\n[9.0f, 10.0f, 11.0f, 12.0f]\n[13.0f, 14.0f, 15.0f, 16.0f]\n%s", ToString(f4x42).c_str());
	LOG_SYSTEM_PRINT("Expect: 107.0f, 126.0f, 145.0f, 164.0f - %s\n", ToString(f4x42.Multiply(f4)).c_str());
	
	LOG_SYSTEM_PRINT("Expect:\n[2.0f, 4.0f, 6.0f, 8.0f]\n[10.0f, 12.0f, 14.0f, 16.0f]\n[18.0f, 20.0f, 22.0f, 24.0f]\n[26.0f, 28.0f, 30.0f, 32.0f]\n%s", ToString(f4x42.Multiply(f4x4)).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[4.0f, 8.0f, 12.0f, 16.0f]\n[20.0f, 24.0f, 28.0f, 32.0f]\n[36.0f, 40.0f, 44.0f, 48.0f]\n[52.0f, 56.0f, 60.0f, 64.0f]\n%s", ToString(f4x42.Multiply(2.0f)).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[6.0f, 8.0f, 12.0f, 16.0f]\n[20.0f, 26.0f, 28.0f, 32.0f]\n[36.0f, 40.0f, 46.0f, 48.0f]\n[52.0f, 56.0f, 60.0f, 66.0f]\n%s", ToString(f4x42.Add(f4x4)).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[11.0f, 13.0f, 17.0f, 21.0f]\n[25.0f, 31.0f, 33.0f, 37.0f]\n[41.0f, 45.0f, 51.0f, 53.0f]\n[57.0f, 61.0f, 65.0f, 71.0f]\n%s", ToString(f4x42.Add(5.0f)).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[9.0f, 13.0f, 17.0f, 21.0f]\n[25.0f, 29.0f, 33.0f, 37.0f]\n[41.0f, 45.0f, 49.0f, 53.0f]\n[57.0f, 61.0f, 65.0f, 69.0f]\n%s", ToString(f4x42.Subtract(f4x4)).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[2.0f, 6.0f, 10.0f, 14.0f]\n[18.0f, 22.0f, 26.0f, 30.0f]\n[34.0f, 38.0f, 42.0f, 46.0f]\n[50.0f, 54.0f, 58.0f, 62.0f]\n%s", ToString(f4x42.Subtract(7.0f)).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[1.0f, 3.0f, 5.0f, 7.0f]\n[9.0f, 11.0f, 13.0f, 15.0f]\n[17.0f, 19.0f, 21.0f, 23.0f]\n[25.0f, 27.0f, 29.0f, 31.0f]\n%s", ToString(f4x42.Divide(2.0f)).c_str());
	
	LOG_SYSTEM_PRINT("Expect: false - ");
	if (f4x42.Equals(f4x4))
	{
		LOG_SYSTEM_PRINT("true\n");
	}
	else
	{
		LOG_SYSTEM_PRINT("false\n");
	}
	
	Float4x4 mat;
	mat.SetRow(0, 2.0f, 4.0f, 6.0f, -4.0f);
	mat.SetRow(1, 5.0f, 3.0f, 1.0f, 2.0f);
	mat.SetRow(2, -2.0f, 0.0f, 2.0f, 3.5f);
	mat.SetRow(3, 2.0f, -1.0f, 2.0f, 3.5f);
	LOG_SYSTEM_PRINT("Expect:\n[2.0f, 4.0f, 6.0f, -4.0f]\n[5.0f, 3.0f, 1.0f, 2.0f]\n[-2.0f, 0.0f, 2.0f, 3.5f]\n[2.0f, -1.0f, 2.0f, 3.5f]\n%s\n%s\n%s\n%s\n", 
		ToString(mat.GetRow(0)).c_str(), ToString(mat.GetRow(1)).c_str(), ToString(mat.GetRow(2)).c_str(), ToString(mat.GetRow(3)).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[2.0f, 4.0f, 6.0f, -4.0f]\n[5.0f, 3.0f, 1.0f, 2.0f]\n[-2.0f, 0.0f, 2.0f, 3.5f]\n[2.0f, -1.0f, 2.0f, 3.5f]\n%.6f, %.6f, %.6f, %.6f\n%.6f, %.6f, %.6f, %.6f\n%.6f, %.6f, %.6f, %.6f\n%.6f, %.6f, %.6f, %.6f\n", 
		mat.GetElement(0, 0), mat.GetElement(0, 1), mat.GetElement(0, 2), mat.GetElement(0, 3),
		mat.GetElement(1, 0), mat.GetElement(1, 1), mat.GetElement(1, 2), mat.GetElement(1, 3),
		mat.GetElement(2, 0), mat.GetElement(2, 1), mat.GetElement(2, 2), mat.GetElement(2, 3),
		mat.GetElement(3, 0), mat.GetElement(3, 1), mat.GetElement(3, 2), mat.GetElement(3, 3));

	LOG_SYSTEM_PRINT("Expect:\n[2.0f, 5.0f, -2.0f, 2.0f]\n[4.0f, 3.0f, 0.0f, -1.0f]\n[6.0f, 1.0f, 2.0f, 2.0f]\n[-4.0f, 2.0f, 3.5f, 3.5f]\n%s", ToString(mat.GetTranspose()).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[0.0009f, 0.0543f, -0.1966f, 0.1666f]\n[0.0037f, 0.2172f, 0.2134f, -0.3333f]\n[0.1189f, -0.1029f, 0.0280f, 0.1666f]\n[-0.0674f, 0.0898f, 0.1573f, 0.0f]\n%s", ToString(mat.GetInverse()).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[0.5f, 2.0f, 64.5f, 36.0f]\n[29.0f, 116.0f, -55.0f, 48.0f]\n[-105.0f, 114.0f, 15.0f, 84.0f]\n[89.0f, -178.0f, 89.0f, 0.0f]\n%s", ToString(mat.Cofactor()).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[0.5f, 29.0f, -105.0f, 89.0f]\n[2.0f, 116.0f, 114.0f, -178.0f]\n[63.5f, -55.0f, 15.0f, 89.0f]\n[-36.0f, 48.0f, 84.0f, 0.0f]\n%s", ToString(mat.Adjugate()).c_str());
	LOG_SYSTEM_PRINT("Expect: 534.0f - %.6f\n", mat.Determinant());
	
	
	LOG_SYSTEM_PRINT("Expect:\n[1.0f, 0.0f, 0.0f, 0.0f]\n[0.0f, 1.0f, 0.0f, 0.0f]\n[0.0f, 0.0f, 1.0f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::Identity()).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[1.0f, 0.0f, 0.0f, 0.0f]\n[0.0f, 1.0f, 0.0f, 0.0f]\n[0.0f, 0.0f, 1.0f, 0.0f]\n[-1.0f, 5.0f, 8.0f, 1.0f]\n%s", ToString(Float4x4::Translation(Vec3f(-1.0f, 5.0f, 8.0f))).c_str());
	
	LOG_SYSTEM_PRINT("Expect:\n[1.0f, 0.0f, 0.0f, 0.0f]\n[0.0f, 0.6427f, 0.7660f, 0.0f]\n[0.0f, -0.7660f, 0.6427f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::RotationX(ToRadiansF(50.0f))).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.0f, -0.7660f, 0.0f]\n[0.0f, 1.0f, 0.0f, 0.0f]\n[0.7660f, 0.0f, 0.6427f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::RotationY(ToRadiansF(50.0f))).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.7660f, 0.0f, 0.0f]\n[-0.7660f, 0.6427f, 0.0f, 0.0f]\n[0.0f, 0.0f, 1.0f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::RotationZ(ToRadiansF(50.0f))).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[1.0f, 0.0f, 0.0f, 0.0f]\n[0.0f, 0.6427f, 0.7660f, 0.0f]\n[0.0f, -0.7660f, 0.6427f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::Rotation(Vec3f(1.0f, 0.0f, 0.0f), ToRadiansF(50.0f))).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.0f, -0.7660f, 0.0f]\n[0.0f, 1.0f, 0.0f, 0.0f]\n[0.7660f, 0.0f, 0.6427f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::Rotation(Vec3f(0.0f, 1.0f, 0.0f), ToRadiansF(50.0f))).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.7660f, 0.0f, 0.0f]\n[-0.7660f, 0.6427f, 0.0f, 0.0f]\n[0.0f, 0.0f, 1.0f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::Rotation(Vec3f(0.0f, 0.0f, 1.0f), ToRadiansF(50.0f))).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[1.0f, 0.0f, 0.0f, 0.0f]\n[0.0f, 0.6427f, 0.7660f, 0.0f]\n[0.0f, -0.7660f, 0.6427f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::RotationRollPitchYaw(0.0f, ToRadiansF(50.0f), 0.0f)).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.0f, -0.7660f, 0.0f]\n[0.0f, 1.0f, 0.0f, 0.0f]\n[0.7660f, 0.0f, 0.6427f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::RotationRollPitchYaw(0.0f, 0.0f, ToRadiansF(50.0f))).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[0.6427f, 0.7660f, 0.0f, 0.0f]\n[-0.7660f, 0.6427f, 0.0f, 0.0f]\n[0.0f, 0.0f, 1.0f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::RotationRollPitchYaw(ToRadiansF(50.0f), 0.0f, 0.0f)).c_str());
	
	LOG_SYSTEM_PRINT("Expect:\n[0.6683f, -0.3329f, -0.6652f]\n[0.4860f, 0.8724f, 0.0516f]\n[0.5631f, -0.3578f, 0.7448f]\n%s", ToString(Float4x4::Rotation(Vec3f(0.5f, 1.5f, -1.0f), ToRadiansF(50.0f))).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[0.5f, 0.0f, 0.0f, 0.0f]\n[0.0f, 0.75f, 0.0f, 0.0f]\n[0.0f, 0.0f, 2.0f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::Scale(Vec3f(0.5f, 0.75f, 2.0f))).c_str());
	LOG_SYSTEM_PRINT("Expect:\n[2.0f, 0.0f, 0.0f, 0.0f]\n[0.0f, 2.0f, 0.0f, 0.0f]\n[0.0f, 0.0f, 2.0f, 0.0f]\n[0.0f, 0.0f, 0.0f, 1.0f]\n%s", ToString(Float4x4::Scale(2.0f)).c_str());

	int32 result = pApp->Run();
	delete pApp;

	return result;
}