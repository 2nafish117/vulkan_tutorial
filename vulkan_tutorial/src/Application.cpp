#include "Application.h"

int Application::Startup()
{
	if (glfwInit() != GLFW_TRUE) {
		return 1;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	AppState.Window = glfwCreateWindow(
		AppState.WindowWidth, AppState.WindowHeight, 
		AppState.Name, nullptr, nullptr);

	// initialise vulkan
	{
		VkApplicationInfo applicationInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = AppState.Name,
			.applicationVersion = VK_MAKE_VERSION(0, 1, 0),
			.pEngineName = "Vulkan Tutorial Engine",
			.engineVersion = VK_MAKE_VERSION(0, 1, 0),
			.apiVersion = VK_MAKE_VERSION(1, 1, 0)
		};

		const char* const requiredLayers[] = {
			"",
			""
		};

		std::vector<VkLayerProperties> properties;
		uint32_t propertyCount = 0;
		vkEnumerateInstanceLayerProperties(&propertyCount, nullptr);
		properties.reserve(propertyCount);
		vkEnumerateInstanceLayerProperties(&propertyCount, properties.data());

		uint32_t glfwExtensionsCount = 0;
		glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

		VkInstanceCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.pApplicationInfo = &applicationInfo,
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = 0,
			.enabledExtensionCount = 0,
			.ppEnabledExtensionNames = 0
		};

		VkResult res = vkCreateInstance(&createInfo, AppState.VkState.Allocator, &AppState.VkState.Instance);

	}

	return 0;
}

int Application::MainLoop()
{
	while (!glfwWindowShouldClose(AppState.Window)) {
		glfwPollEvents();
	}

	return 0;
}

int Application::Shutdown()
{
	//vkDestroyInstance(AppState.VkState.Instance, AppState.VkState.Allocator);

	glfwDestroyWindow(AppState.Window);
	glfwTerminate();
    return 0;
}
