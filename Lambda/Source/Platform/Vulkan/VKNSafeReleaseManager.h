#pragma once
#include "Defines.h"
#include "Types.h"
#include <vector>
#include "Vulkan.h"

//#define LAMBDA_SAFE_RESOURCE_DEBUG

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
		virtual uint64 GetResourceHandle() const = 0;
	};

	//-----------
	//VKNResource
	//-----------

	template<typename VkResourceType>
	class VKNResource final : public VKNResourceBase
	{
	public:
		LAMBDA_NO_COPY(VKNResource);

		VKNResource(VKNDevice* pVkDevice, const VkResourceType& resource) : m_pVkDevice(pVkDevice), m_VkResource(resource) { }
		~VKNResource() { }

		void DestroyResource();


		virtual uint64 GetResourceHandle() const override final
		{
			return (uint64)m_VkResource;
		}


		virtual void Release() override final
		{
			DestroyResource();
			delete this;
		}
	private:
		VKNDevice* m_pVkDevice;
		VkResourceType m_VkResource;
	};

	//---------------------
	//VKNSafeReleaseManager
	//---------------------

	class VKNSafeReleaseManager final
	{
	public:
		LAMBDA_NO_COPY(VKNSafeReleaseManager);

		VKNSafeReleaseManager(VKNDevice* pVkDevice);
		~VKNSafeReleaseManager();

		template<typename VkResourceType>
		void ReleaseResource(const VkResourceType& resource)
		{
#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
			LOG_DEBUG_WARNING("[Vulkan] Releasing VkResource '%p' Frame=%llu\n", resource, m_FrameIndex);
#endif
			m_Resources.emplace_back(std::pair<uint64, VKNResourceBase*>(m_FrameIndex, DBG_NEW VKNResource<VkResourceType>(m_pVkDevice, resource)));
		}

		void EmptyResources();
	private:
		std::vector<std::pair<uint64, VKNResourceBase*>> m_Resources;
		VKNDevice* m_pVkDevice;
		uint64 m_FrameIndex;
	};
}