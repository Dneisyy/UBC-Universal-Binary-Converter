#include <iostream>
#include <string>
#include <bitset>
#include <windows.h>
#include <commctrl.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

//Logic text to binary
std::string TextToBinary(const std::string& input) {
    std::string result = "";
    for (unsigned char c : input) {
        std::bitset<8> bits(c);
        result += bits.to_string() + " ";
    }
    return result;
}

// Logic binary to text
std::string BinaryToText(const std::string& input) {
    std::string cleanInput = "";
    for (char c : input) {
        if (c == '0' || c == '1') cleanInput += c;
    }
    std::string result = "";
    for (size_t i = 0; i < cleanInput.length(); i += 8) {
        if (i + 8 <= cleanInput.length()) {
            std::string byteString = cleanInput.substr(i, 8);
            unsigned long byteVal = std::bitset<8>(byteString).to_ulong();
            result += static_cast<char>(byteVal);
        }
    }
    return result;
}

HWND hInputText, hOutputText, hRadio1;
HFONT hFont;

// windus copy function
void CopyToClipboardWin(const std::string& text) {
    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, text.length() + 1);
        memcpy(GlobalLock(hGlob), text.c_str(), text.length() + 1);
        GlobalUnlock(hGlob);
        SetClipboardData(CF_TEXT, hGlob);
        CloseClipboard();
        MessageBox(NULL, "Copied to clipboard!", "Success", MB_OK | MB_ICONINFORMATION);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            hFont = CreateFont(17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");

            HWND hwndLabel1 = CreateWindow("STATIC", "Enter text or binary code:", WS_VISIBLE | WS_CHILD, 20, 15, 300, 20, hwnd, NULL, NULL, NULL);
            hInputText = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_TABSTOP, 20, 40, 445, 70, hwnd, NULL, NULL, NULL);

            hRadio1 = CreateWindow("BUTTON", "Text -> Binary", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP, 20, 125, 140, 20, hwnd, NULL, NULL, NULL);
            HWND hRadio2 = CreateWindow("BUTTON", "Binary -> Text", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 170, 125, 140, 20, hwnd, NULL, NULL, NULL);
            SendMessage(hRadio1, BM_SETCHECK, BST_CHECKED, 0);

            HWND hButton = CreateWindow("BUTTON", "Convert Data", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP, 20, 160, 290, 40, hwnd, (HMENU)1, NULL, NULL);
            HWND hCopyButton = CreateWindow("BUTTON", "Copy Result", WS_VISIBLE | WS_CHILD | WS_TABSTOP, 320, 160, 145, 40, hwnd, (HMENU)2, NULL, NULL);

            HWND hwndLabel2 = CreateWindow("STATIC", "Result:", WS_VISIBLE | WS_CHILD, 20, 215, 100, 20, hwnd, NULL, NULL, NULL);
            hOutputText = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, 20, 240, 445, 100, hwnd, NULL, NULL, NULL);

            SendMessage(hwndLabel1, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hInputText, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hRadio1, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hRadio2, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hCopyButton, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hwndLabel2, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hOutputText, WM_SETFONT, (WPARAM)hFont, TRUE);
            break;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == 1) {
                char inputBuffer[4096] = {0};
                GetWindowText(hInputText, inputBuffer, 4096);
                std::string result = (SendMessage(hRadio1, BM_GETCHECK, 0, 0) == BST_CHECKED) ? TextToBinary(inputBuffer) : BinaryToText(inputBuffer);
                SetWindowText(hOutputText, result.c_str());
            }
            if (LOWORD(wParam) == 2) {
                char outputBuffer[4096] = {0};
                GetWindowText(hOutputText, outputBuffer, 4096);
                CopyToClipboardWin(outputBuffer);
            }
            break;
        }
        case WM_CTLCOLORSTATIC: {
            HBRUSH hBrush = (HBRUSH)(COLOR_BTNFACE + 1);
            SetBkMode((HDC)wParam, TRANSPARENT);
            return (LRESULT)hBrush;
        }
        case WM_DESTROY: DeleteObject(hFont); PostQuitMessage(0); return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);

    const char CLASS_NAME[] = "UBC_Class_Modern";
    WNDCLASS wc = {}; wc.lpfnWndProc = WindowProc; wc.hInstance = hInstance; wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Universal Binary Converter (Windows)", (WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX) | WS_CAPTION, CW_USEDEFAULT, CW_USEDEFAULT, 500, 410, NULL, NULL, hInstance, NULL);
    if (hwnd == NULL) return 0;
    ShowWindow(hwnd, nCmdShow);
    MSG msg = {}; while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    return 0;
}
