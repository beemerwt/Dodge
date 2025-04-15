#pragma once
#include "Detection.h"
#include "MonitorCapture.h"

class App {
public:
    App(winrt::Windows::UI::Composition::ContainerVisual& root);
    ~App() {}

    cv::Mat latestFrame, latestMask;
    std::mutex frameMutex;

    winrt::Windows::Graphics::Capture::GraphicsCaptureItem TryStartCaptureFromMonitorHandle(HMONITOR hmon);
    inline void SetKeybind(int key) { keybind = key; }
    inline int GetKeybind() { return keybind; }
    void StopCapture();

private:
    void StartCaptureFromItem(winrt::Windows::Graphics::Capture::GraphicsCaptureItem item);

private:
    int keybind = VK_LCONTROL; // default keybind

    winrt::Windows::System::DispatcherQueue m_mainThread { nullptr };
    Detection detection;
    HWND m_mainWindow = nullptr;

    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_device { nullptr };
    std::unique_ptr<MonitorCapture> m_capture { nullptr };
};