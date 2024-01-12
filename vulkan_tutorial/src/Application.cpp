#include "Application.h"

int Application::Run() 
{
	int startupRet = Startup();
	if (startupRet != 0) 
	{
		return startupRet;
	}

	int mainRet = MainLoop();
	if (mainRet != 0) 
	{
		return mainRet;
	}

	int shutdownRet = Shutdown();
	if (shutdownRet != 0) 
	{
		return shutdownRet;
	}
	return 0;
}

int Application::Startup()
{
	spdlog::set_level(spdlog::level::debug);
	spdlog::set_pattern("[%H:%M:%S] [%n] [%^%L%$] [thread %t] %v");

	spdlog::info("Application Startup");

	//spdlog::trace("test logging");
	//spdlog::info("test logging");
	//spdlog::debug("test logging");
	//spdlog::warn("test logging");
	//spdlog::error("test logging");
	//spdlog::critical("test logging");

	if (glfwInit() != GLFW_TRUE) {
		spdlog::critical("glfwInit Failure");
		return 1;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	AppState.Window = glfwCreateWindow(
		AppState.WindowWidth, AppState.WindowHeight, 
		AppState.Name, nullptr, nullptr);

	// initialise vulkan
	{
		spdlog::info("initialising vulkan");

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

	spdlog::info("Application Startup Complete");
	return 0;
}

int Application::MainLoop()
{
	spdlog::info("Application Mainloop");

	while (!glfwWindowShouldClose(AppState.Window)) {
		glfwPollEvents();
	}

	spdlog::info("Application Mainloop Complete");
	return 0;
}

int Application::Shutdown()
{
	spdlog::info("Application Shutdown");

	vkDestroyInstance(AppState.VkState.Instance, AppState.VkState.Allocator);

	glfwDestroyWindow(AppState.Window);
	glfwTerminate();

	spdlog::info("Application Shutdown Complete");
    return 0;
}
