#pragma once
#include <IObject.h>

namespace Lambda
{
	enum CommandListType : uint32
	{
		COMMAND_LIST_TYPE_UNKNOWN = 0,
		COMMAND_LIST_TYPE_GRAPHICS = 1,
		COMMAND_LIST_TYPE_COMPUTE = 2,
	};


	class LAMBDA_API ICommandList : public IObject
	{
	public:
		LAMBDA_INTERFACE(ICommandList);

		ICommandList() = default;
		~ICommandList() = default;
		
		virtual void Close() = 0;
		virtual void Reset() = 0;

	public:
		static ICommandList* Create(CommandListType type);
	};
}