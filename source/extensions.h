/* date = May 28th 2026 5:08 pm */


#define EXT_DEBUG_UTILS_CREATE(name) VkResult name (VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)

#define EXT_DEBUG_UTILS_DESTROY(name) void name (VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator)

typedef EXT_DEBUG_UTILS_CREATE(ext_debug_utils_create_t);
typedef EXT_DEBUG_UTILS_DESTROY(ext_debug_utils_destroy_t);

struct extensions_api_t
{
	ext_debug_utils_create_t* debug_utils_create;
	ext_debug_utils_destroy_t* debug_utils_destroy;
};


internal_f void
ExtensionsLoadFunctions(extensions_api_t *api, VkInstance *instance)
{
	api->debug_utils_create = (ext_debug_utils_create_t*)vkGetInstanceProcAddr(*instance, "vkCreateDebugUtilsMessengerEXT");
	if(!api->debug_utils_create)
	{
		printf(" EXTENSION FUNCTIONS IMPORT: Debug Utils Create NOT loaded... \n");
	}
	
	
	api->debug_utils_destroy = (ext_debug_utils_destroy_t*)vkGetInstanceProcAddr(*instance, "vkDestroyDebugUtilsMessengerEXT");
	if(!api->debug_utils_destroy)
	{
		printf("EXTENSION FUNCTIONS IMPORT: Debug utils Destroy NOT loaded \n");
	}
}
