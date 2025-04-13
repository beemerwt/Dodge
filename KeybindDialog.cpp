#include "pch.h"
#include "KeybindDialog.h"

KeybindDialog* g_currentHookDialog = nullptr;
LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    if (!g_currentHookDialog) {
        OutputDebugString(L"Hook was not cleared\n");
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    return g_currentHookDialog->KeyHook(nCode, wParam, lParam);
}

LRESULT KeybindDialog::KeyHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
        OutputDebugString(L"Received Keypress KeybindDialog\n");
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
        if (pKey->vkCode != VK_ESCAPE)
            currentKey = pKey->vkCode;

        EnableWindow(GetDlgItem(m_window, IDOK), TRUE);
        EnableWindow(GetDlgItem(m_window, IDCANCEL), TRUE);
        ShowCurrentKeybind();

        if (m_hook) {
            UnhookWindowsHookEx(m_hook);
            m_hook = nullptr;
            g_currentHookDialog = nullptr;
        }

        return TRUE;
    }

    return CallNextHookEx(m_hook, nCode, wParam, lParam);
}

void KeybindDialog::ShowCurrentKeybind() {
    char buffer[64];
    GetKeyNameTextA(MapVirtualKeyA(currentKey, MAPVK_VK_TO_VSC) << 16, buffer, sizeof(buffer));
    // SetKeybindText(std::string("Keybind: ") + buffer + " (Click to change)");
    auto text = std::string("Keybind: ") + buffer + " (Click to change)";
    SetWindowTextA(m_keybind, text.c_str());
}

INT_PTR KeybindDialog::HandleMessage(UINT const hMsg, WPARAM const wParam, LPARAM const lParam) {
    switch (hMsg) {
    case WM_INITDIALOG:
        m_keybind = GetDlgItem(m_window, IDC_KEYBIND_BUTTON);
        currentKey = originalValue;
        ShowCurrentKeybind();
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_KEYBIND_BUTTON:
            if (HIWORD(wParam) == BN_CLICKED && !m_hook) {
                SetWindowTextA(m_keybind, "Press any key (ESC to cancel)");
                EnableWindow(GetDlgItem(m_window, IDOK), FALSE);
                EnableWindow(GetDlgItem(m_window, IDCANCEL), FALSE);
                g_currentHookDialog = this;
                m_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, nullptr, 0);
            }

            return (INT_PTR)TRUE;

        case IDCANCEL:
            currentKey = originalValue;

        case IDOK:
            resultPromise.set_value(currentKey);
            EndDialog(m_window, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }

        break;
    }

    return (INT_PTR)FALSE;
}