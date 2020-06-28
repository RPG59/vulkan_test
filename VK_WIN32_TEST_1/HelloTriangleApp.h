#pragma once

#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <GLFW/glfw3.h>
#include <cstring>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentMode;
};

class HelloTriangleApp
{
private:
	VkInstance instance;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentationQueue;
	VkSurfaceKHR surface;
	GLFWwindow *window = nullptr;
	VkSwapchainKHR m_SwapChain;

	VkDebugUtilsMessengerEXT m_debugMessenger;

public:
	void run(GLFWwindow *_window);

private:
	void initVulkan();
	void mainLoop();
	void cleanup();
	void createInstance();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSurface();
	void createSwapChain();

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &);
	VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &);

	void setupDebugMessenger();
};
