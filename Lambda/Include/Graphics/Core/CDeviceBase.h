#pragma once
#include "IDevice.h"

namespace Lambda
{
	//-----------
	//CDeviceBase
	//-----------

	class CDeviceBase : public RefCountedObject<IDevice>
	{
	public:
        CDeviceBase(const DeviceDesc& desc)
            : RefCountedObject<IDevice>(),
            m_Desc(desc),
            m_Features(),
            m_Properties()
        {
        }
        ~CDeviceBase() = default;

        LAMBDA_NO_COPY(CDeviceBase);

		virtual void CreateDefferedContext(IDeviceContext** ppList) override = 0;
		virtual void CreateBuffer(IBuffer** ppBuffer, const ResourceData* pInitalData, const BufferDesc& desc) override = 0;
		virtual void CreateTexture(ITexture** ppTexture, const ResourceData* pInitalData, const TextureDesc& desc) override = 0;
		virtual void CreateShader(IShader** ppShader, const ShaderDesc& desc) override = 0;
		virtual void CreateSamplerState(ISamplerState** ppSamplerState, const SamplerStateDesc& desc) override = 0;
		virtual void CreatePipelineState(IPipelineState** ppPipelineState, const PipelineStateDesc& desc) override = 0;
		virtual void CreateQuery(IQuery** ppQuery, const QueryDesc& desc) override = 0;

		virtual IDeviceContext* GetImmediateContext() const override = 0;
		virtual void* GetNativeHandle() const override = 0;


		virtual const DeviceFeatures& GetFeatures() const override
		{
			return m_Features;
		}


		virtual const DeviceProperties& GetProperties() const override
		{
			return m_Properties;
		}


		virtual const DeviceDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		DeviceDesc m_Desc;
		DeviceFeatures m_Features;
		DeviceProperties m_Properties;
	};
}
