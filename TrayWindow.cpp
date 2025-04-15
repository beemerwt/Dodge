#include "pch.h"
#include "TrayWindow.h"
#include "KeybindDialog.h"
#include "App.h"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Metadata;
    using namespace Windows::Graphics::Capture;
    using namespace Windows::Graphics::DirectX;
    using namespace Windows::System;
    using namespace Windows::UI;
    using namespace Windows::UI::Composition;
    using namespace Windows::UI::Composition::Desktop;
}

const std::wstring WindowsUniversalContract = L"Windows.Foundation.UniversalApiContract";
const std::wstring TrayWindow::ClassName = L"WGCTrayAppClass";
std::once_flag TrayWindowClassRegistration;

TrayWindow::TrayWindow(std::shared_ptr<App> app) {
    auto instance = winrt::check_pointer(GetModuleHandleW(nullptr));
    std::call_once(TrayWindowClassRegistration, []() { RegisterWindowClass(); });

    auto exStyle = 0;
    auto style = WS_OVERLAPPEDWINDOW;

    winrt::check_bool(CreateWindowEx(0, ClassName.c_str(), L"Dodge", style,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr, instance, this));
    WINRT_ASSERT(m_window);

    auto isAllDisplaysPresent = winrt::ApiInformation::IsApiContractPresent(WindowsUniversalContract, 9);
    m_app = app;

    AddTrayIcon();
}

TrayWindow::~TrayWindow()
{
    RemoveTrayIcon();
}

LRESULT TrayWindow::MessageHandler(UINT const uMsg, WPARAM const wParam, LPARAM const lParam) {
    switch (uMsg) {
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP)
            ShowContextMenu();
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_TRAY_EXIT:
            PostQuitMessage(0);
            break;
        case ID_TRAY_KEYBIND:
            int keybind = KeybindDialog::Create(m_app->GetKeybind());
            m_app->SetKeybind(keybind);
            OutputDebugString(L"Keybind changed\n");
            break;
        }
        break;
    case WM_DESTROY:
        RemoveTrayIcon();
        break;
    default:
        return DefWindowProc(m_window, uMsg, wParam, lParam);
    }
    return 0;
}

void TrayWindow::RegisterWindowClass() {
    auto instance = winrt::check_pointer(GetModuleHandleW(nullptr));
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = instance;
    wc.lpszClassName = ClassName.c_str();
    winrt::check_bool(RegisterClass(&wc));
}

void TrayWindow::AddTrayIcon() {
    auto instance = winrt::check_pointer(GetModuleHandleW(nullptr));
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = m_window;  // Your hidden window
    nid.uID = IDD_KEYBIND_DIALOG;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_APP + 1;
    nid.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_TRAY_ICON));
    wcscpy_s(nid.szTip, L"Dodge");
    Shell_NotifyIcon(NIM_ADD, &nid);
}

void TrayWindow::RemoveTrayIcon() {
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = m_window;
    nid.uID = IDD_KEYBIND_DIALOG;
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

void TrayWindow::ShowContextMenu() {
    POINT pt;
    GetCursorPos(&pt);

    HMENU hMenu = CreatePopupMenu();
    InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, ID_TRAY_KEYBIND, L"Change Keybind");
    InsertMenu(hMenu, 1, MF_BYPOSITION | MF_STRING, ID_TRAY_EXIT, L"Exit");

    SetForegroundWindow(m_window);
    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, m_window, NULL);
    DestroyMenu(hMenu);
}