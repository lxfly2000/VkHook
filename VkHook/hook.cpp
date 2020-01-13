//https://pastebin.com/f6d87dd03
#include<Windows.h>
#include"vulkan/vulkan.h"
#include"..\minhook\include\MinHook.h"

#include"custom_swapbuffers.h"

typedef PFN_vkQueuePresentKHR PFvkQueuePresentKHR;
typedef PFN_vkCmdExecuteCommands PFvkCmdExecuteCommands;
typedef PFN_vkCmdSetViewport PFvkCmdSetViewport;
typedef PFN_vkQueueSubmit PFvkQueueSubmit;
static PFvkQueuePresentKHR pfPresent = nullptr, pfOriginalPresent = nullptr;
static PFvkCmdExecuteCommands pfExecuteCommands = nullptr, pfOriginalExecuteCommands = nullptr;
static PFvkCmdSetViewport pfSetViewport = nullptr, pfOriginalSetViewport = nullptr;
static PFvkQueueSubmit pfQueueSubmit = nullptr, pfOriginalQueueSubmit = nullptr;
static HMODULE hDllModule;

DWORD GetDLLPath(LPTSTR path, DWORD max_length)
{
	return GetModuleFileName(hDllModule, path, max_length);
}

DWORD GetDLLPathA(LPSTR path, DWORD max_length)
{
	return GetModuleFileNameA(hDllModule, path, max_length);
}

BOOL WINAPI HookedvkQueuePresentKHR(VkQueue q, const VkPresentInfoKHR* pInfo)
{
	CustomQueuePresentKHR(q, pInfo);
	return pfOriginalPresent(q, pInfo);
}

void WINAPI HookedvkCmdSetViewport(VkCommandBuffer cb, uint32_t firstViewport, uint32_t vpCount, const VkViewport* pViewports)
{
	CustomSetViewport(cb, firstViewport, vpCount, pViewports);
	return pfOriginalSetViewport(cb, firstViewport, vpCount, pViewports);
}

void WINAPI HookedvkCmdExecuteCommands(VkCommandBuffer cb, uint32_t cbCount, const VkCommandBuffer* pCommandBuffers)
{
	CustomExecuteCommands(cb, cbCount, pCommandBuffers);
	return pfOriginalExecuteCommands(cb, cbCount, pCommandBuffers);
}

//目前只发现这个函数被调用的概率比较大
VkResult WINAPI HookedvkQueueSubmit(VkQueue q,uint32_t submitCount,const VkSubmitInfo *pSubmits,VkFence fence)
{
	CustomQueueSubmit(q, submitCount, pSubmits, fence);
	return pfOriginalQueueSubmit(q, submitCount, pSubmits, fence);
}

void WINAPI OriginalSetViewport(VkCommandBuffer cb, uint32_t firstViewport, uint32_t vpCount, const VkViewport* pViewports)
{
	return pfOriginalSetViewport(cb, firstViewport, vpCount, pViewports);
}

PFvkQueuePresentKHR GetQueuePresentKHR()
{
	return reinterpret_cast<PFvkQueuePresentKHR>(GetProcAddress(LoadLibrary(TEXT("vulkan-1.dll")), "vkQueuePresentKHR"));
}

PFvkCmdSetViewport GetSetViewportAddr()
{
	return reinterpret_cast<PFvkCmdSetViewport>(GetProcAddress(LoadLibrary(TEXT("vulkan-1.dll")), "vkCmdSetViewport"));
}

PFvkCmdExecuteCommands GetExecuteCommandsAddr()
{
	return reinterpret_cast<PFvkCmdExecuteCommands>(GetProcAddress(LoadLibrary(TEXT("vulkan-1.dll")), "vkCmdExecuteCommands"));
}

PFvkQueueSubmit GetQueueSubmitAddr()
{
	return reinterpret_cast<PFvkQueueSubmit>(GetProcAddress(LoadLibrary(TEXT("vulkan-1.dll")), "vkQueueSubmit"));
}

//导出以方便在没有DllMain时调用
extern "C" __declspec(dllexport) BOOL StartHook()
{
	pfPresent = GetQueuePresentKHR();
	pfSetViewport = GetSetViewportAddr();
	pfExecuteCommands = GetExecuteCommandsAddr();
	pfQueueSubmit = GetQueueSubmitAddr();
	if (MH_Initialize() != MH_OK)
		return FALSE;
	if (MH_CreateHook(pfPresent, HookedvkQueuePresentKHR, reinterpret_cast<void**>(&pfOriginalPresent)) != MH_OK)
		return FALSE;
	if (MH_CreateHook(pfSetViewport, HookedvkCmdSetViewport, reinterpret_cast<void**>(&pfOriginalSetViewport)) != MH_OK)
		return FALSE;
	if (MH_CreateHook(pfExecuteCommands, HookedvkCmdExecuteCommands, reinterpret_cast<void**>(&pfOriginalExecuteCommands)) != MH_OK)
		return FALSE;
	if (MH_CreateHook(pfQueueSubmit, HookedvkQueueSubmit, reinterpret_cast<void**>(&pfOriginalQueueSubmit)) != MH_OK)
		return FALSE;
	if (MH_EnableHook(pfPresent) != MH_OK)
		return FALSE;
	if (MH_EnableHook(pfSetViewport) != MH_OK)
		return FALSE;
	if (MH_EnableHook(pfExecuteCommands) != MH_OK)
		return FALSE;
	if (MH_EnableHook(pfQueueSubmit) != MH_OK)
		return FALSE;
	return TRUE;
}

//导出以方便在没有DllMain时调用
extern "C" __declspec(dllexport) BOOL StopHook()
{
	if (MH_DisableHook(pfQueueSubmit) != MH_OK)
		return FALSE;
	if (MH_DisableHook(pfExecuteCommands) != MH_OK)
		return FALSE;
	if (MH_DisableHook(pfSetViewport) != MH_OK)
		return FALSE;
	if (MH_DisableHook(pfPresent) != MH_OK)
		return FALSE;
	if (MH_RemoveHook(pfQueueSubmit) != MH_OK)
		return FALSE;
	if (MH_RemoveHook(pfExecuteCommands) != MH_OK)
		return FALSE;
	if (MH_RemoveHook(pfSetViewport) != MH_OK)
		return FALSE;
	if (MH_RemoveHook(pfPresent) != MH_OK)
		return FALSE;
	if (MH_Uninitialize() != MH_OK)
		return FALSE;
	return TRUE;
}

DWORD WINAPI TInitHook(LPVOID param)
{
	return StartHook();
}

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	hDllModule = hInstDll;
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hInstDll);
		CreateThread(NULL, 0, TInitHook, NULL, 0, NULL);
		break;
	case DLL_PROCESS_DETACH:
		StopHook();
		break;
	case DLL_THREAD_ATTACH:break;
	case DLL_THREAD_DETACH:break;
	}
	return TRUE;
}

//SetWindowHookEx需要一个导出函数，否则DLL不会被加载
extern "C" __declspec(dllexport) LRESULT WINAPI HookProc(int code, WPARAM w, LPARAM l)
{
	return CallNextHookEx(NULL, code, w, l);
}