#include "HelloTriangleApp.h"
#include "utils.h"

#define FAILURE(data) (data != VK_SUCCESS)

// #define NDEBUG

// #define DISABLE_VALIDATION_LAYERS

//
//VkResult vkCreateInstance(
//	const VkInstanceCreateInfo* pCreateInfo,
//	const VkAllocationCallbacks* pAllocator,
//	VkInstance* instance
//)
//{
//	if (pCreateInfo == nullptr || instance == nullptr) {
//		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
//		SetConsoleTextAttribute(hConsole, 15);
//		std::cout << "Null pointer passed to required parameter!" << std::endl;
//		return VK_ERROR_INITIALIZATION_FAILED;
//	}
//}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageServity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
) {
	std::cerr << "[VALIDATION LAYER]: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

#ifdef DISABLE_VALIDATION_LAYERS
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char *> validationLayers = {
	"VK_LAYER_KHRONOS_validation"};

bool checkValidationLayerSupport()
{
	uint32_t layerCount;
	auto count_status = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	if (FAILURE(count_status))
		throw std::runtime_error("checkValidationLayerSupport: API Exception");

	std::vector<VkLayerProperties> availableLayers(layerCount);
	auto get_layers_status = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	if (FAILURE(get_layers_status))
		throw std::runtime_error("checkValidationLayerSupport: API Exception");

	// std::cout << "VALIDATION LAYER COUNT :" << layerCount << std::endl;
	for (auto layer : availableLayers)
	{
		std::cout << "Validation layer: " << layer.layerName << std::endl;
	}

	for (const char *layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto &layerProps : availableLayers)
		{
			if (strcmp(layerName, layerProps.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

void HelloTriangleApp::run(GLFWwindow *_window)
{
	window = _window;
	initVulkan();
}

void HelloTriangleApp::initVulkan()
{
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
}

void HelloTriangleApp::mainLoop()
{
}

void HelloTriangleApp::cleanup()
{
	vkDestroyInstance(instance, nullptr);
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
	vkDestroySwapchainKHR(device, m_SwapChain, nullptr);
}

void HelloTriangleApp::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not avaliable");
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for(int i = 0; i < glfwExtensionCount; ++i) {
		std::cout << "Extensions: " << glfwExtensions[i] << std::endl;
	}


	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}
}

void HelloTriangleApp::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	std::multimap<int, VkPhysicalDevice> candidates;

	for (const auto &device : devices)
	{
		int score = rateDeviceSuitability(device);
		if (isDeviceSuitable(device, surface))
		{
			candidates.insert(std::make_pair(score, device));
		}
	}

	if (candidates.size() > 0 && candidates.rbegin()->first > 0)
	{
		physicalDevice = candidates.rbegin()->second;
	}
	else
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

void HelloTriangleApp::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = 0;

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledExtensionNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
}

void HelloTriangleApp::createSurface()
{
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
}

SwapChainSupportDetails HelloTriangleApp::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	bool swapChainAdequeate = false;

	return details;
}

VkSurfaceFormatKHR HelloTriangleApp::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
	for (const auto &availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR HelloTriangleApp::choosePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
	for (const auto &avaliablePresentMode : availablePresentModes)
	{
		if (avaliablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return avaliablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D HelloTriangleApp::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = {640, 480};

		actualExtent.width = std::max(
			capabilities.minImageExtent.width,
			std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(
			capabilities.minImageExtent.height,
			std::min(capabilities.maxImageExtent.height, actualExtent.height));
		return actualExtent;
	}
}

void HelloTriangleApp::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = choosePresentMode(swapChainSupport.presentMode);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1; // Driver ....

	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR cf{};
	cf.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	cf.surface = surface;
	cf.minImageCount = imageCount;
	cf.imageFormat = surfaceFormat.format;
	cf.imageColorSpace = surfaceFormat.colorSpace;
	// cf.imageExtent = extent;
	cf.imageArrayLayers = 1;
	cf.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
	uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	if (indices.graphicsFamily != indices.presentFamily)
	{
		cf.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		cf.queueFamilyIndexCount = 2;
		cf.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		cf.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	cf.preTransform = swapChainSupport.capabilities.currentTransform;
	cf.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	cf.presentMode = presentMode;
	cf.clipped = VK_TRUE;
	cf.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device, &cf, nullptr, &m_SwapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swap chain!");
	}
}

void HelloTriangleApp::setupDebugMessenger()
{
	if(!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT ci{};
	ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	ci.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
	VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	ci.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
	VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	ci.pfnUserCallback = debugCallback;
}
