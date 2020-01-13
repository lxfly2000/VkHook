#pragma once
#include<Windows.h>
#include"vulkan/vulkan.h"
#ifdef __cplusplus
extern "C" {
#endif
void CustomQueuePresentKHR(VkQueue q, const VkPresentInfoKHR* pInfo);
void CustomSetViewport(VkCommandBuffer cb, uint32_t firstViewport, uint32_t vpCount, const VkViewport* pViewports);
void CustomExecuteCommands(VkCommandBuffer cb, uint32_t cbCount, const VkCommandBuffer* pCommandBuffers);
void CustomQueueSubmit(VkQueue q, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence);
void WINAPI OriginalSetViewport(VkCommandBuffer cb, uint32_t firstViewport, uint32_t vpCount, const VkViewport* pViewports);
DWORD GetDLLPath(LPTSTR path, DWORD max_length);
DWORD GetDLLPathA(LPSTR path, DWORD max_length);
#ifdef __cplusplus
}
#endif
