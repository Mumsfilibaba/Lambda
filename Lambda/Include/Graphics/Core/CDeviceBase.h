#pragma once
#include "IDevice.h"
#include "Core/CRefCountBase.h"

namespace Lambda
{
	//-----------
	//CDeviceBase
	//-----------

	class CDeviceBase : public CRefCountBase<IDevice>
	{
	public:
        CDeviceBase(const SDeviceDesc& desc)
            : CRefCountBase<IDevice>(),
            m_Desc(desc),
            m_Features(),
            m_Properties()
        {
        }
        ~CDeviceBase() = default;

        LAMBDA_NO_COPY(CDeviceBase);

		virtual void CreateDefferedContext(IDeviceContext** ppList) override = 0;
		virtual void CreateBuffer(IBuffer** ppBuffer, const SResourceData* pInitalData, const SBufferDesc& desc) override = 0;
		virtual void CreateTexture(ITexture** ppTexture, const SResourceData* pInitalData, const STextureDesc& desc) override = 0;
		virtual void CreateShader(IShader** ppShader, const SShaderDesc& desc) override = 0;
		virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SSamplerStateDesc& desc) override = 0;
		virtual void CreatePipelineState(IPipelineState** ppPipelineState, const SPipelineStateDesc& desc) override = 0;
		virtual void CreateQuery(IQuery** ppQuery, const SQueryDesc& desc) override = 0;

		virtual IDeviceContext* GetImmediateContext() const override = 0;
		virtual void* GetNativeHandle() const override = 0;


		virtual const SDeviceFeatures& GetFeatures() const override
		{
			return m_Features;
		}


		virtual const SDeviceProperties& GetProperties() const override
		{
			return m_Properties;
		}


		virtual const SDeviceDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		SDeviceDesc m_Desc;
		SDeviceFeatures m_Features;
		SDeviceProperties m_Properties;
	};
}
