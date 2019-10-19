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

	//----------
	//VKNWrapper
	//----------

	class VKNWrapper
	{
	public:
		VKNWrapper::VKNWrapper(VKNWrapper&& other)
			: m_pResource(other.m_pResource)
		{
			other.m_pResource = nullptr;
		}

		~VKNWrapper()
		{
			if (m_pResource)
			{
				m_pResource->Release();
				m_pResource = nullptr;
			}
		}

		VKNWrapper& operator=(VKNWrapper&& other)
		{
			if (this != &other)
			{
				if (m_pResource)
					m_pResource->Release();

				m_pResource = other.m_pResource;
				other.m_pResource = nullptr;
			}

			return *this;
		}

		VKNWrapper(const VKNWrapper& other)				= delete;
		VKNWrapper& operator=(const VKNWrapper& other)	= delete;
	private:
		VKNWrapper(VKNResourceBase* pResource = nullptr)
			:m_pResource(pResource)
		{
		}
	private:
		VKNResourceBase* m_pResource;
	public:
		template <typename VkResourceType>
		static VKNWrapper Create(VKNDevice* pDevice, const VkResourceType& resource)
		{
			return VKNWrapper(DBG_NEW VKNResource<VkResourceType>(pDevice, resource));
		}
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
			m_Resources.emplace_back(std::pair<uint64, VKNWrapper>(m_FrameIndex, std::move(VKNWrapper::Create<VkResourceType>(m_pDevice, resource))));
		}

		void EmptyResources();
	private:
		VKNDevice* m_pDevice;
		uint64 m_FrameIndex;
		std::vector<std::pair<uint64, VKNWrapper>> m_Resources;
	};
}