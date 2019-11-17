#pragma once

#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <GLFW/glfw3.h>

class HelloTriangleApp
{
private:
	VkInstance instance;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentationQueue;
	VkSurfaceKHR surface;
	GLFWwindow* window = nullptr;

public:
	void run(GLFWwindow* _window);
private:
	void initVulkan();
	void mainLoop();
	void cleanup();
	void createInstance();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSurface();
};

