#include "Window.h"

#define WINDOW_CLASS_NAME "Playground_Windows"
#define CONTENT_WIDTH  1000
#define CONTENT_HEIGHT 600

LRESULT CALLBACK WindowProcFunc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_DESTROY)
    {
        PostMessage(hWnd, WM_QUIT, NULL, NULL);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

Window::Window()
{
    HINSTANCE instance = GetModuleHandle(NULL);
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProcFunc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = WINDOW_CLASS_NAME;

    RegisterClassEx(&wc);

    int widthExpand = GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
    int heightExpand = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER);

    _hwnd = CreateWindowEx(NULL, //dwExStyle
                           WINDOW_CLASS_NAME,
                           "Playground",
                           WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT,
                           CW_USEDEFAULT,
                           CONTENT_WIDTH + widthExpand,
                           CONTENT_HEIGHT + heightExpand,
                           NULL, //hWndParent
                           NULL, //hMenu
                           instance,
                           NULL);
    _hdc = GetDC(_hwnd);
    SetWindowLongPtr(_hwnd, GWLP_USERDATA, NULL);
    ShowWindow(_hwnd, SW_SHOW);

    _canvas = std::make_shared<VulkanCanvas>(_hwnd);
}

Window::~Window()
{
    _hwnd = NULL;
    _hdc  = NULL;
}

int Window::run()
{
    MSG msg = {};

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (_canvas)
        {
            _canvas->beginPaint(_mvp);
            _canvas->drawModels(_models, true);
            _canvas->endPaint();
            _canvas->display();
        }
    }

    return (int)msg.wParam;
}