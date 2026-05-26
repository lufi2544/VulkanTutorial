
#define WIDTH 800
#define HEIGHT 600


typedef uint32_t u32;
typedef uint8_t u8;


#define Kilobyte(x) x * 1024
#define Megabyte(x) Kilobyte(x) * 1024
#define Gigabyte(x) Megabyte(x) * 1024


internal_f u32 
c_str_size(const char* a)
{
	u32 size = 0;
	while(a[size++]);		
	return size - 1;
}

internal_f bool
c_str_compare(const char* _a, const char* _b)
{
	u8* a=(u8*)_a;
	u8* b=(u8*)_b;
	
	u32 a_size = c_str_size(_a);
	u32 b_size = c_str_size(_b);
	
	if(a_size != b_size)
	{
		return false;
	}
	
	
	for(u32 idx = 0; idx < a_size; ++idx)
	{
		if(a[idx] != b[idx])
		{
			return false;
		}
	}
	
	
	return true;
}

global_f void
BytesCopy(void* _s, void* _d, u32 size)
{
	u8* s = (u8*)_s;
	u8* d = (u8*)_d;
	
	for(u32 idx = 0; idx < size; ++idx)
	{
		d[idx] = s[idx];
	}
}

struct arena_t
{
	u8 *data;
	u32 current;
	u32 used;
	u32 size;	
};


internal_f void*
PushSize(arena_t *arena, u32 size)
{
	assert(arena->current + size < arena->size);
	
	u8* memory = arena->data + arena->current;
	arena->current += size;
	arena->used += size;
	
	return memory;
}

struct scratch_t
{	
	arena_t* parent;
	arena_t arena;
	u32 arena_prev;
	
	
	~scratch_t()
	{
		parent->current = arena_prev;		
	}

};

global_f void
InitArena(arena_t *arena, u32 size, void* memory)
{
	arena->data = (u8*)memory;
	arena->current = 0;
	arena->used = 0;
	arena->size = size;
}


internal_f arena_t* 
ScratchBegin(arena_t *arena, scratch_t *scratch)
{			
	scratch->arena_prev = arena->current;	
	scratch->arena.size = arena->size - arena->used;
	scratch->arena.used = 0;
	
	assert(scratch->arena.size > 0);
	
	scratch->parent = arena;
	
	InitArena(&scratch->arena, arena->size - arena->used, arena->data + arena->current);
	
	return &scratch->arena;
}



#define SCRATCH(arena) \
scratch_t scratch; \
arena_t* temp_arena = ScratchBegin(arena, &scratch); \


struct app_t
{	
	arena_t arena;
	
	GLFWwindow* window;
	VkInstance instance;
};

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
	vkDestroyInstance(app->instance, 0);
	glfwDestroyWindow(app->window);
	
	glfwTerminate();
}




internal_f void
AppCheckExtensionsAreAvailable(app_t *app, VkInstanceCreateInfo *info)
{			
	// Extensions, we are now using glfw for returning the extensions
	// We would normally create an array of const char* and specify the different extensions in here.
	// I think we can query for the compatible devices for this extensions.
	
	
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = 0;
		
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	
	
	u32 our_extensions_size = glfwExtensionCount;
	if(g_enableValidationLayers)
	{
		our_extensions_size++;
	}
			
	const char** our_extensions = (const char**)PushSize(&app->arena, sizeof(char*) * our_extensions_size);
	
	BytesCopy(glfwExtensions, our_extensions, sizeof(char*) * glfwExtensionCount);
	
	if(g_enableValidationLayers)
	{
		our_extensions[our_extensions_size - 1] = (const char*)PushSize(&app->arena, c_str_size(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));
		BytesCopy(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, (void*)our_extensions[our_extensions_size - 1], c_str_size(VK_EXT_DEBUG_UTILS_EXTENSION_NAME) + 1);
		
		printf(" PEPE: %s \n", our_extensions[our_extensions_size - 1]);
	}
	
	
	uint32_t extension_count = 0;		
	vkEnumerateInstanceExtensionProperties(NULL, &extension_count, 0);
			
	SCRATCH(&app->arena);
	VkExtensionProperties* properties = (VkExtensionProperties*)PushSize(temp_arena, extension_count * sizeof(VkExtensionProperties));		
	vkEnumerateInstanceExtensionProperties(NULL, &extension_count, properties);	
	
	
	printf("\n");
	printf("\n");
	printf(" =============\n");
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
		while (idx++ < extension_count)
		{
			if(c_str_compare(our_extensions[to_check_idx], properties[idx].extensionName))
			{
				break;
			}
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
