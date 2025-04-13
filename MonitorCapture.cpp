#include "pch.h"
#include "Util.h"
#include <fstream>
#include "composition.interop.h"
#include "direct3d11.interop.h"
#include "MonitorCapture.h"

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

MonitorCapture::MonitorCapture(winrt::IDirect3DDevice const& device, winrt::GraphicsCaptureItem const& item)
    : m_device(device), m_item(item)
{
    auto pixelFormat = winrt::DirectXPixelFormat::B8G8R8A8UIntNormalized;

    m_d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_device);
    m_d3dDevice->GetImmediateContext(m_d3dContext.put());

    m_swapChain = CreateDXGISwapChain(m_d3dDevice, static_cast<uint32_t>(m_item.Size().Width), static_cast<uint32_t>(m_item.Size().Height),
        static_cast<DXGI_FORMAT>(pixelFormat), 2);

    m_framePool = winrt::Direct3D11CaptureFramePool::CreateFreeThreaded(m_device,
        pixelFormat, 2, m_item.Size());

    m_session = m_framePool.CreateCaptureSession(m_item);
    m_framePool.FrameArrived({ this, &MonitorCapture::OnFrameArrived });
}

void MonitorCapture::StartCapture()
{
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
    auto frame = sender.TryGetNextFrame();
    if (!frame) return;
    auto surface = frame.Surface();
    auto texture = GetD3D11TextureFromSurface(surface); // custom helper
    // Save the texture to a BMP file
    SaveTextureToBMP(m_d3dDevice.get(), texture.get(), L"capture.bmp");
}

void MonitorCapture::Close()
{
    auto expected = false;
    if (m_closed.compare_exchange_strong(expected, true))
    {
        m_session.Close();
        m_framePool.Close();

        m_swapChain = nullptr;
        m_framePool = nullptr;
        m_session = nullptr;
        m_item = nullptr;
    }
}