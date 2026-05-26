
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