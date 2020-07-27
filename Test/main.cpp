#include <windows.h>

#include <xframework.h>

LPCTSTR WndClassName = L"xfwTest";

const int width = 800; 
const int height = 600;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW; 
    wc.lpfnWndProc = WndProc; 
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = WndClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    if (!RegisterClassEx(&wc))
        return -1;

    HWND hwnd = CreateWindowEx(NULL,  WndClassName, L"Xframework - Test", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height, NULL, NULL, hInstance,  NULL );

    if (!hwnd)
        return -2;

    ShowWindow(hwnd, nShowCmd);
    UpdateWindow(hwnd);

    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    xframework xfw;
    if (!xfw.Initialize(hInstance, hwnd, width, height))
        return -3;

    ShowCursor(false);

    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            xfw.Update();

            if (xfw.InputIsKeyPressed(DIK_ESCAPE))
                break;

            // Do Xframework stuff here
            xfw.BeginScene(0.8f, 0.8f, 0.8f);

            xfw.DrawTriangle(
                DirectX::XMFLOAT3(-0.4f, 0.5f, 0.5f),
                DirectX::XMFLOAT3(0.0f, -0.5f, 0.5f),
                DirectX::XMFLOAT3(-0.8f, -0.5f, 0.5f),
                DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0)
            );
            
            xfw.DrawTriangle(
                DirectX::XMFLOAT3(0.4f, 0.5f, 0.5f),
                DirectX::XMFLOAT3(0.8f, -0.5f, 0.5f),
                DirectX::XMFLOAT3(0.0f, -0.5f, 0.5f),
                DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)
            );

            // xfw.DrawPoint(0.0f, 0.9f, 0.0f, 1.0f, 0.0f, 1.0f);
            // xfw.DrawLine(1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f);

            xfw.EndScene();
        }
    }

    xfw.Shutdown();
    ShowCursor(true);

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}