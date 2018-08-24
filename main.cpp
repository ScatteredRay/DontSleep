#include <windows.h>
#include <stdio.h>

static char szAppName[] = "dontsleep";
static char szWinName[] = "Don't Sleep";

static WCHAR szPowerRequest[] = L"DontSleep is running.";

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC         hdc;

    switch ( iMsg ) {
    case WM_TIMER:
    {
        bool success = SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED | ES_DISPLAY_REQUIRED) != 0;
        if(success) {
            OutputDebugString("Success\n");
        }
        else {
            OutputDebugString("Failure\n");
        }
        return 0;
    }
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        TextOut(hdc, 100, 100, szWinName, 13);
        EndPaint(hwnd, &ps);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow) {
    HWND        hwnd;
    MSG         msg;
    WNDCLASSEX  wndclass;

    wndclass.cbSize         = sizeof(wndclass);
    wndclass.style          = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc    = WndProc;
    wndclass.cbClsExtra     = 0;
    wndclass.cbWndExtra     = 0;
    wndclass.hInstance      = hInstance;
    wndclass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground  = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndclass.lpszClassName  = szAppName;
    wndclass.lpszMenuName   = NULL;

    RegisterClassEx(&wndclass);

    hwnd = CreateWindow(szAppName, szWinName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    REASON_CONTEXT reas;

    reas.Version = POWER_REQUEST_CONTEXT_VERSION;
    reas.Flags = POWER_REQUEST_CONTEXT_SIMPLE_STRING;
    reas.Reason.SimpleReasonString = szPowerRequest;

    HANDLE pwrH = PowerCreateRequest(&reas);
    if(pwrH == 0)
        return -1;

    if(PowerSetRequest(pwrH, PowerRequestAwayModeRequired) == 0)
        return -1;

    SetTimer(hwnd, 1, 1000, (TIMERPROC) NULL);

    while ( GetMessage(&msg, NULL, 0, 0) ) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    SetThreadExecutionState(ES_CONTINUOUS);

    if(PowerClearRequest(pwrH, PowerRequestAwayModeRequired) == 0)
        return -1;

    CloseHandle(pwrH);

    return msg.wParam;
}
