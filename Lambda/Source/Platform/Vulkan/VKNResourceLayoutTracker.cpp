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
		auto& currentLayout = m_ResourceLayouts.find(image);
		if (currentLayout != m_ResourceLayouts.end())
		{
			if (mipLevel == VK_REMAINING_MIP_LEVELS)
			{
				if (!currentLayout->second.SubLayouts.empty())
				{
					//If we have transitioned an image-subresource before we need to know what layout the rest of the resource is in.
					//because we transition each
					//subresource and then add a final barrier that tranitions all resources. This greatly simplifies how to 
					//handle the resourcestates. Since newLayout cannot be VK_IMAGE_LAYOUT_UNDEFINED, images needs to be in
					//a known state when being transitioned. This means that VK_IMAGE_LAYOUT_UNDEFINED should be avoided at all
					//cost. 
					VkImageLayout firstLayout = currentLayout->second.SubLayouts[startMipLevel];
					VkImageLayout previousLayout = firstLayout;
					barrier.Barrier.subresourceRange.levelCount = 0;

					//Set all subresource states to the same as the first
					uint32 endMipLevel = startMipLevel + numMipLevels;
					for (uint32 i = startMipLevel; i < endMipLevel; i++)
					{
						//Is this the same layout as the last, we add a barrier
						if (currentLayout->second.SubLayouts[i] != previousLayout)
						{
							//Add barrier
							if (previousLayout != firstLayout)
							{
								FinishBarrier(barrier);
								m_DefferedBarriers.emplace_back(barrier);
							}

							//Check for this layout
							previousLayout = currentLayout->second.SubLayouts[i];
							barrier.Barrier.oldLayout = previousLayout;
							barrier.Barrier.subresourceRange.baseMipLevel = i;
							barrier.Barrier.subresourceRange.levelCount = 1;
						}
						else
						{
							currentLayout->second.SubLayouts[i] = layout;
							barrier.Barrier.subresourceRange.levelCount++;
						}
					}

					//Setup initial barrier

					barrier.Barrier.oldLayout = previousLayout;


					//Add barrier if layouts are not the same
					if (barrier.Barrier.subresourceRange.levelCount > 0 && barrier.Barrier.oldLayout != layout)
					{
						//Add to barriers
						FinishBarrier(barrier);
						m_DefferedBarriers.emplace_back(barrier);
					}

					//Set layout for all resources since all have the same layout
					currentLayout->second.Layout = toLayout;
					currentLayout->second.SubLayouts.clear();
				}
				else
				{
					//Avoid unnecceassary barrier
					if (currentLayout->second.Layout != toLayout)
					{
						//Setup new layout
						barrier.Barrier.oldLayout						= currentLayout->second.Layout;
						barrier.Barrier.subresourceRange.baseMipLevel	= 0;
						barrier.Barrier.subresourceRange.levelCount		= VK_REMAINING_MIP_LEVELS;
						currentLayout->second.Layout = toLayout;

						//Add barrier
						FinishBarrier(barrier);
						m_DefferedBarriers.emplace_back(barrier);
					}
				}
			}
			else
			{
				if (currentLayout->second.SubLayouts[mipLevel] != toLayout)
				{
					//Setup new layout
					barrier.Barrier.oldLayout						= currentLayout->second.Layout;
					barrier.Barrier.subresourceRange.baseMipLevel	= mipLevel;
					barrier.Barrier.subresourceRange.levelCount		= 1;
					currentLayout->second.Layout = toLayout;

					//Add barrier
					FinishBarrier(barrier);
					m_DefferedBarriers.emplace_back(barrier);
				}
			}
		}
		else
		{
			//Setup a new layout
			if (mipLevel == VK_REMAINING_MIP_LEVELS)
				m_ResourceLayouts[image].Layout = toLayout;
			else
				m_ResourceLayouts[image].SubLayouts[mipLevel] = toLayout;

			//This layout is undefined
			barrier.Barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.Barrier.newLayout = toLayout;

			//Add barrier
			FinishBarrier(barrier);
			m_DefferedBarriers.emplace_back(barrier);
		}
	}
	

	void VKNResourceLayoutTracker::FlushBarriers(VkCommandBuffer cmdBuffer)
	{
		LAMBDA_ASSERT(cmdBuffer != VK_NULL_HANDLE);
		std::lock_guard<std::mutex> lock(s_GlobalMutex);

		//Make sure the states are still valid
		std::vector<ImageBarrier> commitedBarriers;
		commitedBarriers.reserve(m_DefferedBarriers.size());
		for (auto& barrier : m_DefferedBarriers)
		{
			//Check with global layout
			auto globalState = s_GlobalResourceLayouts.find(barrier.Barrier.image);
			if (globalState != s_GlobalResourceLayouts.end())
			{
				uint32 baseMipLevel = barrier.Barrier.subresourceRange.baseMipLevel;
				uint32 levelCount = barrier.Barrier.subresourceRange.levelCount;
				if (baseMipLevel == 0 && levelCount == VK_REMAINING_MIP_LEVELS)
				{
					if (globalState->second.SubLayouts.empty())
					{
						if (globalState->second.Layout != barrier.Barrier.oldLayout)
						{
							//If barrier does not equal the global state we need to update the old layout
							barrier.Barrier.oldLayout = globalState->second.Layout;
							FinishBarrier(barrier);
						}

						//Commit barrier
						commitedBarriers.emplace_back(barrier);
					}
					else
					{
						for (auto& subLayout : globalState->second.SubLayouts)
						{
							//Is this the same layout as the last, we add a barrier
							/*if (subLayout != previousLayout)
							{
								//Add barrier
								if (previousLayout != layout)
								{
									FinishBarrier(barrier);
									m_DefferedBarriers.emplace_back(barrier);
								}

								//Check for this layout
								previousLayout = subLayout;
								barrier.ImageBarrier.oldLayout = previousLayout;
								barrier.ImageBarrier.subresourceRange.baseMipLevel += barrier.ImageBarrier.subresourceRange.levelCount;
								barrier.ImageBarrier.subresourceRange.levelCount = 1;
							}
							else
							{
								//Add one mipbarrier
								barrier.ImageBarrier.subresourceRange.levelCount++;
							}*/
						}
					}
				}
				else
				{
					uint32 endMipLevel = baseMipLevel + levelCount;
					for (uint32 i = baseMipLevel; i < endMipLevel; i++)
					{
						//if (globalState->second.SubLayouts)
					}

					/*if (globalState->second.SubLayouts[barrier.Transition.Subresource] != barrier.Transition.StateBefore)
						barrier.Transition.StateBefore = globalState->second.State;*/

					//Setup initial barrier
					VkImageLayout layoutCurrentlyChecked = globalState->second.SubLayouts[baseMipLevel];

					//Set all subresource states to the correct one
					for (uint32 i = baseMipLevel; i < endMipLevel; i++)
					{
						//Is this the same layout as the last, we add a barrier
						/*if (currentLayout.SubLayouts[i] != previousLayout)
						{
							//If this is the first one we set count to 1
							if (barrier.ImageBarrier.subresourceRange.levelCount == 0)
								barrier.ImageBarrier.subresourceRange.levelCount = 1;

							//Add barrier
							FinishBarrier(barrier);

							//Check for this layout
							previousLayout = currentLayout.SubLayouts[i];
							barrier.ImageBarrier.oldLayout = previousLayout;
							barrier.ImageBarrier.subresourceRange.baseMipLevel = i;
							barrier.ImageBarrier.subresourceRange.levelCount = 0;
						}

						//If layout is not correct add a barrier
						if (previousLayout != layout)
						{
							currentLayout.SubLayouts[i] = layout;
							barrier.ImageBarrier.subresourceRange.levelCount++;
						}*/
					}

					//Add barrier if layouts are not the same
					if (barrier.Barrier.subresourceRange.levelCount > 0)
						FinishBarrier(barrier);

				}

				//Remove barrier if same
				if (barrier.Barrier.oldLayout == barrier.Barrier.newLayout)
				{
					barrier = m_DefferedBarriers.back();
					m_DefferedBarriers.pop_back();
				}
			}
			else
			{
				//If barrier does not exist in global state commit anyway
				commitedBarriers.emplace_back(barrier);
			}
		}
		
		//Don't do if no barriers
		if (!commitedBarriers.empty())
		{
			for (auto& barrier : commitedBarriers)
				vkCmdPipelineBarrier(cmdBuffer, barrier.SourceStage, barrier.DestinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier.Barrier);

			m_DefferedBarriers.clear();
			SyncGlobalLayouts();
		}
	}


	void VKNResourceLayoutTracker::SyncGlobalLayouts()
	{
		for (auto& iter : m_ResourceLayouts)
		{
			auto& globalState = s_GlobalResourceLayouts[iter.first];
			globalState = iter.second;
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

		//Return the layout
		auto resourceLayout = s_GlobalResourceLayouts.find(image);
		return resourceLayout->second;
	}


	bool VKNResourceLayoutTracker::HasGlobalLayout(VkImage image)
	{
		LAMBDA_ASSERT(image != VK_NULL_HANDLE);

		//Does the resource have a layout
		auto resourceLayout = s_GlobalResourceLayouts.find(image);
		return resourceLayout != s_GlobalResourceLayouts.end();
	}


	void VKNResourceLayoutTracker::AddGlobalLayout(VkImage image, const VKNImageLayout& layout)
	{
		LAMBDA_ASSERT(image != VK_NULL_HANDLE);

		//Add a new globalstate
		auto& resourceLayout = s_GlobalResourceLayouts.find(image);
		if (resourceLayout == s_GlobalResourceLayouts.end())
			resourceLayout->second = layout;
	}


	void VKNResourceLayoutTracker::RemoveGlobalLayout(VkImage image)
	{
		LAMBDA_ASSERT(image != VK_NULL_HANDLE);

		std::lock_guard<std::mutex> lock(s_GlobalMutex);
		s_GlobalResourceLayouts.erase(image);
	}
}
