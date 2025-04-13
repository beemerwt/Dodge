#pragma once
#include "DesktopWindow.h"

class App;
class KeybindDialog;

class TrayWindow : public DesktopWindow<TrayWindow> {
public:
    static const std::wstring ClassName;
    TrayWindow(std::shared_ptr<App> app);
    ~TrayWindow();

    LRESULT MessageHandler(UINT const message, WPARAM const wParam, LPARAM const lParam);

private:
    static void RegisterWindowClass();
    void AddTrayIcon();
    void RemoveTrayIcon();
    void ShowContextMenu();

private:
    winrt::Windows::Graphics::Capture::GraphicsCaptureItem::Closed_revoker m_itemClosedRevoker;
    std::shared_ptr<App> m_app;
};