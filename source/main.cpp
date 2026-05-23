
#include <stdio.h>
#include "GLFW/glfw3.h"
#include <cassert>

int main(int args_num, char** args)
{	
	printf("Hello Vulkan \n");
	
	if(glfwInit())
	{
		printf(" Initialized GLFW \n");
	}
	
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	
	GLFWmonitor* monitor = 0;
	GLFWwindow* window = glfwCreateWindow(1920, 1080,  "vulkan renderer", glfwGetPrimaryMonitor(), 0);
	
	assert(window != 0);
	
	glfwShowWindow(window);
	
	while(!glfwWindowShouldClose(window))
	{
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();
	
	
	
	return 0;
}