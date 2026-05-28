/* date = May 28th 2026 5:10 pm */


struct app_t
{	
	arena_t arena;
	
	GLFWwindow* window;
			
	extensions_api_t extensions_api;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debug_messenger;
	
};