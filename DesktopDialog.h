#pragma once

class App;

template<typename T, typename R>
struct DesktopDialog {
    using base_type = DesktopDialog<T, R>;
    HWND m_window = nullptr;
    R originalValue;

    std::atomic<bool> done = false;
    std::promise<int> resultPromise;

    static R Create(R original) {
        std::shared_ptr<T> dialog = std::make_shared<T>();
        dialog->originalValue = original;
        dialog->resultPromise = std::promise<R>();
        auto future = dialog->resultPromise.get_future();
        auto instance = winrt::check_pointer(GetModuleHandleW(nullptr));

        DialogBoxParam(instance, MAKEINTRESOURCE(T::DIALOG_RESOURCE),
            nullptr, DialogProc, reinterpret_cast<LPARAM>(dialog.get()));

        return future.get();
    }

    static INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
        DesktopDialog<T, R>* self = nullptr;

        if (message == WM_INITDIALOG) {
            OutputDebugString(L"DialogProc: WM_INITDIALOG\n");
            self = reinterpret_cast<DesktopDialog<T, R>*>(lParam);
            self->m_window = hDlg;
            SetWindowLongPtr(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        }
        else {
            self = reinterpret_cast<T*>(GetWindowLongPtr(hDlg, GWLP_USERDATA));
        }

        if (self) return self->HandleMessage(message, wParam, lParam);
        return (INT_PTR)FALSE;
    }

    virtual INT_PTR HandleMessage(UINT const message, WPARAM const wParam, LPARAM const lParam) {
        OutputDebugString(L"Handle message was called in DesktopDialog.h\n");
        return (INT_PTR)FALSE;
    }
};