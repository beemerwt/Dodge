#pragma once
#include <opencv2/core.hpp>

class MonitorCapture {
public:
    MonitorCapture(
        winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const& device,
        winrt::Windows::Graphics::Capture::GraphicsCaptureItem const& item);
    ~MonitorCapture() { Close(); }

    void StartCapture();

    inline void SetFrameHandler(std::function<void(const cv::cuda::GpuMat&)> handler) {
        m_frameHandler = handler;
    }

    winrt::Windows::UI::Composition::ICompositionSurface CreateSurface(winrt::Windows::UI::Composition::Compositor const& compositor);

    void Close();

private:
    void OnFrameArrived(
        winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender,
        winrt::Windows::Foundation::IInspectable const& args);

    inline void CheckClosed() {
        if (m_closed.load() == true) {
            throw winrt::hresult_error(RO_E_CLOSED);
        }
    }

private:
    D3D11_BOX cropRegion = {};
    Microsoft::WRL::ComPtr<ID3D11Texture2D> croppedTexture;
    std::function<void(const cv::cuda::GpuMat&)> m_frameHandler;

    cv::cuda::GpuMat m_frame;

    winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_item{ nullptr };
    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool{ nullptr };
    winrt::Windows::Graphics::Capture::GraphicsCaptureSession m_session{ nullptr };

    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_device{ nullptr };
    winrt::com_ptr<IDXGISwapChain1> m_swapChain{ nullptr };
    winrt::com_ptr<ID3D11Device> m_d3dDevice{ nullptr };
    winrt::com_ptr<ID3D11DeviceContext> m_d3dContext{ nullptr };

    cudaGraphicsResource* m_cudaResource = nullptr;

    std::atomic<bool> m_closed{ false };
};