#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <Windows.h>
#include <vector>
#include <cstring>

#include<GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/vec4.hpp>
#include<glm/mat4x4.hpp>

#include "HelloTriangleApp.h"

#pragma comment(lib, "GLFW/glfw3.lib")
#pragma comment(lib, "vulkan-1.lib")

#define WIDTH 1320
#define HEIGHT 768






int main()
{
	HelloTriangleApp app;


	if (!glfwInit()) {
		//DebugBreak();
		return -1;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = nullptr;
	window = glfwCreateWindow(WIDTH, HEIGHT, "VK_WIN32_TEST_1", nullptr, nullptr);

	if (window == nullptr) {
		//DebugBreak();
		return -1;
	}

	try {
		app.run(window);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 10);
	std::cout << extensionCount << "extensions support" << std::endl;

	for (const auto& extension : extensions) {
		std::cout << "\t" << extension.extensionName << std::endl;
	}

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}


	glfwDestroyWindow(window);
	glfwTerminate();

	std::cout << "HW\n";

	return EXIT_SUCCESS;

}