#pragma once
#include <winrt/base.h>
#include <d3d11_4.h>
#include <dxgi1_2.h>

inline auto CreateD3D11Device(D3D_DRIVER_TYPE const type, UINT flags, winrt::com_ptr<ID3D11Device>& device)
{
    WINRT_ASSERT(!device);
    return D3D11CreateDevice(nullptr, type, nullptr, flags, nullptr, 0,
        D3D11_SDK_VERSION, device.put(), nullptr, nullptr);
}

inline auto CreateD3D11Device(UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT)
{
    winrt::com_ptr<ID3D11Device> device;
    HRESULT hr = CreateD3D11Device(D3D_DRIVER_TYPE_HARDWARE, flags, device);
    if (DXGI_ERROR_UNSUPPORTED == hr)
    {
        hr = CreateD3D11Device(D3D_DRIVER_TYPE_WARP, flags, device);
    }

    winrt::check_hresult(hr);
    return device;
}

inline auto CreateDXGISwapChain(winrt::com_ptr<ID3D11Device> const& device, const DXGI_SWAP_CHAIN_DESC1* desc)
{
    auto dxgiDevice = device.as<IDXGIDevice2>();
    winrt::com_ptr<IDXGIAdapter> adapter;
    winrt::check_hresult(dxgiDevice->GetParent(winrt::guid_of<IDXGIAdapter>(), adapter.put_void()));
    winrt::com_ptr<IDXGIFactory2> factory;
    winrt::check_hresult(adapter->GetParent(winrt::guid_of<IDXGIFactory2>(), factory.put_void()));

    winrt::com_ptr<IDXGISwapChain1> swapchain;
    winrt::check_hresult(factory->CreateSwapChainForComposition(device.get(), desc, nullptr, swapchain.put()));
    return swapchain;
}

inline auto CreateDXGISwapChain(winrt::com_ptr<ID3D11Device> const& device,
    uint32_t width, uint32_t height, DXGI_FORMAT format, uint32_t bufferCount)
{
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.Format = format;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferCount = bufferCount;
    desc.Scaling = DXGI_SCALING_STRETCH;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

    return CreateDXGISwapChain(device, &desc);
}