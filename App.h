#pragma once
#include "MonitorCapture.h"

class App {
public:
    App(winrt::Windows::UI::Composition::ContainerVisual& root);
    ~App() {}

    winrt::Windows::Graphics::Capture::GraphicsCaptureItem TryStartCaptureFromMonitorHandle(HMONITOR hmon);
    void StopCapture();

    int keybind = VK_LCONTROL; // default keybind

private:
    void StartCaptureFromItem(winrt::Windows::Graphics::Capture::GraphicsCaptureItem item);

private:
    winrt::Windows::System::DispatcherQueue m_mainThread { nullptr };

    HWND m_mainWindow = nullptr;

    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_device { nullptr };
    std::unique_ptr<MonitorCapture> m_capture { nullptr };
};