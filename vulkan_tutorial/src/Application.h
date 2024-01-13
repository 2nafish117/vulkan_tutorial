#pragma once

#include "Common.h"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

struct ApplicationState {
	
	GLFWwindow* Window = nullptr;
	const uint32_t WindowWidth = 800;
	const uint32_t WindowHeight = 600;
	const char* const Name = "Vulkan Tutorial";
	
	struct VulkanState {
		VkInstance Instance = 0;
		VkAllocationCallbacks* Allocator = nullptr;
		VkDebugUtilsMessengerEXT DebugMessenger = 0;
	} VkState;
};

class Application
{
public:
	Application() {
	}

	virtual ~Application() {

	}

	int Run();
public:

private:
	// application lifecycle
	int Startup();
	int MainLoop();
	int Shutdown();

	// vulkan stuff
	int InitVulkan();
	int DeInitVulkan();

	int CreateInstance();
	bool CheckRequiredLayerSupport(std::vector<const char*>& requiredLayers);
	bool CheckRequiredExtensionSupport(std::vector<const char*>& requiredExtensions);

private:
	ApplicationState AppState;
};

