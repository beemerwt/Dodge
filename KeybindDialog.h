#pragma once
#include "DesktopDialog.h"

class KeybindDialog : public DesktopDialog<KeybindDialog, int> {
public:
    static constexpr int DIALOG_RESOURCE = IDD_KEYBIND_DIALOG;
    KeybindDialog() = default;
    ~KeybindDialog();

    INT_PTR HandleMessage(UINT const hMsg, WPARAM const wParam, LPARAM const lParam) override;
    LRESULT KeyHook(int nCode, WPARAM wParam, LPARAM lParam);

private:
    void ShowCurrentKeybind();

private:
    HHOOK m_hook = nullptr;
    HWND m_keybind = nullptr;
    int currentKey = VK_LCONTROL; // default keybind
};