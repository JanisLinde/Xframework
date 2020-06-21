#include <windows.h>

#include <xframework.h>

LPCTSTR WndClassName = L"firstwindow";
HWND hwnd = NULL; 

const int width = 800; 
const int height = 600;

bool InitializeWindow(HINSTANCE hInstance, 
    int ShowWnd,
    int width, int height,
    bool windowed);

int messageloop();

LRESULT CALLBACK WndProc(HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    if (!InitializeWindow(hInstance, nShowCmd, width, height, true))
    {
        MessageBox(0, 
            L"Window Initialization - Failed",
            L"Error", MB_OK);
        return 0;
    }

    messageloop();

    return 0;
}

bool InitializeWindow(HINSTANCE hInstance, 
    int ShowWnd,
    int width, int height,
    bool windowed)
{
    WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);    //Size of our windows class
    wc.style = CS_HREDRAW | CS_VREDRAW;    //class styles
    wc.lpfnWndProc = WndProc;    //Default windows procedure function
    wc.cbClsExtra = NULL;    //Extra bytes after our wc structure
    wc.cbWndExtra = NULL;    //Extra bytes after our windows instance
    wc.hInstance = hInstance;    //Instance to current application
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);    //Title bar Icon
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);    //Default mouse Icon
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);    //Window bg color
    wc.lpszMenuName = NULL;    //Name of the menu attached to our window
    wc.lpszClassName = WndClassName;    //Name of our windows class
    wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO); //Icon in your taskbar

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, 
            L"Error registering class",
            L"Error", 
            MB_OK | MB_ICONERROR);
        return false;
    }

    hwnd = CreateWindowEx(    //Create our Extended Window
        NULL,    //Extended style
        WndClassName,    //Name of our windows class
        L"Xframework - Test",    //Name in the title bar of our window
        WS_OVERLAPPEDWINDOW,    //style of our window
        CW_USEDEFAULT, CW_USEDEFAULT,    //Top left corner of window
        width,    //Width of our window
        height,    //Height of our window
        NULL,    //Handle to parent window
        NULL,    //Handle to a Menu
        hInstance,    //Specifies instance of current program
        NULL    //used for an MDI client window
    );

    if (!hwnd)    //Make sure our window has been created
    {
        //If not, display error
        MessageBox(NULL, 
            L"Error creating window",
            L"Error",
            MB_OK | MB_ICONERROR);
        return false;
    }

    ShowWindow(hwnd, ShowWnd);    //Shows our window
    UpdateWindow(hwnd);    //Its good to update our window

    return true;    //if there were no errors, return true
}

int messageloop() 
{

    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    xframework xfw;
    xfw.Initialize(hwnd, width, height);

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
            xfw.BeginScene(0.25f, 0.5f, 0.0f);

            xfw.EndScene();
        }


    }

    xfw.Shutdown();

    return (int)msg.wParam; 

}

LRESULT CALLBACK WndProc(HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (msg)
    {

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            if (MessageBox(0, 
                L"Are you sure you want to exit?",
                L"Really?", 
                MB_YESNO | MB_ICONQUESTION) == IDYES)
                DestroyWindow(hwnd);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}