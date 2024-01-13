#include "Application.h"

static VkBool32 VKAPI_PTR VkDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageTypes,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	static const char* MessageTypeStr[] = {
		"General",
		"Validation",
		"Performance",
	};

	// @TODO: see what else is important in this struct
	const char* message = pCallbackData->pMessage;

	switch (messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		spdlog::trace("{} {}", MessageTypeStr[messageTypes], message);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		spdlog::info("{} {}", MessageTypeStr[messageTypes], message);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		spdlog::warn("{} {}", MessageTypeStr[messageTypes], message);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		spdlog::error("{} {}", MessageTypeStr[messageTypes], message);
		break;
	default:
		spdlog::error("Unknown vulkan error severity {}", static_cast<uint32_t>(messageSeverity));
		break;
	}
	
	return VK_FALSE;
}

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
	spdlog::set_pattern("[%H:%M:%S] [%^%L%$] [thread %t] %v");

	spdlog::info("Application Startup");

	//spdlog::trace("test logging");
	//spdlog::info("test logging");
	//spdlog::debug("test logging");
	//spdlog::warn("test logging");
	//spdlog::error("test logging");
	//spdlog::critical("test logging");

	if (glfwInit() != GLFW_TRUE) 
	{
		spdlog::critical("glfwInit Failure");
		return 1;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	AppState.Window = glfwCreateWindow(
		AppState.WindowWidth, AppState.WindowHeight, 
		AppState.Name, nullptr, nullptr);

	if (int result = InitVulkan(); result != 0) 
	{
		return result;
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

	DeInitVulkan();

	glfwDestroyWindow(AppState.Window);
	glfwTerminate();

	spdlog::info("Application Shutdown Complete");
    return 0;
}

int Application::InitVulkan()
{
	spdlog::info("Initialising vulkan");

	if (CreateInstance() != 0)
	{
		return 1;
	}

	return 0;
}

int Application::DeInitVulkan()
{
	spdlog::info("Deinitialise vulkan");

	auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(AppState.VkState.Instance, "vkDestroyDebugUtilsMessengerEXT");
	assert(vkDestroyDebugUtilsMessengerEXT);

	vkDestroyDebugUtilsMessengerEXT(AppState.VkState.Instance, AppState.VkState.DebugMessenger, AppState.VkState.Allocator);

	vkDestroyInstance(AppState.VkState.Instance, AppState.VkState.Allocator);
	
	spdlog::info("Deinitialise vulkan finished");
	return 0;
}

int Application::CreateInstance()
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

	// @TODO: add mandatory, optional flag to check
	std::vector<const char*> requiredLayers = {
		"VK_LAYER_RENDERDOC_Capture"
	};
	if (!CheckRequiredLayerSupport(requiredLayers))
	{
		return 1;
	}
	spdlog::info("Found all required layers");

	// @TODO: add mandatory, optional flag to check
	std::vector<const char*> requiredExtensions = {
		"VK_EXT_debug_utils",
	};
	if (!CheckRequiredExtensionSupport(requiredExtensions))
	{
		return 1;
	}
	spdlog::info("Found all required extensions");

	VkInstanceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pApplicationInfo = &applicationInfo,
		.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
		.ppEnabledLayerNames = requiredLayers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
		.ppEnabledExtensionNames = requiredExtensions.data()
	};

	VkResult result = vkCreateInstance(&createInfo, AppState.VkState.Allocator, &AppState.VkState.Instance);
	if (result != VK_SUCCESS)
	{
		spdlog::critical("Vulkan Instance Creation Failed with result {}", (int)result);
		return static_cast<int>(result);
	}

	auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(AppState.VkState.Instance, "vkCreateDebugUtilsMessengerEXT");
	assert(vkCreateDebugUtilsMessengerEXT);

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.pNext = nullptr,
		.flags = 0,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = VkDebugCallback,
		.pUserData = nullptr
	};

	vkCreateDebugUtilsMessengerEXT(
		AppState.VkState.Instance, 
		&debugCreateInfo, 
		AppState.VkState.Allocator,
		&AppState.VkState.DebugMessenger);

	return 0;
}


bool Application::CheckRequiredLayerSupport(std::vector<const char*>& requiredLayers)
{
	spdlog::info("Enumerating vulkan instance layer properties");

	uint32_t propertyCount = 0;
	vkEnumerateInstanceLayerProperties(&propertyCount, nullptr);

	spdlog::info("Found {} layer properties", propertyCount);

	std::vector<VkLayerProperties> availableLayers(propertyCount);
	vkEnumerateInstanceLayerProperties(&propertyCount, availableLayers.data());

	for (const VkLayerProperties& prop : availableLayers)
	{
		spdlog::info("layerName: {}\n description: {}\n implVersion: {}\n specVersion: {}\n",
			prop.layerName,
			prop.description,
			prop.implementationVersion,
			prop.specVersion);
	}

	bool foundAllRequiredLayers = true;
	for (const char* const layer : requiredLayers)
	{
		typedef std::vector<VkLayerProperties>::iterator Iterator;
		Iterator found = std::find_if(
			availableLayers.begin(),
			availableLayers.end(),
			[layer](const VkLayerProperties& availableLayer)
		{
			return std::strcmp(layer, availableLayer.layerName) == 0;
		});

		if (found == availableLayers.end())
		{
			foundAllRequiredLayers = false;
			spdlog::critical("Cant find required vulkan layer {}", layer);
		}
	}

	return foundAllRequiredLayers;
}

bool Application::CheckRequiredExtensionSupport(std::vector<const char*>& requiredExtensions)
{
	uint32_t glfwExtensionsCount = 0;
	const char** glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

	spdlog::info("Glfw required extensions:");
	for (uint32_t i = 0; i < glfwExtensionsCount; ++i)
	{
		spdlog::info("{}", glfwRequiredExtensions[i]);
		requiredExtensions.emplace_back(glfwRequiredExtensions[i]);
	}

	spdlog::info("Required Extensions: ");
	for (const char* const ext : requiredExtensions) 
	{
		spdlog::info("{}", ext);
	}

	// enumerate extension properties
	spdlog::info("Enumerating vulkan instance extension properties");
	uint32_t propertyCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(propertyCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, availableExtensions.data());

	for (const VkExtensionProperties& prop : availableExtensions)
	{
		spdlog::info("\nextName: {}\nspecVersion: {}", prop.extensionName, prop.specVersion);
	}

	bool foundAllRequiredExt = true;
	for (const char* const ext : requiredExtensions)
	{
		typedef std::vector<VkExtensionProperties>::const_iterator Iterator;
		Iterator found = std::find_if(
			availableExtensions.begin(),
			availableExtensions.end(),
			[ext](const VkExtensionProperties& availableExt) 
		{ 
			return std::strcmp(ext, availableExt.extensionName) == 0; 
		});

		if (found == availableExtensions.end()) 
		{
			foundAllRequiredExt = false;
			spdlog::critical("Cant find required vulkan extension {}", ext);
		}
	}

	return foundAllRequiredExt;
}
