#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "gdiplus.lib")
#include <windows.h>
#include <windowsx.h>
#include <GdiPlus.h>
#include "myObjectList.h"
#include "PictureFrame.h"

TCHAR szClassName[] = TEXT("Window");
myObjectList * pDataList;
PictureFrame * picfrm;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit;
	static HWND hButton1;
	static HWND hButton2;
	static HWND hButton3;
	static HWND hButton4;
	static HWND hButton5;
	static HWND hButton6;
	static HWND hButton7;
	static HWND hButton8;
	static HWND hButton9;
	static HBITMAP hBitmap;
	static HDC hdcBitmap;
	static BOOL bMouseDown;
	static INT width;
	static INT height;
	static COLORREF color;
	static HPEN hPen;
	static HPEN hOldPen;
	switch (msg)
	{
	case WM_CREATE:
		hButton1 = CreateWindow(TEXT("BUTTON"), TEXT("クリア"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)100, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton2 = CreateWindow(TEXT("BUTTON"), TEXT("保存"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)101, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton3 = CreateWindow(TEXT("BUTTON"), TEXT("黒"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, 0, 0, 0, 0, hWnd, (HMENU)102, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton4 = CreateWindow(TEXT("BUTTON"), TEXT("赤"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, 0, 0, 0, 0, hWnd, (HMENU)103, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton5 = CreateWindow(TEXT("BUTTON"), TEXT("緑"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, 0, 0, 0, 0, hWnd, (HMENU)104, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton6 = CreateWindow(TEXT("BUTTON"), TEXT("青"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, 0, 0, 0, 0, hWnd, (HMENU)105, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton7 = CreateWindow(TEXT("BUTTON"), TEXT("灰"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, 0, 0, 0, 0, hWnd, (HMENU)106, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton8 = CreateWindow(TEXT("BUTTON"), TEXT("ピンク"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, 0, 0, 0, 0, hWnd, (HMENU)107, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton9 = CreateWindow(TEXT("BUTTON"), TEXT("オレンジ"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, 0, 0, 0, 0, hWnd, (HMENU)108, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("16"), WS_VISIBLE | WS_CHILD | ES_NUMBER | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, (HMENU)109, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SendMessage(hButton3, BM_SETCHECK, BST_CHECKED, 0);
		break;
	case WM_SIZE:
		MoveWindow(hButton1, 0, 0, 64, 64, TRUE);
		MoveWindow(hButton2, 64, 0, 64, 64, TRUE);
		MoveWindow(hButton3, 128, 0, 64, 64, TRUE);
		MoveWindow(hButton4, 192, 0, 64, 64, TRUE);
		MoveWindow(hButton5, 256, 0, 64, 64, TRUE);
		MoveWindow(hButton6, 320, 0, 64, 64, TRUE);
		MoveWindow(hButton7, 384, 0, 64, 64, TRUE);
		MoveWindow(hButton8, 448, 0, 64, 64, TRUE);
		MoveWindow(hButton9, 512, 0, 64, 64, TRUE);
		MoveWindow(hEdit, 512 + 64, 0, 64, 64, TRUE);
		SendMessage(hWnd, WM_APP, 0, 0);
		{
			width = LOWORD(lParam);
			height = HIWORD(lParam);

			//ビットマップの作成
			const HDC hdc = GetDC(hWnd);
			hdcBitmap = CreateCompatibleDC(hdc);
			hBitmap = CreateCompatibleBitmap(hdc, width, height);
			SelectObject(hdcBitmap, hBitmap);
			ReleaseDC(hWnd, hdc);

			PatBlt(hdcBitmap, 0, 0, width, height, WHITENESS);
		}
		break;
	case WM_LBUTTONDOWN:
		if (!bMouseDown)
		{
			SetCapture(hWnd);
			const int nPenWidth = GetDlgItemInt(hWnd, 109, 0, 0);
			hPen = CreatePen(PS_SOLID, nPenWidth, color);
			hOldPen = (HPEN)SelectObject(hdcBitmap, hPen);
			MoveToEx(hdcBitmap, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0);
			bMouseDown = TRUE;
		}
		break;
	case WM_MOUSEMOVE:
		if (bMouseDown)
		{
			LineTo(hdcBitmap, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			InvalidateRect(hWnd, 0, 0);
		}
		break;
	case WM_LBUTTONUP:
		if (bMouseDown)
		{
			ReleaseCapture();
			SelectObject(hdcBitmap, hOldPen);
			bMouseDown = FALSE;
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			RECT rect;
			GetClientRect(hWnd, &rect);
			if (hdcBitmap)
			{
				BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcBitmap, 0, 0, SRCCOPY);
			}
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 100:
			PatBlt(hdcBitmap, 0, 0, width, height, WHITENESS);
			InvalidateRect(hWnd, 0, 0);
			break;
		case 101:
			{

			}
			break;
		case 102:
			color = RGB(0, 0, 0);
			break;
		case 103:
			color = RGB(255, 0, 0);
			break;
		case 104:
			color = RGB(0, 255, 0);
			break;
		case 105:
			color = RGB(0, 0, 255);
			break;
		case 106:
			color = RGB(128, 128, 128);
			break;
		case 107:
			color = RGB(255, 128, 128);
			break;
		case 108:
			color = RGB(255, 165, 0);
			break;
		}
		break;
	case WM_APP:
		if (hdcBitmap)
		{
			DeleteDC(hdcBitmap);
			hdcBitmap = NULL;
			DeleteObject(hBitmap);
			hBitmap = NULL;
		}
		break;
	case WM_DESTROY:
		SendMessage(hWnd, WM_APP, 0, 0);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPWSTR pCmdLine, int nCmdShow)
{
	ULONG_PTR gdiToken;
	Gdiplus::GdiplusStartupInput gdiSI;
	Gdiplus::GdiplusStartup(&gdiToken, &gdiSI, NULL);
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("Objective Paint"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	Gdiplus::GdiplusShutdown(gdiToken);
	return (int)msg.wParam;
}
