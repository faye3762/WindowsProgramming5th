#include<windows.h>
#include<tchar.h>
#include<math.h>
#include<stdlib.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	TCHAR szAppName[] = _T("EMF1");
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
		_T("Enhanced Metafile Demo #1"),
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


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam , LPARAM lParam)
{
	static HENHMETAFILE hemf;
	HDC hdc, hdcEMF;
	PAINTSTRUCT ps;
	RECT rect;

	switch (message)
	{
	case WM_CREATE:
		hdcEMF = CreateEnhMetaFile(NULL, NULL, NULL, NULL);

		Rectangle(hdcEMF, 100, 100, 200, 200);
		MoveToEx(hdcEMF, 100, 100, NULL);
		LineTo(hdcEMF, 200, 200);
		MoveToEx(hdcEMF, 200,100, NULL);
		LineTo(hdcEMF, 100, 200);

		hemf = CloseEnhMetaFile(hdcEMF);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		GetClientRect(hwnd, &rect);
		rect.left = rect.right / 4;
		rect.right = 3 * rect.right / 4;
		rect.top = rect.bottom / 4;
		rect.bottom = 3 * rect.bottom / 4;

		PlayEnhMetaFile(hdc, hemf, &rect);

		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		DeleteEnhMetaFile(hemf);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}