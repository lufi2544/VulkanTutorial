
#define WIDTH 800
#define HEIGHT 600


internal_f void
AppInitWindow(app_t *app)
{
	assert(glfwInit());
	
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_PLATFORM, GLFW_PLATFORM_WIN32);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	
	app->window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", 0, 0);
	
	glfwShowWindow(app->window);
	glfwFocusWindow(app->window);
}

internal_f void
AppInitVulkan(app_t *app);

internal_f void
AppInitDebugMessenger(app_t *app)
{
	if(!g_enableValidationLayers)
	{
		return;
	}
	
	VkDebugUtilsMessengerCreateInfoEXT create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = DebugCallback;
	create_info.pUserData = app;
	
}


internal_f void
AppInit(app_t *app)
{
	// Memory Init		
	InitArena(&app->arena, Gigabyte(1), malloc(Gigabyte(1)));	
		
	
	AppInitWindow(app);
	AppInitVulkan(app);
}

internal_f void
AppMainLoop(app_t *app)
{
	while(!glfwWindowShouldClose(app->window))
	{
		glfwPollEvents();
	}		
}


internal_f void 
AppCleanUp(app_t *app)
{
	if(g_enableValidationLayers)
	{
		app->extensions_api.debug_utils_destroy(app->instance, app->debug_messenger, 0);
	}
	
	vkDestroyInstance(app->instance, 0);
	glfwDestroyWindow(app->window);
	
	glfwTerminate();
}

internal_f void 
AppGetExtensions(arena_t *arena, const char*** out_extensions, u32 *extensions_num)
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = 0;
		
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	
	
	u32 our_extensions_size = glfwExtensionCount;
	if(g_enableValidationLayers)
	{
		our_extensions_size++;
	}
			
	const char** our_extensions = (const char**)PushSize(arena, sizeof(char*) * our_extensions_size);
	
	BytesCopy(glfwExtensions, our_extensions, sizeof(char*) * glfwExtensionCount);
	
	if(g_enableValidationLayers)
	{
		our_extensions[our_extensions_size - 1] = (const char*)PushSize(arena, c_str_size(VK_EXT_DEBUG_UTILS_EXTENSION_NAME) + 1);
		BytesCopy(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, (void*)our_extensions[our_extensions_size - 1], c_str_size(VK_EXT_DEBUG_UTILS_EXTENSION_NAME) + 1);
		
		printf(" PEPE: %s \n", our_extensions[our_extensions_size - 1]);
	}
	
	*out_extensions = our_extensions;
	*extensions_num = our_extensions_size;
}


internal_f void
AppCheckExtensionsAreAvailable(app_t *app, VkInstanceCreateInfo *info)
{			
	// Extensions, we are now using glfw for returning the extensions
	// We would normally create an array of const char* and specify the different extensions in here.
	// I think we can query for the compatible devices for this extensions.
	
	
	const char** our_extensions = 0;
	u32 our_extensions_size = 0;
	
	AppGetExtensions(&app->arena, &our_extensions, &our_extensions_size);
	
	
	printf("\n");
	printf("\n");
	printf(" =============\n");
	printf("OUR Extensions: \n");
	printf("\n");
	printf("\n");
	
	for(u32 idx = 0; idx < our_extensions_size; ++idx)
	{
		printf("%s \n", our_extensions[idx]);
	}
		
	printf("\n");
	printf("\n");
	
	
	uint32_t extension_count = 0;		
	vkEnumerateInstanceExtensionProperties(NULL, &extension_count, 0);
			
	VkExtensionProperties* properties = (VkExtensionProperties*)PushSize(&app->arena, extension_count * sizeof(VkExtensionProperties));		
	vkEnumerateInstanceExtensionProperties(NULL, &extension_count, properties);	
	
	
	printf("Compatible Extensions: \n");
	printf("\n");
	printf("\n");
	
	for(u32 idx = 0; idx < extension_count; ++idx)
	{
		printf( "%s\n", properties[idx].extensionName);
	}
	printf("\n");
	printf("\n");
	
	for(u32 to_check_idx = 0; to_check_idx < our_extensions_size; ++to_check_idx)
	{
		
		u32 idx = 0;
		while (idx < extension_count)
		{
			if(c_str_compare(our_extensions[to_check_idx], properties[idx].extensionName))
			{
				break;
			}
			
			++idx;
		}
		
		// Not all the extensions are supported
		assert(idx != extension_count);
	}

	
	
	info->enabledExtensionCount = our_extensions_size;
	info->ppEnabledExtensionNames = our_extensions;
	
	printf(" All Extensions available...  \n");	
	printf(" =============\n");		
}



internal_f void 
AppCheckValidationLayersAvailable(app_t *app, VkInstanceCreateInfo *create_info)
{
	SCRATCH(&app->arena);
	
	u32 layer_count = 0;
	vkEnumerateInstanceLayerProperties(&layer_count, 0);
	
	
	VkLayerProperties* available_layers = (VkLayerProperties*)PushSize(temp_arena, sizeof(VkLayerProperties) * layer_count);;
	vkEnumerateInstanceLayerProperties(&layer_count, available_layers);
	
	
	printf("\n");
	printf("\n");
	printf(" =============\n");
	printf("Available Validation Layers: \n");
	printf("\n");
	printf("\n");
	
	for(
		u32 layer_idx = 0;
		layer_idx < layer_count;
		++layer_idx)
	{
		printf(" %s \n", available_layers[layer_idx].layerName);
	}

	
	for(u32 our_layers_idx = 0; our_layers_idx < ArrayCount(g_validation_layers); ++our_layers_idx)
	{
		// our intended validation layers to activate
		u32 available_idx = 0;		
		while(available_idx < layer_count)
		{			
			if(c_str_compare(g_validation_layers[our_layers_idx], available_layers[available_idx].layerName) == true)
			{
				break;
			}
			
			available_idx++;
		}
		
		// check if we have iterated all the posssible validation layers from the system, if so, then no match encountered
		// Not all the validation layers are supported.
		assert(available_idx != layer_count);
	}
	
	if(g_enableValidationLayers)
	{
		create_info->enabledLayerCount = ArrayCount(g_validation_layers);
		create_info->ppEnabledLayerNames = g_validation_layers;
	}
	
	printf(" All validation layers available... \n");
	printf(" =============\n");			
}




internal_f void
VulkanCreateInstance(app_t *app)
{
	// App info 
	
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Hello Triangle";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "No Engine";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;
	
	
	// Creation info 
	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	create_info.enabledLayerCount = 0;	
	
	AppCheckExtensionsAreAvailable(app, &create_info);		
	AppCheckValidationLayersAvailable(app, &create_info);	
					
	VkResult result = vkCreateInstance(&create_info, 0, &app->instance);
	if(result == VK_SUCCESS)
	{
		printf(" Vulkan instance created successfully!! \n");
	}
	else
	{
		printf(" ERROR: Vulkan instance not created correctly...  \n");
	}
	
	
	ExtensionsLoadFunctions(&app->extensions_api, &app->instance);
	
	
	if(g_enableValidationLayers)
	{
		ValidationLayerCreateDebugMessenger(app);
	}
}

internal_f void
AppInitVulkan(app_t *app)
{	
	VulkanCreateInstance(app);	
}

global_f void
AppRun(app_t *app)
{
	AppInit(app);
	AppMainLoop(app);
	AppCleanUp(app);
}
