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
		VkAllocationCallbacks* Allocator = nullptr;
		VkInstance Instance = 0;
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
	int Startup();
	int MainLoop();
	int Shutdown();

private:
	ApplicationState AppState;
};

