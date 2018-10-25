#include<windows.h>
#include<tchar.h>
#include<math.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = _T("Sketch");
	HWND hWnd;
	MSG msg;
	WNDCLASS wndClass;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, _T("This Program requires Windows NT!"), szAppName, MB_ICONERROR);
		return -1;
	}

	hWnd = CreateWindow(szAppName,
		_T("Sketch"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void GetLargestDisplayMode(int* pcxBitmap, int* pcyBitmap)
{
	DEVMODE devmode;
	int iModeNum = 0;

	*pcxBitmap = *pcyBitmap = 0;

	ZeroMemory(&devmode, sizeof(DEVMODE));
	devmode.dmSize = sizeof(DEVMODE);

	while (EnumDisplaySettings(NULL, iModeNum++, &devmode))
	{
		*pcxBitmap = max(*pcxBitmap, (int)devmode.dmPelsWidth);
		*pcyBitmap = max(*pcyBitmap, (int)devmode.dmPelsHeight);
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam , LPARAM lParam)
{
	static BOOL fLeftButtonDown, fRightButtonDown;
	static HBITMAP hBitmap;
	static HDC hdcMem;
	static int cxBitmap, cyBitmap, cxClient, cyClient, xMouse, yMouse;
	HDC hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		GetLargestDisplayMode(&cxBitmap, &cyBitmap);

		hdc = GetDC(hwnd);
		hBitmap = CreateCompatibleBitmap(hdc, cxBitmap, cyBitmap);
		hdcMem = CreateCompatibleDC(hdc);
		ReleaseDC(hwnd, hdc);

		if (NULL == hBitmap)
		{
			DeleteDC(hdcMem);
			return -1;
		}

		SelectObject(hdcMem, hBitmap);
		PatBlt(hdcMem, 0, 0, cxBitmap, cyBitmap, WHITENESS);
		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;

	case WM_LBUTTONDOWN:
		if (!fRightButtonDown)
		{
			SetCapture(hwnd);
		}
		xMouse = LOWORD(lParam);
		yMouse = HIWORD(lParam);
		fLeftButtonDown = TRUE;
		return 0;

	case WM_LBUTTONUP:
		if (fLeftButtonDown)
		{
			SetCapture(NULL);
		}
		fLeftButtonDown = FALSE;
		return 0;

	case WM_RBUTTONDOWN:
		if (!fLeftButtonDown)
		{
			SetCapture(hwnd);
		}
		xMouse = LOWORD(lParam);
		yMouse = HIWORD(lParam);
		fRightButtonDown = TRUE;

		return 0;
	case WM_RBUTTONUP:
		if (fRightButtonDown)
		{
			SetCapture(NULL);
		}
		fRightButtonDown = FALSE;
		return 0;

	case WM_MOUSEMOVE:
		if (!fLeftButtonDown && !fRightButtonDown)
		{
			return 0;
		}
		hdc = GetDC(hwnd);
		SelectObject(hdc, GetStockObject(fLeftButtonDown ? BLACK_PEN : WHITE_PEN));
		SelectObject(hdcMem, GetStockObject(fLeftButtonDown ? BLACK_PEN : WHITE_PEN));

		MoveToEx(hdc, xMouse, yMouse, NULL);
		MoveToEx(hdcMem, xMouse, yMouse, NULL);
		xMouse = (short)(LOWORD(lParam));
		yMouse = (short)(HIWORD(lParam));
		LineTo(hdc, xMouse, yMouse);
		LineTo(hdcMem, xMouse, yMouse);

		ReleaseDC(hwnd, hdc);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		BitBlt(hdc, 0, 0, cxClient, cyClient, hdcMem, 0, 0, SRCCOPY);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		DeleteDC(hdcMem);
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
