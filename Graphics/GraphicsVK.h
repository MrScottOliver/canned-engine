#ifndef _GRAPHICSVK_H_
#define _GRAPHICSVK_H_

#include <vector>
#include <list>

#include <vulkan/vulkan.hpp>

#include "Logging/Log.h"
#include "Maths/Vector.h"

#include "Graphics/VulkanAllocation.h"
#include "Graphics/VulkanStaging.h"

#include "System/Debug.h"

#pragma warning( push )
#pragma warning( disable : 4003 ) //v2.1.1 has 100s of warnings relating to min/max macros. Fixed in v3.
#include "nlohmann/json.hpp"
#pragma warning( pop )

using json = nlohmann::json;

class GLFWwindow;

struct GPU
{
	VkPhysicalDevice Device;
	std::vector<VkQueueFamilyProperties> QueueFamilyProperties;
	std::vector<VkExtensionProperties> ExtensionProperties;
	VkSurfaceCapabilitiesKHR SurfaceCapabilities;
	std::vector<VkSurfaceFormatKHR> SurfaceFormats;
	std::vector<VkPresentModeKHR> PresentModes;
	VkPhysicalDeviceMemoryProperties MemoryProperties;
	VkPhysicalDeviceProperties DeviceProperties;
};

struct Vertex
{
	//Vector2D Position;
	//Vector3D Colour;

	float Position[2];
	float Colour[3];

	static VkVertexInputBindingDescription GetBindingDescription( void )
	{
		VkVertexInputBindingDescription bindingDescription = {};

		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions( void )
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof( Vertex, Position );

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof( Vertex, Colour );

		return attributeDescriptions;
	}
};

std::vector<char> ReadFile( const std::string& Filename );
VkResult CreateDebugReportCallbackEXT( VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback );
void DestroyDebugReportCallbackEXT( VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator );

class GraphicsVK
{
	friend class VulkanStagingManager;
private:

	VkDebugReportCallbackEXT Callback;

	GLFWwindow* Window;

	Log log{ "GraphicsVK" };

	json Settings;

	const bool enableDebugLayers = true; //TODO: Get from settings file later

	std::vector<const char *> InstanceExtensions;
	std::vector<const char *> DeviceExtensions;
	std::vector<const char *> ValidationLayers;

	VkInstance Instance;
	VkSurfaceKHR Surface;

	std::vector<GPU> GPUs;

	int GraphicsFamilyIndex = 0;
	int PresentFamilyIndex = 0;
	int TransferFamilyIndex = 0;

	VkPhysicalDevice PhysicalDevice;

	VkQueue GraphicsQueue;
	VkQueue PresentQueue;
	VkQueue TransferQueue;

	std::vector<VkSemaphore> AcquireSemaphores;
	std::vector<VkSemaphore> RenderCompleteSemaphores;

	VkCommandPool CommandPool;
	std::vector<VkCommandBuffer> CommandBuffers;
	std::vector<VkFence> CommandBufferFences;

	VkSwapchainKHR SwapChain;
	VkFormat SwapChainFormat;
	VkPresentModeKHR SwapChainPresentMode;
	VkExtent2D SwapChainExtent;

	std::vector<VkImageView> SwapChainImageViews;

	unsigned int CurrentFrame = 0;

	VkFormat DepthFormat;
	VkImage DepthBuffer;
	vkAllocation DepthBufferMemory;
	VkImageView DepthBufferView;

	VkRenderPass RenderPass;

	std::vector<VkFramebuffer> FrameBuffers;

	void ReadSettings( void );
	void WriteSettings( void );

	//GLFW
	void GetInstanceExtensions( void );

	//Vulkan
	bool CheckValidationLayerSupport( void );
	bool CheckPhysicalDeviceExtensionSupport( GPU Device );
	void CreateDebugReportCallback( void );
	void CreateInstance( void );
	void CreateSurface( void );
	void EnumeratePhysicalDevices( void );
	void SelectPhysicalDevice( void );
	void CreateLogicalDeviceAndQueues( void );
	void CreateSemaphores( void );
	void CreateCommandPool( void );
	void CreateCommandBuffer( void );
	void CreateSwapChain( void );
	void CreateRenderTargets( void );
	void CreateRenderPass( void );
	void CreateFrameBuffers( void );
	void RecordCommands( void );

	void InitSwapChain( void );
	void CleanUpSwapChain( void );
	void RecreateSwapChain( void );

	VkSurfaceFormatKHR ChooseSurfaceFormat( void );
	VkPresentModeKHR ChoosePresentMode( void );
	VkExtent2D ChooseSurfaceExtent( void );
	VkFormat ChooseSupportedFormat( const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags features );
public:

	const unsigned int BufferCount = 2;
	GPU* GPUInfo = nullptr;

	VkDevice LogicalDevice;

	GraphicsVK( GLFWwindow* Window );

	void Init( void );
	void CleanUp( void );

	void Begin( void ); //Begin()?
	void End( void ); //End()?
	void Submit( void ); //Submit()?

	void DrawFrame( void );

	VkResult FindMemoryTypeIndex( const unsigned int MemoryTypeBits, const VkMemoryPropertyFlags Required, const VkMemoryPropertyFlags Preferred, unsigned int& SelectedMemoryTypeIndex, VkMemoryPropertyFlags& SupportedProperties );
	VkBuffer CreateBuffer( VkDeviceSize Size, VkBufferUsageFlags Usage, VkSharingMode SharingMode, VkMemoryPropertyFlags Properties, vkAllocation& Allocation );
	VkShaderModule LoadShaderModule( const std::string& Filename );

	VkCommandBuffer GetCurrentCommandBuffer( void );
	VkRenderPass GetRenderPass( void );
	VkExtent2D GetFrameBufferExtent( void );

	VulkanAllocator MemoryAllocator{ *this };
	VulkanStagingManager StagingManager{ *this };

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback( VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData );
};

#endif //_GRAPHICSVK_H_