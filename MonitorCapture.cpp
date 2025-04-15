#include "pch.h"
#include "Util.h"
#include <fstream>
#include "composition.interop.h"
#include "direct3d11.interop.h"
#include <wrl/client.h>
#include "MonitorCapture.h"

#define FRAME_WIDTH 1560
#define FRAME_HEIGHT 750

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Numerics;
    using namespace Windows::Graphics;
    using namespace Windows::Graphics::Capture;
    using namespace Windows::Graphics::DirectX;
    using namespace Windows::Graphics::DirectX::Direct3D11;
    using namespace Windows::System;
    using namespace Windows::UI;
    using namespace Windows::UI::Composition;
}

using Microsoft::WRL::ComPtr;

MonitorCapture::MonitorCapture(winrt::IDirect3DDevice const& device, winrt::GraphicsCaptureItem const& item)
    : m_device(device), m_item(item), m_frame(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC4)
{
    auto pixelFormat = winrt::DirectXPixelFormat::B8G8R8A8UIntNormalized;

    m_d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_device);
    m_d3dDevice->GetImmediateContext(m_d3dContext.put());

    m_swapChain = CreateDXGISwapChain(m_d3dDevice, static_cast<uint32_t>(m_item.Size().Width), static_cast<uint32_t>(m_item.Size().Height),
        static_cast<DXGI_FORMAT>(pixelFormat), 2);

    m_framePool = winrt::Direct3D11CaptureFramePool::CreateFreeThreaded(m_device,
        pixelFormat, 2, m_item.Size());

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = 1560;
    desc.Height = 750;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // or whatever your surface uses
    desc.Usage = D3D11_USAGE_DEFAULT; // For CPU: D3D11_USAGE_STAGING;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // For CPU: D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    // For CPU: desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED; // GPU ONLY
    desc.SampleDesc.Count = 1;
    desc.BindFlags = 0;
    desc.MipLevels = 1;
    desc.ArraySize = 1;

    HRESULT hr = m_d3dDevice->CreateTexture2D(&desc, nullptr, &croppedTexture);
    if (FAILED(hr)) {
        OutputDebugString(L"Failed to create cropped texture.\n");
        throw winrt::hresult_error(hr);
    }

    ComPtr<IDXGIResource> dxgiResource;
    croppedTexture.As(&dxgiResource);

    HANDLE sharedHandle;
    dxgiResource->GetSharedHandle(&sharedHandle);

    cudaGraphicsD3D11RegisterResource(
        &m_cudaResource,
        croppedTexture.Get(),
        cudaGraphicsRegisterFlagsNone
    );

    cropRegion.left = 180;
    cropRegion.top = 120;
    cropRegion.right = 1740;
    cropRegion.bottom = 870;
    cropRegion.front = 0;
    cropRegion.back = 1;

    m_session = m_framePool.CreateCaptureSession(m_item);
    m_framePool.FrameArrived({ this, &MonitorCapture::OnFrameArrived });
}

void MonitorCapture::StartCapture()
{
    if (!m_frameHandler)
        throw winrt::hresult_error(E_FAIL, L"Frame handler not set.");

    CheckClosed();
    m_session.StartCapture();
}

winrt::ICompositionSurface MonitorCapture::CreateSurface(winrt::Compositor const& compositor)
{
    CheckClosed();
    return CreateCompositionSurfaceForSwapChain(compositor, m_swapChain.get());
}


winrt::com_ptr<ID3D11Texture2D> GetD3D11TextureFromSurface(winrt::IDirect3DSurface const& surface) {
    winrt::com_ptr<IDirect3DDxgiInterfaceAccess> access = surface.as<IDirect3DDxgiInterfaceAccess>();
    winrt::com_ptr<ID3D11Texture2D> texture;
    winrt::check_hresult(access->GetInterface(IID_PPV_ARGS(&texture)));
    return texture;
}

bool SaveTextureToBMP(ID3D11Device* device, ID3D11Texture2D* texture, const std::wstring& filename) {
    winrt::com_ptr<ID3D11DeviceContext> context;
    device->GetImmediateContext(context.put());

    D3D11_TEXTURE2D_DESC desc = {};
    texture->GetDesc(&desc);

    D3D11_TEXTURE2D_DESC cpuDesc = desc;
    cpuDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    cpuDesc.Usage = D3D11_USAGE_STAGING;
    cpuDesc.BindFlags = 0;
    cpuDesc.MiscFlags = 0;

    winrt::com_ptr<ID3D11Texture2D> cpuTexture;
    if (FAILED(device->CreateTexture2D(&cpuDesc, nullptr, cpuTexture.put())))
        return false;

    context->CopyResource(cpuTexture.get(), texture);

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    if (FAILED(context->Map(cpuTexture.get(), 0, D3D11_MAP_READ, 0, &mapped)))
        return false;

    BITMAPFILEHEADER bfh = {};
    BITMAPINFOHEADER bih = {};

    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = desc.Width;
    bih.biHeight = -static_cast<LONG>(desc.Height); // top-down
    bih.biPlanes = 1;
    bih.biBitCount = 32;
    bih.biCompression = BI_RGB;

    bfh.bfType = 0x4D42;
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bfh.bfSize = bfh.bfOffBits + desc.Height * mapped.RowPitch;

    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;

    file.write(reinterpret_cast<char*>(&bfh), sizeof(bfh));
    file.write(reinterpret_cast<char*>(&bih), sizeof(bih));
    for (UINT y = 0; y < desc.Height; ++y)
        file.write(reinterpret_cast<char*>((BYTE*)mapped.pData + mapped.RowPitch * y), desc.Width * 4);

    context->Unmap(cpuTexture.get(), 0);
    return true;
}

void MonitorCapture::OnFrameArrived(winrt::Direct3D11CaptureFramePool const& sender, winrt::IInspectable const&)
{
    auto start = std::chrono::high_resolution_clock::now();

    auto frame = sender.TryGetNextFrame();
    if (!frame) return;
    auto surface = frame.Surface();
    auto texture = GetD3D11TextureFromSurface(surface);
    m_d3dContext->CopySubresourceRegion(croppedTexture.Get(), 0, 0, 0, 0,
        texture.get(), 0, &cropRegion);

    // Map the CUDA resource
    cudaError_t err = cudaGraphicsMapResources(1, &m_cudaResource, 0);
    if (err != cudaSuccess) {
        OutputDebugString(L"Failed to map CUDA resource.\n");
        return;
    }

    cudaArray_t cudaArray;
    cudaGraphicsSubResourceGetMappedArray(&cudaArray, m_cudaResource, 0, 0);

    // Now can bind to CUDA texture or copy to a GpuMat or device pointer
    cudaMemcpy2DFromArray(m_frame.ptr(), m_frame.step, cudaArray, 0, 0, FRAME_WIDTH * 4, FRAME_HEIGHT, cudaMemcpyDeviceToDevice);
    m_frameHandler(m_frame);

    // Unmap the texture
    cudaGraphicsUnmapResources(1, &m_cudaResource, 0);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::wstring message = L"Frame processing time: " + std::to_wstring(duration) + L" ms\n";
    OutputDebugString(message.c_str());
}

void MonitorCapture::Close()
{
    auto expected = false;
    if (m_closed.compare_exchange_strong(expected, true))
    {

        if (m_cudaResource)
            cudaGraphicsUnregisterResource(m_cudaResource);

        m_session.Close();
        m_framePool.Close();

        m_cudaResource = nullptr;
        m_swapChain = nullptr;
        m_framePool = nullptr;
        m_session = nullptr;
        m_item = nullptr;
    }
}