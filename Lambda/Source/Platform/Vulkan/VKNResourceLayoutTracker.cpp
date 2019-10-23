#include "LambdaPch.h"
#include "VKNResourceLayoutTracker.h"

namespace Lambda
{
	//------------------------
	//VKNResourceLayoutTracker
	//------------------------

	std::mutex VKNResourceLayoutTracker::s_GlobalMutex										= std::mutex();
	VKNResourceLayoutTracker::ImageMap VKNResourceLayoutTracker::s_GlobalResourceLayouts	= VKNResourceLayoutTracker::ImageMap();


	VKNResourceLayoutTracker::VKNResourceLayoutTracker()
		: m_ResourceLayouts(),
		m_DefferedBarriers()
	{
	}


	void VKNResourceLayoutTracker::TransitionImage(VkImage image, VkImageAspectFlags aspectFlags, uint32 mipLevel, VkImageLayout toLayout)
	{
		LAMBDA_ASSERT(image != VK_NULL_HANDLE);

		//Setup barrier
		ImageBarrier barrier = {};
		barrier.Barrier.sType							= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.Barrier.srcQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
		barrier.Barrier.dstQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
		barrier.Barrier.image							= image;
		barrier.Barrier.newLayout						= toLayout;
		barrier.Barrier.subresourceRange.aspectMask		= aspectFlags;
		barrier.Barrier.srcAccessMask					= 0;
		barrier.Barrier.dstAccessMask					= 0;
		barrier.Barrier.subresourceRange.baseArrayLayer = 0;
		barrier.Barrier.subresourceRange.layerCount		= 1;
		//Set stages
		barrier.SourceStage			= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		barrier.DestinationStage	= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

		//Setup barrier based on subresources
		const auto& currentLayout = m_ResourceLayouts.find(image);
		if (currentLayout != m_ResourceLayouts.end())
		{
			if (mipLevel == VK_REMAINING_MIP_LEVELS)
			{
				if (!currentLayout->second.SubLayouts.empty())
				{
					//If we have transitioned an image-subresource (i.e a miplevel) before we need to know what layout the rest of 
					//the resource is in so that we can transition each subresource into this layout. Then we can tranistion the whole
					//resource into the layout that we want
					for (auto& sublayout : currentLayout->second.SubLayouts)
					{
						//Setup new layout
						barrier.Barrier.oldLayout						= sublayout.second;
						barrier.Barrier.newLayout						= currentLayout->second.Layout;
						barrier.Barrier.subresourceRange.baseMipLevel	= sublayout.first;
						barrier.Barrier.subresourceRange.levelCount		= 1;

						//Add barrier
						FinishBarrier(barrier);
						m_DefferedBarriers.emplace_back(barrier);
					}

					//Set layout for all resources since all have the same layout
					currentLayout->second.SubLayouts.clear();
				}

				//Avoid unnecceassary barrier
				if (currentLayout->second.Layout != toLayout)
				{
					//Setup new layout
					barrier.Barrier.oldLayout						= currentLayout->second.Layout;
					barrier.Barrier.newLayout						= toLayout;
					barrier.Barrier.subresourceRange.baseMipLevel	= 0;
					barrier.Barrier.subresourceRange.levelCount		= VK_REMAINING_MIP_LEVELS;
					currentLayout->second.Layout = toLayout;

					//Add barrier
					FinishBarrier(barrier);
					m_DefferedBarriers.emplace_back(barrier);
				}
			}
			else
			{
				//If we are trying to change layout into one we already are in
				const auto& sublayout = currentLayout->second.SubLayouts.find(mipLevel);
				if (!(sublayout == currentLayout->second.SubLayouts.end() && currentLayout->second.Layout == toLayout))
				{
					//Make sure we are not setting a state we are already in
					if (currentLayout->second.SubLayouts[mipLevel] != toLayout)
					{
						//Setup new layout
						barrier.Barrier.subresourceRange.baseMipLevel	= mipLevel;
						barrier.Barrier.subresourceRange.levelCount		= 1;
						if (sublayout == currentLayout->second.SubLayouts.end())
							barrier.Barrier.oldLayout = currentLayout->second.Layout;
						else
							barrier.Barrier.oldLayout = currentLayout->second.SubLayouts[mipLevel];

						//If we change the layout to the same as the rest we remove the sublayout
						if (toLayout == currentLayout->second.Layout)
							currentLayout->second.SubLayouts.erase(mipLevel);
						else
							currentLayout->second.SubLayouts[mipLevel] = toLayout;

						//Add barrier
						FinishBarrier(barrier);
						m_DefferedBarriers.emplace_back(barrier);
					}
				}
			}
		}
		else
		{
			//Setup a new layout
			barrier.Barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED; //This layout is undefined
			if (mipLevel == VK_REMAINING_MIP_LEVELS)
			{
				m_ResourceLayouts[image].Layout					= toLayout;
				barrier.Barrier.subresourceRange.baseMipLevel	= 0;
				barrier.Barrier.subresourceRange.levelCount		= VK_REMAINING_MIP_LEVELS;
			}
			else
			{
				m_ResourceLayouts[image].SubLayouts[mipLevel]	= toLayout;
				barrier.Barrier.subresourceRange.baseMipLevel	= mipLevel;
				barrier.Barrier.subresourceRange.levelCount		= 1;
			}

			//Add barrier
			FinishBarrier(barrier);
			m_DefferedBarriers.emplace_back(barrier);
		}
	}
	

	void VKNResourceLayoutTracker::FlushBarriers(VkCommandBuffer cmdBuffer)
	{
		LAMBDA_ASSERT(cmdBuffer != VK_NULL_HANDLE);

		//Lock global states
		std::lock_guard<std::mutex> lock(s_GlobalMutex);

		//Make sure the states are still valid
		std::vector<ImageBarrier> commitedBarriers;
		commitedBarriers.reserve(m_DefferedBarriers.size());
		for (auto& barrier : m_DefferedBarriers)
		{
			//Do we have a global state
			auto globalState = s_GlobalResourceLayouts.find(barrier.Barrier.image);
			if (globalState != s_GlobalResourceLayouts.end())
			{
				if (barrier.Barrier.subresourceRange.levelCount == VK_REMAINING_MIP_LEVELS)
				{
					if (!globalState->second.SubLayouts.empty())
					{
						//If we have a global state that contains sublayouts, we have had a local layout that thinks that
						//the whole resource has the same state. We need to add barriers to fix this.
						for (auto& sublayout : globalState->second.SubLayouts)
						{
							//Setup new layout
							ImageBarrier newBarrier = barrier;
							newBarrier.Barrier.oldLayout						= sublayout.second;
							newBarrier.Barrier.newLayout						= globalState->second.Layout;
							newBarrier.Barrier.subresourceRange.baseMipLevel	= sublayout.first;
							newBarrier.Barrier.subresourceRange.levelCount		= 1;

							//Add barrier
							FinishBarrier(barrier);
							commitedBarriers.emplace_back(barrier);
						}

						//Remove the gloabl sublayouts
						globalState->second.SubLayouts.clear();
					}

					//If barrier does not equal the global state we need to update the old layout
					if (globalState->second.Layout != barrier.Barrier.oldLayout)
					{
						barrier.Barrier.oldLayout = globalState->second.Layout;
						FinishBarrier(barrier);
					}

					//Commit barrier if the state is not the same
					if (barrier.Barrier.oldLayout != barrier.Barrier.newLayout)
					{
						commitedBarriers.emplace_back(barrier);
						globalState->second.Layout = barrier.Barrier.newLayout;
					}
				}
				else
				{
					//Are we are trying to change layout into one we already are in
					const auto& sublayout = globalState->second.SubLayouts.find(barrier.Barrier.subresourceRange.baseMipLevel);
					if (sublayout != globalState->second.SubLayouts.end())
					{
						//Make sure the oldstate matches the actual oldstate
						if (barrier.Barrier.oldLayout != sublayout->second)
						{
							barrier.Barrier.oldLayout = sublayout->second;
							FinishBarrier(barrier);
						}
					}
					else if (barrier.Barrier.oldLayout != globalState->second.Layout)
					{
						//If sublayout does not exist globaly we need to make sure that old layout is the current layout of the resource
						barrier.Barrier.oldLayout = globalState->second.Layout;
						FinishBarrier(barrier);
					}

					//Commit barrier if the state is not the same
					if (barrier.Barrier.oldLayout != barrier.Barrier.newLayout)
					{
						commitedBarriers.emplace_back(barrier);
						if (barrier.Barrier.newLayout != globalState->second.Layout)
							globalState->second.SubLayouts[barrier.Barrier.subresourceRange.baseMipLevel] = barrier.Barrier.newLayout;
						else
							globalState->second.SubLayouts.erase(barrier.Barrier.subresourceRange.baseMipLevel);
					}
				}
			}
			else
			{
				//If the resource does not currently have a global layout, it must be new
				commitedBarriers.emplace_back(barrier);
			}
		}
		
		//Don't do if no barriers
		if (!commitedBarriers.empty())
		{
			for (auto& barrier : commitedBarriers)
				vkCmdPipelineBarrier(cmdBuffer, barrier.SourceStage, barrier.DestinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier.Barrier);

			m_DefferedBarriers.clear();
		}
	}


	void VKNResourceLayoutTracker::FinishBarrier(ImageBarrier& barrier)
	{
		//Set source- mask and stage based on the layouts
		switch (barrier.Barrier.oldLayout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			barrier.Barrier.srcAccessMask = 0;
			barrier.SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			break;
		case VK_IMAGE_LAYOUT_GENERAL:
			barrier.Barrier.srcAccessMask = 0;
			barrier.SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			barrier.Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.SourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			barrier.Barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.SourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			barrier.Barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.SourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			break;
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
			barrier.Barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			barrier.SourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			break;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			barrier.Barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			barrier.SourceStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			break;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			barrier.Barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.SourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			break;
		default:
			LOG_DEBUG_ERROR("Vulkan: Unsupported src-image layout transition '%d'\n", barrier.Barrier.oldLayout);
			break;
		}

		//Set destination- mask and stage based on the layouts
		switch (barrier.Barrier.newLayout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			LOG_DEBUG_ERROR("Vulkan: Cannot transition to undefined\n");
			break;
		case VK_IMAGE_LAYOUT_GENERAL:
			barrier.Barrier.dstAccessMask = 0;
			barrier.DestinationStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			barrier.Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.DestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			barrier.Barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.DestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			barrier.Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.DestinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			break;
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
			barrier.Barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			barrier.DestinationStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			break;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			barrier.Barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			barrier.DestinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			break;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			barrier.Barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.DestinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			break;
		default:
			LOG_DEBUG_ERROR("Vulkan: Unsupported dst-image layout transition '%d'\n", barrier.Barrier.newLayout);
			break;
		}
	}


	VkImageLayout VKNResourceLayoutTracker::GetLayout(VkImage image, uint32 mipLevel) const
	{
		LAMBDA_ASSERT(image != VK_NULL_HANDLE);

		//Get layout
		auto pair = m_ResourceLayouts.find(image);
		if (pair != m_ResourceLayouts.end())
		{
			//Return current layout
			auto& state = pair->second;
			return state.SubLayouts.empty() ? state.Layout : state.SubLayouts[mipLevel];
		}

		return VK_IMAGE_LAYOUT_UNDEFINED;
	}


	VKNImageLayout& VKNResourceLayoutTracker::GetGlobalLayout(VkImage image)
	{
		LAMBDA_ASSERT(image != VK_NULL_HANDLE);

		//Lock global states
		std::lock_guard<std::mutex> lock(s_GlobalMutex);

		//Return the layout
		auto resourceLayout = s_GlobalResourceLayouts.find(image);
		return resourceLayout->second;
	}


	bool VKNResourceLayoutTracker::HasGlobalLayout(VkImage image)
	{
		LAMBDA_ASSERT(image != VK_NULL_HANDLE);

		//Lock global states
		std::lock_guard<std::mutex> lock(s_GlobalMutex);

		//Does the resource have a layout
		auto resourceLayout = s_GlobalResourceLayouts.find(image);
		return resourceLayout != s_GlobalResourceLayouts.end();
	}


	void VKNResourceLayoutTracker::AddGlobalLayout(VkImage image, const VKNImageLayout& layout)
	{
		LAMBDA_ASSERT(image != VK_NULL_HANDLE);

		//Lock global states
		std::lock_guard<std::mutex> lock(s_GlobalMutex);

		//Add a new globalstate
		const auto& resourceLayout = s_GlobalResourceLayouts.find(image);
		if (resourceLayout == s_GlobalResourceLayouts.end())
			resourceLayout->second = layout;
	}


	void VKNResourceLayoutTracker::RemoveGlobalLayout(VkImage image)
	{
		LAMBDA_ASSERT(image != VK_NULL_HANDLE);
		
		//Lock global states and erase
		std::lock_guard<std::mutex> lock(s_GlobalMutex);
		s_GlobalResourceLayouts.erase(image);
	}
}
