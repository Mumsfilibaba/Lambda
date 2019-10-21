#pragma once
#include "Defines.h"
#include "Types.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace Lambda
{
	class VKNDevice;

	//---------------
	//VKNResourceBase
	//---------------

	class VKNResourceBase
	{
	public:
		LAMBDA_INTERFACE(VKNResourceBase);
		virtual void Release() = 0;
	};

	//-----------
	//VKNResource
	//-----------

	template<typename VkResourceType>
	class VKNResource final : public VKNResourceBase
	{
	public:
		LAMBDA_NO_COPY(VKNResource);

		VKNResource(VKNDevice* pDevice, const VkResourceType& resource) : m_pDevice(pDevice), m_Resource(resource) { }
		~VKNResource() { }

		void DestroyResource();
		virtual void Release() override final
		{
			DestroyResource();
			delete this;
		}
	private:
		VKNDevice* m_pDevice;
		VkResourceType m_Resource;
	};

	//---------------------
	//VKNSafeReleaseManager
	//---------------------

	class VKNSafeReleaseManager final
	{
	public:
		LAMBDA_NO_COPY(VKNSafeReleaseManager);

		VKNSafeReleaseManager(VKNDevice* pDevice);
		~VKNSafeReleaseManager();

		template<typename VkResourceType>
		void ReleaseResource(const VkResourceType& resource)
		{
			m_Resources.emplace_back(std::pair<uint64, VKNResourceBase*>(m_FrameIndex, DBG_NEW VKNResource<VkResourceType>(m_pDevice, resource)));
		}

		void EmptyResources();
	private:
		VKNDevice* m_pDevice;
		uint64 m_FrameIndex;
		std::vector<std::pair<uint64, VKNResourceBase*>> m_Resources;
	};
}