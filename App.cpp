#include "pch.h"
#include "App.h"
#include "Util.h"
#include "direct3d11.interop.h"
#include "capture.desktop.interop.h"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Metadata;
    using namespace Windows::Graphics::Capture;
    using namespace Windows::Graphics::DirectX;
    using namespace Windows::Graphics::Imaging;
    using namespace Windows::Storage;
    using namespace Windows::Storage::Pickers;
    using namespace Windows::System;
    using namespace Windows::UI;
    using namespace Windows::UI::Composition;
    using namespace Windows::UI::Popups;
}

App::App(winrt::ContainerVisual& root) : detection(keybind) {
    m_mainThread = winrt::DispatcherQueue::GetForCurrentThread();
    WINRT_VERIFY(m_mainThread != nullptr);

    auto d3dDevice = CreateD3D11Device();
    auto dxgiDevice = d3dDevice.as<IDXGIDevice>();
    m_device = CreateDirect3DDevice(dxgiDevice.get());
}

void App::StartCaptureFromItem(winrt::GraphicsCaptureItem item)
{
    m_capture = std::make_unique<MonitorCapture>(m_device, item);
    bool running = true;

    // In a GUI thread (call this from main thread)
    /*
    std::thread([&]() {
        cv::namedWindow("DetectionWindow", cv::WINDOW_NORMAL);
        cv::namedWindow("Mask", cv::WINDOW_NORMAL);

        cv::resizeWindow("DetectionWindow", 1560, 750);  // Optional
        cv::resizeWindow("Mask", 1560, 750);  // Optional

        while (running) {
            std::lock_guard<std::mutex> lock(frameMutex);
            cv::Mat frame = latestFrame;
            cv::Mat mask = latestMask;

            if (!frame.empty())
                cv::imshow("DetectionWindow", frame);

            if (!mask.empty())
                cv::imshow("Mask", mask);

            int key = cv::waitKey(1);
            if (key == 27) running = false; // ESC to quit
        }

        cv::destroyAllWindows();
    }).detach();
    */

    m_capture->SetFrameHandler([this](const cv::cuda::GpuMat& frame) {
        if (frame.empty()) {
            OutputDebugString(L"Frame was empty\n");
            return;
        }

        std::lock_guard<std::mutex> lock(frameMutex);

        cv::cuda::GpuMat hsv;
        cv::cuda::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

        // Define purple glow range (you might need to tune this)
        cv::Scalar lower(141, 0, 199);  // H, S, V
        cv::Scalar upper(150, 158, 255);

        // Create mask
        cv::Mat mask;
        cv::inRange(hsv, lower, upper, mask);

        // Optional: Blur to reduce noise
        cv::cuda::GaussianBlur(mask, mask, cv::Size(9, 9), 2);

        // Detect circles
        std::vector<cv::Vec3f> circles;
        cv::HoughCircles(mask, circles, cv::HOUGH_GRADIENT, 1,
            mask.rows/8,   // min dist between circles
            100, 30,       // param1, param2 (edge detection thresholds)
            30, 150);      // min/max radius

        detection.Process(circles);

        // Draw circles
        for (size_t i = 0; i < circles.size(); i++) {
            cv::Vec3i c = circles[i];
            cv::circle(frame, cv::Point(c[0], c[1]), c[2], cv::Scalar(0, 255, 0), 2);
        }

        latestFrame = frame.clone();
        latestMask = mask.clone();
    });
    m_capture->StartCapture();
}

winrt::GraphicsCaptureItem App::TryStartCaptureFromMonitorHandle(HMONITOR hmon)
{
    winrt::GraphicsCaptureItem item{ nullptr };
    try
    {
        item = CreateCaptureItemForMonitor(hmon);
        StartCaptureFromItem(item);
    }
    catch (winrt::hresult_error const& error)
    {
        MessageBoxW(m_mainWindow, error.message().c_str(), L"Dodge", MB_OK | MB_ICONERROR);
    }
    return item;
}

void App::StopCapture() {
    if (m_capture)
    {
        m_capture->Close();
        m_capture = nullptr;
    }
}