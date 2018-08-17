/*************************************************************************************************************************************
* LimitEdit Super class
*
*
*
**************************************************************************************************************************************/
#include "stdafx.h"
#include "LimitEdit.h"
#include <string>

LRESULT(CALLBACK* OldEditWndProc)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK LimitEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CHAR:
	{
					//if return;
					TCHAR ch = (TCHAR) wParam;
					if (!(ch >= _T('!') && ch <= _T('~')||ch == VK_BACK))
						return 0;
	}
		break;
	case WM_PASTE:
	{
					 OpenClipboard(hWnd);
					 HGLOBAL hMem = GetClipboardData(CF_UNICODETEXT);
					 if (hMem)
					 {
						LPWSTR lpstr=(LPWSTR)GlobalLock(hMem) ;
						GlobalUnlock(hMem);
						if (lpstr != nullptr)
						{
							std::wstring wstr=lpstr;
							GlobalUnlock(hMem);
							//MessageBox(hWnd, wstr.c_str(), L"Paste", MB_OK);
							if (wstr.length() >= 32)
							{
								return 0;
							}
							//check wstr;
						}
					 }
					 CloseClipboard();
					 //GetClipboardData
	}break;
    case WM_MOUSEMOVE:
		break;
	case WM_LBUTTONUP:
		break;
	default:
		break;
	}
	return CallWindowProc(OldEditWndProc, hWnd, message, wParam, lParam);
}

BOOL RegisterLimitEdit()
{
	WNDCLASSEX LimitEditEx;
	ZeroMemory(&LimitEditEx, sizeof(LimitEditEx));
	LimitEditEx.cbSize = sizeof(WNDCLASSEX);
	GetClassInfoEx(0, TEXT("EDIT"), &LimitEditEx);
	OldEditWndProc = LimitEditEx.lpfnWndProc;
	LimitEditEx.lpfnWndProc = LimitEditProc;
	//PreEditEx.lpszMenuName=
	LimitEditEx.lpszClassName = TEXT("LimitEdit");
	RegisterClassEx(&LimitEditEx);
	return TRUE;
}

HWND CreateLimitEditW(LPWSTR text, RECT rect, HWND hWndParat, const LONG_PTR Id, HINSTANCE hInst, LPVOID lpParam,bool Ispwd)
{
	DWORD dwLimitEx = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE;
	DWORD dwLimit = WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL;
	if (!RegisterLimitEdit())
		return nullptr;
	if (Ispwd)
		dwLimit |= ES_PASSWORD;
	LONG_PTR hId;
	if (Id <= 0)
		hId = 0;
	else
		hId = Id;
	return CreateWindowEx(dwLimitEx, LimitEdit, text, dwLimit, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, hWndParat, (HMENU) hId, hInst, lpParam);
}
