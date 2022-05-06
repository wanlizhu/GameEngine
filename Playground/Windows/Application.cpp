#include "Application.h"

#define WINDOW_CLASS_NAME "Playground_Windows"
#define CONTENT_WIDTH  1000
#define CONTENT_HEIGHT 600

LRESULT CALLBACK WindowProcFunc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    OpenWorld* world = (OpenWorld*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    
    if (world) 
    {
        MessageParameter param = std::make_pair(wParam, lParam);
        world->messageDidReceive(message, param);

        if (message == WM_SIZE)
        {
            world->resizeView((void*)hWnd);
        }

        if (message == WM_PAINT)
        {
            world->drawInView((void*)hWnd);
        }
    }

    if (message == WM_DESTROY) 
    {
        PostMessage(hWnd, WM_QUIT, NULL, NULL);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

Application::Application()
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
                           CONTENT_WIDTH  + widthExpand,
                           CONTENT_HEIGHT + heightExpand,
                           NULL, //hWndParent
                           NULL, //hMenu
                           instance,
                           NULL);
    _dc = GetDC(_hwnd);

    _world = std::make_shared<OpenWorld>();
    _world->initWithView((void*)_hwnd);
    SetWindowLongPtr(_hwnd, GWLP_USERDATA, (LONG_PTR)(_world ? _world.get() : NULL));

    ShowWindow(_hwnd, SW_SHOW);
}

Application::~Application()
{
    _hwnd = NULL;
    _dc = NULL;
}

int Application::run()
{
    MSG msg = {};
    
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}