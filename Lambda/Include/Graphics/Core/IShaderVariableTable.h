#pragma once
#include "IDeviceObject.h"

namespace Lambda
{
	class IBuffer;
	class ITexture;
	class ISamplerState;
	class IPipelineState;
	class IShaderVariableTable;

	//------------------
	//ShaderVariableDesc
	//------------------

	struct ShaderVariableDesc
	{
		const char* pName				= "";
		const char* pStaticSamplerName	= "";
		ResourceType Type				= RESOURCE_TYPE_UNKNOWN;
		ShaderStage	Stage				= SHADER_STAGE_UNKNOWN;
		Usage Usage						= USAGE_UNKNOWN;
		uint32 Slot						= 0;
	};

	//-----------------
	//ConstantBlockDesc
	//-----------------

	struct ConstantBlockDesc
	{
		ShaderStage	Stage	= SHADER_STAGE_UNKNOWN;
		uint32 SizeInBytes	= 0;
	};

	//----------------------
	//StaticSamplerStateDesc
	//----------------------

	struct StaticSamplerStateDesc
	{
		const char* pName				= "";
		SamplerAddressMode AdressMode	= SAMPLER_ADDRESS_MODE_REPEAT;
		float MinMipLOD					= 0.0f;
		float MaxMipLOD					= 0.0f;
		float MipLODBias				= 0.0f;
		float Anisotropy				= 1.0f;
	};

	//-----------------------
	//ShaderVariableTableDesc
	//-----------------------

	struct ShaderVariableTableDesc
	{
		uint32 NumVariables									= 0;
		const ShaderVariableDesc* pVariables				= nullptr;
		uint32 NumConstantBlocks							= 0;
		const ConstantBlockDesc* pConstantBlocks			= nullptr;
		uint32 NumStaticSamplerStates						= 0;
		const StaticSamplerStateDesc* pStaticSamplerStates	= nullptr;
	};

	//--------------
	//ShaderVariable
	//--------------

	class LAMBDA_API IShaderVariable : public IDeviceObject
	{
	public:
		LAMBDA_IOBJECT_INTERFACE(IShaderVariable);

		virtual void SetTexture(ITexture* pTexture) = 0;
		virtual void SetConstantBuffer(IBuffer* pBuffer) = 0;
		virtual void SetSamplerState(ISamplerState* pSamplerState) = 0;
		virtual IShaderVariableTable* GetShaderVariableTable() const = 0;
		virtual const ShaderVariableDesc& GetDesc() const = 0;
	};

	//-------------------
	//ShaderVariableTable
	//-------------------

	class LAMBDA_API IShaderVariableTable : public IDeviceObject
	{
	public:
		LAMBDA_IOBJECT_INTERFACE(IShaderVariableTable);

		virtual IShaderVariable* GetVariableByName(ShaderStage shader, const char* pName) = 0;
		virtual IShaderVariable* GetVariableByIndex(ShaderStage shader, uint32 index) = 0;
		virtual IPipelineState* GetPipelineState() const = 0;
		virtual uint32 GetVariableCount() const = 0;
	};
}