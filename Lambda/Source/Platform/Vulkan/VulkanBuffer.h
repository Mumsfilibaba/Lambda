#pragma once
#include "Graphics/IBuffer.h"
#include "VulkanAllocator.h"

namespace Lambda
{
	//------------
	//VulkanBuffer
	//------------

    class VulkanBuffer final : public IBuffer
    {
    public:
        LAMBDA_NO_COPY(VulkanBuffer);
        
        VulkanBuffer(IVulkanAllocator* pAllocator, const BufferDesc& desc);
        ~VulkanBuffer() = default;
        
        virtual void		Map(void** ppMem) override final;
        virtual void		Unmap() override final;
        virtual void*		GetNativeHandle() const override final;
        virtual BufferDesc	GetDesc() const override final;

		bool	IsDirty() const;
		void	SetIsClean();
		void	AdvanceFrame();
		void	DynamicUpdate(const ResourceData* pData);
		void	Reallocate(uint32 sizeInBytes);
		uint32	GetDynamicOffset() const;

        //Release releases vulkan resources while destroy also calls 'delete this'
        void	Release(VkDevice device);
        void	Destroy(VkDevice device);
        
    private:
        void Init(const BufferDesc& desc);
        
    private:
		IVulkanAllocator* const m_pAllocator;
        VulkanMemory			m_Memory;
        VkBuffer				m_Buffer;
        BufferDesc				m_Desc;
		uint32					m_CurrentFrame;
		uint32					m_FrameCount;
		uint32					m_SizePerFrame;
		uint32					m_SizePerUpdate;
		uint32					m_DynamicOffset;
		uint32					m_TotalDynamicOffset;
        uint64					m_DynamicAlignment;
		bool					m_IsDirty;
    };

	//--------------------------
	//VulkanDynamicBufferManager
	//--------------------------

	class VulkanDynamicBufferManager final
	{
	public:
		LAMBDA_NO_COPY(VulkanDynamicBufferManager);

		VulkanDynamicBufferManager();
		~VulkanDynamicBufferManager();

		void MoveToNextFrame();
		void RegisterBuffer(VulkanBuffer* pBuffer);
		void UnregisterBuffer(VulkanBuffer* pBuffer);

	private:
		std::vector<VulkanBuffer*> m_Buffers;

	public:
		static VulkanDynamicBufferManager& GetInstance();

	private:
		static VulkanDynamicBufferManager* s_pInstance;
	};
}
