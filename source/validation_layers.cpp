
/** 
 * Vulkan does not check everything by default, by everything I mean: proper creation of the instances, memory allocation, correct params injection, etc. 
 * That is why Vulkan has the validation layers, in this case this layers are not enabled by default and let you to debug these scenarios correctly.
 * 
 */


global_f const char* g_validation_layers[] =
{
	"VK_LAYER_KHRONOS_validation"
};


global_f const bool g_enableValidationLayers = true;

// EXTENSION API CALLBACKS



internal_f VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
														VkDebugUtilsMessageSeverityFlagBitsEXT _message_severity,
														VkDebugUtilsMessageTypeFlagsEXT _message_type,
														const VkDebugUtilsMessengerCallbackDataEXT* _callback_data,
														void *_user_ata
														)
{
	
	if(_message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{		
		printf(" Validation Layer: %s \n", _callback_data->pMessage);
	}
	
	return VK_FALSE;
}


internal_f void 
ValidationLayerCreateDebugMessenger(app_t *app)
{
	VkDebugUtilsMessengerCreateInfoEXT create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = DebugCallback;
	create_info.pUserData = app;
	
	
	app->extensions_api.debug_utils_create(app->instance, &create_info, 0, &app->debug_messenger);
}
