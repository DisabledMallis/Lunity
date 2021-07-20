#ifndef LUNITY_CLIENT_RENDERER_D3D12_D3D12RENDERER_H
#define LUNITY_CLIENT_RENDERER_D3D12_D3D12RENDERER_H

#include <exception>
#include <chrono>
#include <Windows.h>
#include <dxgi.h>
#include <d3d12.h>

// kiero
#include "../../../../Lib/kiero/kiero.h"
#define KIERO_INCLUDE_D3D12  1 // 1 if you need D3D12 hook
#define KIERO_USE_MINHOOK    1 // 1 if you will use kiero::bind function
// kiero end

// imgui
#include "../../../../Lib/imgui/imgui.h"
#include "../../../../Lib/imgui/backends/imgui_impl_dx12.h"
// imgui end

#include "../../../../Utils/Event.h"
#include "../Renderer.h"

struct ImGuiD3D12FrameContext
{
    ID3D12CommandAllocator* CommandAllocatorPtr = nullptr;
    ID3D12Resource* MainRenderTargetResourcePtr = nullptr;
    D3D12_CPU_DESCRIPTOR_HANDLE MainRenderTargetDescriptor;
};

struct ImGuiD3D12InitializingEventArgs
{
    bool Handled;
    bool Cancel;
};

struct ImGuiD3D12RenderEventArgs
{
    bool Handled;
};

class ImGuiD3D12Renderer : Renderer
{
private:
    bool IsInitialized = false;
    bool Hooked = false;
    static bool IsAlreadyHooked = false;

public:
    // Typedefs
    typedef long(__fastcall* D3D12PresentFunc) (IDXGISwapChain3* SwapChainPtr, uint SyncInterval, uint Flags);
    typedef HRESULT(__fastcall* D3D12ResizeBuffersFunc)(IDXGISwapChain* SwapChainPtr, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    typedef void(*D3D12ExecuteCommandListsFunc)(ID3D12CommandQueue* CommandQueuePtr, uint CommandListsCount, ID3D12CommandList* CommandListPtr);
    typedef long(*D3D12SignalFunc)(ID3D12CommandQueue* commandQueuePtr, ID3D12Fence* fence, uint64 value);
    typedef long(__fastcall* D3D12ResizeRenderTargetsFunc)(IDXGISwapChain3* SwapChainPtr, const DXGI_MODE_DESC* newTargetParametersPtr);
    typedef void(__fastcall* D3D12DrawInstancedFunc)(ID3D12GraphicsCommandList* CommandListPtr, UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation);
    typedef void(__fastcall* D3D12DrawIndexedInstancedFunc)(ID3D12GraphicsCommandList* CommandListPtr, UINT IndexCount, UINT InstanceCount, UINT StartIndex, INT BaseVertex);

    // Resources
    IDXGISwapChain3* D3D12SwapChain3Ptr = nullptr;
    ID3D12Device* D3D12DevicePtr = nullptr;
    ID3D12DescriptorHeap* D3D12DescriptorHeapBackBuffersPtr = nullptr;
    ID3D12DescriptorHeap* D3D12DescriptorHeapImGuiRenderPtr = nullptr;
    ID3D12GraphicsCommandList* D3D12CommandListPtr = nullptr;
    ID3D12CommandQueue* D3D12CommandQueuePtr = nullptr;
    ID3D12Fence* D3D12FencePtr = nullptr;
    UInt64 D3D12FenceValue = 0;
    ImGuiD3D12FrameContext* FrameContextPtr = nullptr;
    UInt ReservedBufferCount = 0;
    UInt BufferCount = 0;

    // Original Functions
    static D3D12PresentFunc D3D12Present = nullptr;
    static D3D12ResizeBuffersFunc D3D12ResizeBuffers = nullptr;
    static D3D12ExecuteCommandListsFunc D3D12ExecuteCommandLists = nullptr;
    static D3D12SignalFunc D3D12Signal = nullptr;
    static D3D12ResizeRenderTargetsFunc D3D12ResizeRenderTargets = nullptr;
    static D3D12DrawInstancedFunc D3D12DrawInstanced = nullptr;
    static D3D12DrawIndexedInstancedFunc D3D12DrawIndexedInstanced = nullptr;

private:
    bool InitializeOnNextCall = true;
    bool ReleaseOnNextCall = false;
    bool DisposeOnNextCall = false;
    unsigned int BufferWidth = 0;
    unsigned int BufferHeight = 0;

public:
    HWND WindowHandle = 0;

private:
    unsigned int _Width;
    unsigned int _Height;
    unsigned long long _FPS = 0;
    unsigned long long _CurrentFrame = 0;
    std::chrono::duration<double, std::milli> _FrameTime;
    std::chrono::high_resolution_clock _Clock;
    std::chrono::steady_clock::time_point _StartPoint = _Clock.now();
    unsigned long long _LastResizeTimePoint = 0;

public:
    ImGuiD3D12Renderer() : Renderer("Renderer::D3D12") { }
    ~ImGuiD3D12Renderer();
    auto IsHooked() -> bool;
    auto HookUnsafely() -> void;
    auto Hook() -> void;
    auto Unhook() -> void;
    auto Reload() -> void;
    auto Release() -> void;
    auto InvokePresent(IDXGISwapChain3* SwapChainPtr,
                       unsigned int SyncInterval,
                       unsigned int Flags) -> long;
    auto InvokeResizeBuffers(IDXGISwapChain3* SwapChainPtr,
                             unsigned int BufferCount,
                             unsigned int Width,
                             unsigned int Height,
                             DXGI_FORMAT NewFormat,
                             unsigned int SwapChainFlags) -> long;
public:
    Event<void(ImGuiD3D12InitializingEventArgs&)> Initializing; // Is being invoked before the hook initializes all its resources
    Event<void(ImGuiD3D12RenderEventArgs&)> Render; // Is being invoked when everything is being rendered
};


#endif //LUNITY_CLIENT_RENDERER_D3D12_D3D12RENDERER_H
