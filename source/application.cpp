
#define WIDTH 800
#define HEIGHT 600

struct app_t
{
	GLFWwindow* window;
	VkInstance instance;
};

internal_f void
AppInitWindow(app_t *app)
{
	assert(glfwInit());
	
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
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
	glfwDestroyWindow(app->window);
	
	glfwTerminate();
}


internal_f void
AppInitVulkan(app_t *app)
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
	
	
	// Extensions, we are now using glfw for returning the extensions
	// We would normally create an array of const char* and specify the different extensions in here.
	// I think we can query for the compatible devices for this extensions.
	
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	
	create_info.enabledExtensionCount = glfwExtensionCount;
	create_info.ppEnabledExtensionNames = glfwExtensions;
	
	create_info.enabledLayerCount = 0;
	
	
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


global_f void
AppRun(app_t *app)
{
	AppInit(app);
	AppMainLoop(app);
	AppCleanUp(app);
}

