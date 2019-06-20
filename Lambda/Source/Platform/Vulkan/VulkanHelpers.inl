namespace Lambda
{
    inline uint32 FindMemoryType(VkPhysicalDevice adapter, uint32 typeFilter, VkMemoryPropertyFlags properties)
    {
        //Get memory properties of the GPU
        VkPhysicalDeviceMemoryProperties memoryProperties = {};
        vkGetPhysicalDeviceMemoryProperties(adapter, &memoryProperties);
        
        //Find suitable type
        for (uint32 i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }
        
        //Return error
        LOG_DEBUG_ERROR("Vulkan: Failed to find a suitable memory type\n");
        return -1;
    }
}
