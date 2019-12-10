#pragma once
#include "IDeviceObject.h"

namespace Lambda
{
	class IBuffer;
	class ITexture;
	class ISamplerState;
	class IPipelineState;
	class IShaderVariableTable;

	//-------------------
	//SShaderVariableDesc
	//-------------------

	struct SShaderVariableDesc
	{
		const char* pName				= "";
		const char* pStaticSamplerName	= "";
		EResourceType Type				= RESOURCE_TYPE_UNKNOWN;
		EShaderStage Stage			    = SHADER_STAGE_UNKNOWN;
		EUsage Usage					= USAGE_UNKNOWN;
		uint32 Slot						= 0;
	};

	//------------------
	//SConstantBlockDesc
	//------------------

	struct SConstantBlockDesc
	{
		EShaderStage	Stage	= SHADER_STAGE_UNKNOWN;
		uint32 SizeInBytes	= 0;
	};

	//-----------------------
	//SStaticSamplerStateDesc
	//-----------------------

	struct SStaticSamplerStateDesc
	{
		const char* pName				= "";
		ESamplerAddressMode AdressMode	= SAMPLER_ADDRESS_MODE_REPEAT;
		float MinMipLOD					= 0.0f;
		float MaxMipLOD					= 0.0f;
		float MipLODBias				= 0.0f;
		float Anisotropy				= 1.0f;
	};

	//------------------------
	//SShaderVariableTableDesc
	//------------------------

	struct SShaderVariableTableDesc
	{
		uint32 NumVariables									= 0;
		const SShaderVariableDesc* pVariables				= nullptr;
		uint32 NumConstantBlocks							= 0;
		const SConstantBlockDesc* pConstantBlocks			= nullptr;
		uint32 NumStaticSamplerStates						= 0;
		const SStaticSamplerStateDesc* pStaticSamplerStates	= nullptr;
	};

	//---------------
	//IShaderVariable
	//---------------

	class LAMBDA_API IShaderVariable : public IDeviceObject
	{
	public:
		LAMBDA_IOBJECT_INTERFACE(IShaderVariable);

		virtual void SetTexture(ITexture* pTexture) = 0;
		virtual void SetConstantBuffer(IBuffer* pBuffer) = 0;
		virtual void SetSamplerState(ISamplerState* pSamplerState) = 0;
		virtual IShaderVariableTable* GetShaderVariableTable() const = 0;
		virtual const SShaderVariableDesc& GetDesc() const = 0;
	};

	//--------------------
	//IShaderVariableTable
	//--------------------

    class LAMBDA_API IShaderVariableTable : public IDeviceObject
	{
	public:
		LAMBDA_IOBJECT_INTERFACE(IShaderVariableTable);

		virtual IShaderVariable* GetVariableByName(EShaderStage shader, const char* pName) = 0;
		virtual IShaderVariable* GetVariableByIndex(EShaderStage shader, uint32 index) = 0;
		virtual IPipelineState* GetPipelineState() const = 0;
		virtual uint32 GetVariableCount() const = 0;
	};
}
