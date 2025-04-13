#pragma once
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.UI.Composition.h>
#include <windows.ui.composition.interop.h>
#include <d2d1_1.h>
#include <dxgi1_6.h>

inline auto CreateCompositionGraphicsDevice(winrt::Windows::UI::Composition::Compositor const& compositor, ::IUnknown* device)
{
    winrt::Windows::UI::Composition::CompositionGraphicsDevice graphicsDevice{ nullptr };
    auto compositorInterop = compositor.as<ABI::Windows::UI::Composition::ICompositorInterop>();
    winrt::com_ptr<ABI::Windows::UI::Composition::ICompositionGraphicsDevice> graphicsInterop;
    winrt::check_hresult(compositorInterop->CreateGraphicsDevice(device, graphicsInterop.put()));
    winrt::check_hresult(graphicsInterop->QueryInterface(winrt::guid_of<winrt::Windows::UI::Composition::CompositionGraphicsDevice>(),
        winrt::put_abi(graphicsDevice)));
    return graphicsDevice;
}

inline auto CreateCompositionSurfaceForSwapChain(winrt::Windows::UI::Composition::Compositor const& compositor, ::IUnknown* swapChain)
{
    winrt::Windows::UI::Composition::ICompositionSurface surface{ nullptr };
    auto compositorInterop = compositor.as<ABI::Windows::UI::Composition::ICompositorInterop>();
    winrt::com_ptr<ABI::Windows::UI::Composition::ICompositionSurface> surfaceInterop;
    winrt::check_hresult(compositorInterop->CreateCompositionSurfaceForSwapChain(swapChain, surfaceInterop.put()));
    winrt::check_hresult(surfaceInterop->QueryInterface(winrt::guid_of<winrt::Windows::UI::Composition::ICompositionSurface>(), winrt::put_abi(surface)));
    return surface;
}