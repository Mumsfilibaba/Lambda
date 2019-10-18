#pragma once
#include "Defines.h"
#include "Types.h"
#include <vector>
#include <mutex>
#include <map>
#include <unordered_map>
#include <vulkan/vulkan.h>

namespace Lambda
{
	//--------------
	//VKNImageLayout
	//--------------

	struct VKNImageLayout
	{
		VkImageLayout Layout = VK_IMAGE_LAYOUT_UNDEFINED;
		mutable std::map<uint32, VkImageLayout> SubLayouts;
	};

	//---------------------
	//VKNResourceLayoutTracker
	//---------------------

	class VKNResourceLayoutTracker final
	{
		using ImageMap = std::unordered_map<VkImage, VKNImageLayout>;
	
		//------------
		//ImageBarrier
		//------------

		struct ImageBarrier
		{
			VkPipelineStageFlags SourceStage		= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			VkPipelineStageFlags DestinationStage	= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			VkImageMemoryBarrier Barrier			= {};
		};

	public:
		LAMBDA_NO_COPY(VKNResourceLayoutTracker);

		VKNResourceLayoutTracker();
		~VKNResourceLayoutTracker() = default;

		void TransitionImage(VkImage image, VkImageAspectFlags aspectFlags, uint32 mipLevels, VkImageLayout toLayout);
		void FlushBarriers(VkCommandBuffer cmdBuffer);

		VkImageLayout GetLayout(VkImage image, uint32 mipLevel) const;
		inline bool NeedsFlush() const { return !m_DefferedBarriers.empty(); };
	private:
		void SyncGlobalLayouts();
		void FinishBarrier(ImageBarrier& barrier);
	private:
		ImageMap m_ResourceLayouts;
		std::vector<ImageBarrier> m_DefferedBarriers;
	private:
		static VKNImageLayout& GetGlobalLayout(VkImage image);
		static bool HasGlobalLayout(VkImage image);
		static void AddGlobalLayout(VkImage image, const VKNImageLayout& layout);
		static void RemoveGlobalLayout(VkImage image);
	private:
		static std::mutex s_GlobalMutex;
		static ImageMap s_GlobalResourceLayouts;
	};
}