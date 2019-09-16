#pragma once
#include "Graphics/IBuffer.h"
#include <string>
#include "VKNAllocator.h"

namespace Lambda
{
	//---------
	//VKNBuffer
	//---------

    class VKNBuffer final : public IBuffer
    {
    public:
        LAMBDA_NO_COPY(VKNBuffer);
        
        VKNBuffer(IVKNAllocator* pAllocator, const BufferDesc& desc);
        ~VKNBuffer() = default;
        
        virtual void		Map(void** ppMem) override final;
        virtual void		Unmap() override final;
        virtual void*		GetNativeHandle() const override final;
        virtual BufferDesc	GetDesc() const override final;

		bool	IsDirty() const;
		void	SetIsClean();
		void	AdvanceFrame(uint32 frameCount);
		void	DynamicUpdate(const ResourceData* pData);
		void	Reallocate(uint32 sizeInBytes);
		uint32	GetDynamicOffset() const;

        //Release releases vulkan resources while destroy also calls 'delete this'
        void	Release(VkDevice device);
        void	Destroy(VkDevice device);
        
    private:
        void Init(const BufferDesc& desc);
        
    private:
		IVKNAllocator* const	m_pAllocator;
        VKNMemory				m_Memory;
        VkBuffer				m_Buffer;
		uint32					m_CurrentFrame;
		uint32					m_SizePerFrame;
		uint32					m_SizePerUpdate;
		uint32					m_DynamicOffset;
		uint32					m_TotalDynamicOffset;
        BufferDesc				m_Desc;
		bool					m_IsDirty;
    };


	inline bool VKNBuffer::IsDirty() const
	{
		return m_IsDirty;
	}


	inline void VKNBuffer::SetIsClean()
	{
		m_IsDirty = false;
	}


	inline uint32 VKNBuffer::GetDynamicOffset() const
	{
		return m_TotalDynamicOffset;
	}


	//----------------
	//VKNBufferManager
	//----------------


	class VKNBufferManager final
	{
	public:
		LAMBDA_NO_COPY(VKNBufferManager);

		VKNBufferManager();
		~VKNBufferManager();

		void MoveToNextFrame(uint32 frameCount);
        void DestroyBuffer(VkBuffer buffer, uint32 frameCount);
		void RegisterBuffer(VKNBuffer* pBuffer);
		void UnregisterBuffer(VKNBuffer* pBuffer);

	private:
		void CleanupBuffers(uint32 frameCount);

	private:
        std::vector<VKNBuffer*>				m_Buffers;
        std::vector<std::vector<VkBuffer>>  m_BuffersToDelete;

	public:
		static VKNBufferManager& GetInstance();

	private:
		static VKNBufferManager* s_pInstance;
	};
}
