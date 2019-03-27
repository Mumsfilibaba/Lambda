#pragma once
#include <LambdaPch.h>

namespace Lambda
{
	class IWindow;

	class LAMBDA_API Input
	{
	public:
		LAMBDA_NO_COPY(Input);

		Input() = default;
		virtual ~Input() = default;

	private:
		virtual bool InternalIsKeyUp(Key keycode) const = 0;
		virtual bool InternalIsKeyDown(Key keycode) const = 0;

		virtual void InternalSetMousePosition(uint32 x, uint32 y) const = 0;
		virtual void InternalGetMousePosition(uint32& x, uint32& y) const = 0;
		virtual void InternalSetMousePosition(IWindow* pRelativeTo, uint32 x, uint32 y) const = 0;
		virtual void InternalGetMousePosition(IWindow* pRelativeTo, uint32& x, uint32& y) const = 0;

	public:
		static bool IsKeyUp(Key keycode);
		static bool IsKeyDown(Key keycode);
		
		static void SetMousePosition(uint32 x, uint32 y);
		static void GetMousePosition(uint32& x, uint32& y);
		static void SetMousePosition(IWindow* pRelativeTo, uint32 x, uint32 y);
		static void GetMousePosition(IWindow* pRelativeTo, uint32& x, uint32& y);

	protected:
		static Input& GetInstance();

	private:
		static Input* Create();

	private:
		static std::unique_ptr<Input> s_Instance;
	};

	inline Input& Input::GetInstance()
	{
		return *s_Instance;
	}
}