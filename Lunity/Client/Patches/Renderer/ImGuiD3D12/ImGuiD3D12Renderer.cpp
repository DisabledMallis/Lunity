#include "ImGuiD3D12Renderer.h"

inline ImGuiD3D12Renderer* currentRenderer = nullptr;

long __fastcall _Present(IDXGISwapChain3* SwapChainPtr,
                         unsigned int SyncInterval,
                         unsigned int Flags) {
    if (currentRenderer) return currentRenderer->invokePresent(SwapChainPtr, SyncInterval, Flags);
    if (!ImGuiD3D12Renderer::D3D12Present) throw std::exception("ImGuiD3D12Renderer::D3D12Present was null.");
    return ImGuiD3D12Renderer::D3D12Present(SwapChainPtr, SyncInterval, Flags);
}

long __fastcall _ResizeBuffers(IDXGISwapChain3* SwapChainPtr,
                               unsigned int BufferCount,
                               unsigned int Width,
                               unsigned int Height,
                               DXGI_FORMAT NewFormat,
                               unsigned int SwapChainFlags) {
    if (currentRenderer) return currentRenderer->invokeResizeBuffers(SwapChainPtr, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    if (!ImGuiD3D12Renderer::D3D12ResizeBuffers) throw std::exception("ImGuiD3D12Renderer::D3D12ResizeBuffers was null.");
    return ImGuiD3D12Renderer::D3D12ResizeBuffers(SwapChainPtr, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

ImGuiD3D12Renderer::~ImGuiD3D12Renderer() {
    if (IsHooked()) {
        Unhook();
    }
}

auto ImGuiD3D12Renderer::IsHooked() -> bool {
    return Hooked;
}

auto ImGuiD3D12Renderer::HookUnsafely() -> void {
    if (Hooked) return;

    currentRenderer = this;

    if (kiero::init(kiero::RenderType::D3D12) != kiero::Status::Success)
        throw std::exception("Could not initialize kiero in D3D12Renderer.cpp");

    InitializeOnNextCall = true;

    kiero::bind(140, (void**)&D3D12Present, _Present);
    kiero::bind(145, (void**)&D3D12ResizeBuffers, _ResizeBuffers); // 13

    Hooked = true;
}

auto ImGuiD3D12Renderer::Hook() -> void {
    if (Hooked) return;
    if (IsAlreadyHooked) throw std::exception("A hook has already been initialized.");

    hookUnsafely();

    IsAlreadyHooked = true;
}

auto ImGuiD3D12Renderer::Unhook() -> void {
    if (!Hooked) return;

    if (currentRenderer == this) currentRenderer = nullptr;

    kiero::unbind(140);
    kiero::unbind(145);

    Release();

    ImGui_ImplDX12_Shutdown();

    IsAlreadyHooked = false;
    Hooked = false;
}

auto ImGuiD3D12Renderer::Release() -> void {
    if (!Hooked) return;

    DisposeOnNextCall = true;

    if (D3D12DevicePtr) D3D12DevicePtr->Release();
    if (D3D12DescriptorHeapBackBuffersPtr) D3D12DescriptorHeapBackBuffersPtr->Release();
    if (D3D12DescriptorHeapImGuiRenderPtr) D3D12DescriptorHeapImGuiRenderPtr->Release();

    if (FrameContextPtr)
    {
        for (size_t i = 0; i < BufferCount; i++)
        {
            if ((uintptr_t)FrameContextPtr[i].MainRenderTargetResourcePtr > 69420)
                FrameContextPtr[i].MainRenderTargetResourcePtr->Release();
        }
    }

    D3D12DevicePtr = nullptr;
    D3D12DescriptorHeapBackBuffersPtr = nullptr;
    D3D12DescriptorHeapImGuiRenderPtr = nullptr;

    Hooked = false;
}

auto ImGuiD3D12Renderer::Reload() -> void {
    Unhook();
    Hook();
}

auto ImGuiD3D12Renderer::InvokePresent(int *SwapChainPtr,
                                       unsigned int SyncInterval,
                                       unsigned int Flags) -> long {
    auto startTime = _Clock.now();

    _CurrentFrame++;

    if (std::chrono::duration<float>(startTime - _StartPoint).count() > 0.99999f)
    {
        _FPS = _CurrentFrame;
        _CurrentFrame = 0;
        _StartPoint = startTime;
    }

    if (time(0) < (_LastResizeTimePoint + 1))
    {
        return D3D12Present(SwapChainPtr, SyncInterval, Flags);
    }

    if (_ReleaseOnNextCall)
    {
        _DisposeOnNextCall = true;
        _ReleaseOnNextCall = false;
        _Release();
    }

    if (_InitializeOnNextCall)
    {
        _InitializeOnNextCall = false;
        _DisposeOnNextCall = false;

        D3D12SpawChain3Ptr = SwapChainPtr;
        if (SUCCEEDED(SwapChainPtr->GetDevice(__uuidof(ID3D12Device), (void**)&D3D12DevicePtr)))
        {
            static bool hasInitializedImGui = false;

            if (!hasInitializedImGui)
            {
                ImGui::CreateContext();
                uchar* pixels;
                int width;
                int height;
                ImGuiIO& io = ImGui::GetIO();
                ImGui::StyleColorsDark();

                io.Fonts->AddFontDefault();

                bool b;
                Initializing.Invoke(b);

                io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
                io.IniFilename = null;
                io.MouseDrawCursor = false;

                CreateEvent(nullptr, false, false, nullptr);

                hasInitializedImGui = true;
            }

            DXGI_SWAP_CHAIN_DESC swapChainDescription;
            SwapChainPtr->GetDesc(&swapChainDescription);
            swapChainDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            swapChainDescription.OutputWindow = _WindowHandle;
            swapChainDescription.Windowed = ((GetWindowLongPtr(_WindowHandle, GWL_STYLE) & WS_POPUP) != 0) ? false : true;

            ReservedBufferCount = swapChainDescription.BufferCount;

            D3D12_DESCRIPTOR_HEAP_DESC heapDescriptorImGuiRender = { };
            heapDescriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            heapDescriptorImGuiRender.NumDescriptors = ReservedBufferCount;
            heapDescriptorImGuiRender.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

            _RegisteredImagesHandleHeapDescriptorDescriptorCount(&heapDescriptorImGuiRender);

            BufferCount = heapDescriptorImGuiRender.NumDescriptors;
            FrameContextPtr = new FrameContext[BufferCount];

            if (D3D12DevicePtr->CreateDescriptorHeap(&heapDescriptorImGuiRender, IID_PPV_ARGS(&D3D12DescriptorHeapImGuiRenderPtr)) != S_OK)
            {
                return false;
            }

            ID3D12CommandAllocator* commandAllocator;

            if (D3D12DevicePtr->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)) != S_OK)
            {
                return false;
            }

            for (ulong i = 0; i < BufferCount; i++)
            {
                FrameContextPtr[i].CommandAllocatorPtr = commandAllocator;
            }

            if (D3D12DevicePtr->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, null, IID_PPV_ARGS(&D3D12CommandListPtr)) != S_OK || D3D12CommandListPtr->Close() != S_OK)
            {
                return false;
            }

            D3D12_DESCRIPTOR_HEAP_DESC heapDescriptorBackBuffers = { };
            heapDescriptorBackBuffers.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            heapDescriptorBackBuffers.NumDescriptors = BufferCount;
            heapDescriptorBackBuffers.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            heapDescriptorBackBuffers.NodeMask = 1;

            if (D3D12DevicePtr->CreateDescriptorHeap(&heapDescriptorBackBuffers, IID_PPV_ARGS(&D3D12DescriptorHeapBackBuffersPtr)) != S_OK)
            {
                return false;
            }

            uint rtvDescriptorSize = D3D12DevicePtr->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = D3D12DescriptorHeapBackBuffersPtr->GetCPUDescriptorHandleForHeapStart();

            for (uint i = 0; i < ReservedBufferCount; i++)
            {
                ID3D12Resource* backBufferPtr = nullptr;
                FrameContextPtr[i].MainRenderTargetDescriptor = rtvHandle;
                SwapChainPtr->GetBuffer(i, IID_PPV_ARGS(&backBufferPtr));
                D3D12DevicePtr->CreateRenderTargetView(backBufferPtr, nullptr, rtvHandle);
                FrameContextPtr[i].MainRenderTargetResourcePtr = backBufferPtr;
                rtvHandle.ptr += rtvDescriptorSize;
                D3D12_RESOURCE_DESC backBufferDescriptor = backBufferPtr->GetDesc();
                _Width = backBufferDescriptor.Width;
                _Height = backBufferDescriptor.Height;
            }

            _RegisteredImagesHandleSvr(D3D12DevicePtr, D3D12DescriptorHeapImGuiRenderPtr, ReservedBufferCount);

            ImGui_ImplWin32_Init(_WindowHandle);
            ImGui_ImplDX12_Init(
                D3D12DevicePtr,
                BufferCount,
                DXGI_FORMAT_R8G8B8A8_UNORM, D3D12DescriptorHeapImGuiRenderPtr,
                D3D12DescriptorHeapImGuiRenderPtr->GetCPUDescriptorHandleForHeapStart(),
                D3D12DescriptorHeapImGuiRenderPtr->GetGPUDescriptorHandleForHeapStart()
            );

            ImGui_ImplDX12_CreateDeviceObjects();
        }
    }

    if (_DisposeOnNextCall)
    {
        return D3D12Present(SwapChainPtr, SyncInterval, Flags);
    }

    if (D3D12CommandQueuePtr != nullptr)
    {
        try
        {
            ImGui_ImplDX12_NewFrame();
            //ImGui_ImplWin32_NewFrame();
            ImGuiIO& io = ImGui::GetIO();
            io.DisplaySize = ImVec2(_Width, _Height);

            ImGui::NewFrame();

            bool b;
            static bool _FirstRender = false;
            if (!_FirstRender)
            {
                _FirstRender = true;
                FirstRender.Invoke(b);
            }
            Render.Invoke(b);

            FrameContext& currentFrameContext = FrameContextPtr[SwapChainPtr->GetCurrentBackBufferIndex()];
            currentFrameContext.CommandAllocatorPtr->Reset();

            D3D12_RESOURCE_BARRIER barrier;
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = currentFrameContext.MainRenderTargetResourcePtr;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

            D3D12CommandListPtr->Reset(currentFrameContext.CommandAllocatorPtr, nullptr);
            D3D12CommandListPtr->ResourceBarrier(1, &barrier);
            D3D12CommandListPtr->OMSetRenderTargets(1, &currentFrameContext.MainRenderTargetDescriptor, false, nullptr);
            D3D12CommandListPtr->SetDescriptorHeaps(1, &D3D12DescriptorHeapImGuiRenderPtr);

            ImGui::Render();
            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), D3D12CommandListPtr);

            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

            D3D12CommandListPtr->ResourceBarrier(1, &barrier);
            D3D12CommandListPtr->Close();

            D3D12CommandQueuePtr->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&D3D12CommandListPtr));
        }
        catch (...)
        {

        }
    }

    _FrameTime = _Clock.now() - startTime;

    return D3D12Present(SwapChainPtr, SyncInterval, Flags);
}

auto ImGuiD3D12Renderer::InvokeResizeBuffers(IDXGISwapChain3* SwapChainPtr,
                                             unsigned int BufferCount,
                                             unsigned int Width,
                                             unsigned int Height,
                                             DXGI_FORMAT NewFormat,
                                             unsigned int SwapChainFlags) -> long {
    _LastResizeTimePoint = time(0) + 1;
    if (InitializeOnNextCall)
    {
        return D3D12ResizeBuffers(SwapChainPtr, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    }
    Release();
    InitializeOnNextCall = true;
    return D3D12ResizeBuffers(SwapChainPtr, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}