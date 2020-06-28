VULKAN_SDK_PATH = /home/rpg59/Downloads/1.2.135.0/x86_64
CFLAGS = -std=c++17 -g3 -I$(VULKAN_SDK_PATH)/include -I~/dev/pakages/glfw/include -I./_dep/include  -I./VK_WIN32_TEST_1
LDFLAGS = -lglfw -lvulkan

mt:
	$(MAKE) -j8 vt

vt: ./VK_WIN32_TEST_1/main.cpp
	g++ $(CFLAGS) -o VulkanTest ./VK_WIN32_TEST_1/main.cpp ./VK_WIN32_TEST_1/HellowTrianleApp.cpp $(LDFLAGS)

.PHONY: test clean


test: VulkanTest
	LD_LIBRARY_PATH=${VULKAN_SDK_PATH}/lib VK_LAYER_PATH=$(VULKAN_SDK_PATH)/etc/vulkan/explicit_layer.d ./VulkanTest

clean:
	rm -f VulkanTest

d: 
	gdb (LD_LIBRARY_PATH=${VULKAN_SDK_PATH}/lib VK_LAYER_PATH=$(VULKAN_SDK_PATH)/etc/vulkan/explicit_layer.d ./VulkanTest)
