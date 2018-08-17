//MSVC not Support UTF-8 without BOM
#include "stdafx.h"
#include "WiFiAssistant.h"
#include "AssistantProcess.h"
#include "AssistantExtension.h"
#include "LimitEdit.h"
#include <Winternl.h>
#include <assert.h>
#include "WirelessErrorType.h"

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
bool IsUacSelf = false;

//Globla Value
namespace Global{
	std::wstring wifixml;
	bool IsShowPsd=false;
	WiFiNetInfoW wifiifo;
	bool IsAdmin = false;	
	int nShow;
	bool IsRunsFirst = false;
}

//Button_SetElevationRequiredState()

namespace WndStyle{
	DWORD dwBt = WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT;
	DWORD dwBtEx = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY;
	DWORD dwCkBt = BS_PUSHBUTTON | BS_TEXT | BS_DEFPUSHBUTTON | BS_CHECKBOX | BS_AUTOCHECKBOX | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE;
	DWORD dwCkBtEx = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
	DWORD dwEdit = WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL;
	DWORD dwEditEx = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE;
	DWORD dwSt = WS_CHILDWINDOW | WS_VISIBLE | WS_GROUP | SS_LEFT;
	DWORD dwStEx = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_TRANSPARENT | WS_EX_NOPARENTNOTIFY;
	DWORD dwupd = UDS_ARROWKEYS | UDS_SETBUDDYINT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE ;
	DWORD dwupdEx = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
	DWORD dwpgEx = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY;
	DWORD dwpg = WS_CHILDWINDOW | WS_VISIBLE;
}

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

BOOL VECTORCALL InitGlobalValue();
BOOL VECTORCALL SavaGlobalValue();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
#if defined(_MSC_VER)&&_MSC_VER>=1800
	if (!IsWindows7OrGreater())
	{
		MessageBox(NULL, L"Your OS is not Support!", L"ErrorShow", MB_OKCANCEL | MB_ICONERROR);
		return 1;
	}
#else
	OSVERSIONINFO osvi;
	BOOL bIsWindows7orLater;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	bIsWindows7orLater =
		((osvi.dwMajorVersion > 6) ||
		((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion >= 1)));
	if (!bIsWindows7orLater)
	{
		MessageBox(NULL, L"Your OS is not Support!", L"ErrorShow", MB_OKCANCEL | MB_ICONERROR);
		return 1;
	}
#endif
	if (IsUserAnAdmin())
	{
		//MessageBox(NULL, L"Yes!", L"Administrator", MB_OK);
		Global::IsAdmin = true;
	}
	/**Mutex Test Start:*/
	SetLastError(NO_ERROR);
	HANDLE hMutex = CreateMutex(NULL, FALSE, _T("WiFiAssistant"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		if (__argc != 2 || _wcsicmp(__wargv[1],L"-uac")!=0)
		{
			CloseHandle(hMutex);
			hMutex = nullptr;
			return 0;
		}
		//GetCurrentProcessID()
	}


	Global::nShow = nCmdShow;

 	// TODO:  在此放置代码。
	MSG msg;
	HACCEL hAccelTable;
	InitGlobalValue();
	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIFIASSISTANT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	//AreThereFindWirelessNetworkAdapter();
	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIFIASSISTANT));

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	//UnregisterClass(szWindowClass,hInstance);
	if (IsUacSelf==false)
		SavaGlobalValue();
	CloseHandle(hMutex);
	hMutex = nullptr;
	return (int) msg.wParam;
}



//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIFIASSISTANT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= nullptr/*MAKEINTRESOURCE(IDC_WIFIASSISTANT)*/;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindowEx(WS_EX_APPWINDOW, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW&~WS_MAXIMIZEBOX&~WS_THICKFRAME,
	   (GetSystemMetrics(SM_CXSCREEN) - 450) / 2, (GetSystemMetrics(SM_CYSCREEN) - 480) / 2, 450, 480, NULL, NULL, hInstance, NULL);

   if (Global::IsAdmin)
   {
	   WCHAR szTitle[120] = { 0 };
	   GetWindowText(hWnd, szTitle, 120);
	   wcscat_s(szTitle, L" -[管理员]");
	   SetWindowText(hWnd,szTitle);
   }
   if (!hWnd)
   {
      return FALSE;
   }
   HMENU SysMenu = GetSystemMenu(hWnd, FALSE);
   InsertMenu(SysMenu, SC_CLOSE, MF_ENABLED, IDM_SYS_ABOUT, L"关于WiFiAssistant(&A)\tAlt+F1");
   //EnableMenuItem
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	//Shell_NotifyIcon

	//Get GUI Font
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT logFont = { 0 };
	GetObject(hFont, sizeof(logFont), &logFont);
	DeleteObject(hFont);
	hFont = nullptr;
#if defined(_MSC_VER)&&_MSC_VER>=1800
	if (IsWindows8OrGreater()){
		logFont.lfHeight = 18;
		logFont.lfWeight = FW_NORMAL;
		wcscpy_s(logFont.lfFaceName, L"Microsoft Yahei UI");
	}
	else{
		logFont.lfHeight = 15;
		logFont.lfWeight = FW_NORMAL;
		wcscpy_s(logFont.lfFaceName, L"MS Shell Dlg");
	}
#else
	OSVERSIONINFO osvi;
	BOOL bIsWindows8orLater;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	bIsWindows8orLater =
		((osvi.dwMajorVersion > 6) ||
		((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion >= 2)));
	if (!bIsWindows8orLater)
	{
		logFont.lfHeight = 15;
		logFont.lfWeight = FW_NORMAL;
		wcscpy_s(logFont.lfFaceName, L"MS Shell Dlg");
	}
	else{
		logFont.lfHeight = 18;
		logFont.lfWeight = FW_NORMAL;
		wcscpy_s(logFont.lfFaceName, L"Microsoft Yahei UI");
	}
#endif
	hFont = CreateFontIndirect(&logFont);

	switch (message)
	{
	case WM_CREATE:
	{
		//msctls_updown32
		InitCommonControls();
		INITCOMMONCONTROLSEX icx = { sizeof(INITCOMMONCONTROLSEX), ICC_WIN95_CLASSES };
		InitCommonControlsEx(&icx);

		HWND hOnK = CreateWindowEx(WndStyle::dwBtEx, WC_BUTTON, L"一键启动虚拟WiFi", WndStyle::dwBt, 110, 50, 200, 30, hWnd, HMENU(IDC_BT_ONEAUTO), hInst, nullptr);
		HWND hStSSID = CreateWindowEx(WndStyle::dwStEx, WC_STATIC, L"WiFi标识:", WndStyle::dwSt, 30, 90, 70, 25, hWnd, HMENU(IDC_ST_SSID), hInst, nullptr);
		HWND hStKEY = CreateWindowEx(WndStyle::dwStEx, WC_STATIC, L"WiFi密码:", WndStyle::dwSt, 30, 120, 70, 25, hWnd, HMENU(IDC_ST_KEY), hInst, nullptr);
		HWND hEditSsid = CreateWindowEx(WndStyle::dwEditEx, WC_EDIT, L"", WndStyle::dwEdit, 110, 90, 200, 25, hWnd, HMENU(IDC_EDIT_SSID), hInst, nullptr);
		HWND hBTClose = CreateWindowEx(WndStyle::dwBtEx, WC_BUTTON, L"关闭WiFi", WndStyle::dwBt, 110, 160, 95, 30, hWnd, HMENU(IDC_BT_CLOSE), hInst, nullptr);
		//CheckBox SendMessage to CheckBox BM_GETCHECK BST_UNCHECKED/BST_CHECKED
		HWND hBTCheck = CreateWindowEx(WndStyle::dwCkBtEx, WC_BUTTON, L"显示密码", WndStyle::dwCkBt, 320, 120, 80, 30, hWnd, HMENU(IDC_CHECKBT_PAW), hInst, nullptr);
		HWND hBTOpen = CreateWindowEx(WndStyle::dwBtEx, WC_BUTTON, L"开启WiFi", WndStyle::dwBt, 215, 160, 95, 30, hWnd, HMENU(IDC_BT_OPEN), hInst, nullptr);

		RECT rect = { 110, 120, 310, 145 };
		HWND hLimit = CreateLimitEditW(L"", rect, hWnd, IDC_LIMIT_EDIT, hInst, nullptr, true);


		HWND hProgress = CreateWindowEx(WndStyle::dwpgEx, PROGRESS_CLASS, L"", WndStyle::dwpg, 110, 200, 200, 20, hWnd, HMENU(IDC_PROGRESS_SHOW), hInst, nullptr);

		//Creat Time Control 3Edit 1unclass
		HWND hSTime = CreateWindowEx(WndStyle::dwStEx, WC_STATIC, L"时间[d|h|min]:", WndStyle::dwSt, 110, 250, 100, 25, hWnd, HMENU(IDC_ST_TIME), hInst, nullptr);
		HWND hETimeD = CreateWindowEx(WndStyle::dwEditEx, WC_EDIT, L"0", WndStyle::dwEdit | ES_NUMBER&~ES_LEFT | ES_RIGHT, 110, 280, 30, 25, hWnd, HMENU(IDC_EDIT_TIMEDAY), hInst, nullptr);
		HWND hETimeH = CreateWindowEx(WndStyle::dwEditEx, WC_EDIT, L"0", WndStyle::dwEdit | ES_NUMBER&~ES_LEFT | ES_RIGHT, 140, 280, 25, 25, hWnd, HMENU(IDC_EDIT_TIMEHOR), hInst, nullptr);
		HWND hETimeM = CreateWindowEx(WndStyle::dwEditEx, WC_EDIT, L"0", WndStyle::dwEdit | ES_NUMBER&~ES_LEFT | ES_RIGHT, 165, 280, 25, 25, hWnd, HMENU(IDC_EDIT_TIMEMIN), hInst, nullptr);

		HWND hUpDown = CreateUpDownControl(WndStyle::dwupd, 190, 279, 20, 27, hWnd, IDC_UPDOWN_TIME, hInst, hETimeM, 60, 0, 60);
		//HWND hUDDay = CreateWindowEx(WndStyle::dwupdEx, UPDOWN_CLASS, L"", WndStyle::dwupd, 190, 280, 20, 25, hWnd, HMENU(IDC_UPDOWN_TIME), hInst, nullptr);
		HWND hBTShut = CreateWindowEx(WndStyle::dwBtEx, WC_BUTTON, L"定时关机", WndStyle::dwBt, 210, 280, 100, 27, hWnd, HMENU(IDC_BT_SETTIMESHUT), hInst, nullptr);
		HWND hBTCShut = CreateWindowEx(WndStyle::dwBtEx, WC_BUTTON, L"取消定时关机", WndStyle::dwBt, 210, 250, 100, 27, hWnd, HMENU(IDC_BT_CANCELTIMESHUT), hInst, nullptr);
		HWND hRunAs = CreateWindowEx(WndStyle::dwBtEx, WC_BUTTON, L"", WndStyle::dwBt, 110, 325, 200, 30, hWnd, HMENU(IDC_BT_RUNAS), hInst, nullptr);



		SendMessage(hEditSsid, EM_SETLIMITTEXT, (WPARAM) 64, lParam);
		SendMessage(GetDlgItem(hWnd, IDC_LIMIT_EDIT), EM_SETLIMITTEXT, (WPARAM) 64, lParam);

		SendMessage(hOnK, WM_SETFONT, (WPARAM) hFont, lParam);
		SendMessage(hStSSID, WM_SETFONT, (WPARAM) hFont, lParam);
		SendMessage(hStKEY, WM_SETFONT, (WPARAM) hFont, lParam);

		SendMessage(hEditSsid, WM_SETFONT, (WPARAM) hFont, lParam);

		SendMessage(hBTClose, WM_SETFONT, (WPARAM) hFont, lParam);
		SendMessage(hBTCheck, WM_SETFONT, (WPARAM) hFont, lParam);
		SendMessage(hBTOpen, WM_SETFONT, (WPARAM) hFont, lParam);
		SendMessage(GetDlgItem(hWnd, IDC_LIMIT_EDIT), WM_SETFONT, (WPARAM) hFont, lParam);

		SendMessage(hSTime, WM_SETFONT, (WPARAM) hFont, lParam);
		SendMessage(hETimeD, WM_SETFONT, (WPARAM) hFont, lParam);
		SendMessage(hETimeH, WM_SETFONT, (WPARAM) hFont, lParam);
		SendMessage(hETimeM, WM_SETFONT, (WPARAM) hFont, lParam);
		SendMessage(hBTShut, WM_SETFONT, (WPARAM) hFont, lParam);
		SendMessage(hBTCShut, WM_SETFONT, (WPARAM) hFont, lParam);
		SendMessage(hRunAs, WM_SETFONT, (WPARAM) hFont, lParam);
		Button_SetElevationRequiredState(hRunAs, !Global::IsAdmin);

		SendMessage(hEditSsid, EM_SETLIMITTEXT, (WPARAM) 32, lParam);
		SendMessage(hETimeM, EM_SETLIMITTEXT, (WPARAM) 2, lParam);
		SendMessage(hETimeH, EM_SETLIMITTEXT, (WPARAM) 2, lParam);
		SendMessage(hETimeD, EM_SETLIMITTEXT, (WPARAM) 3, lParam);

		if (Global::wifiifo.IsShowPwd == true)
		{
			Button_SetCheck(hBTCheck, BST_CHECKED);
			SetWindowText(GetDlgItem(hWnd, IDC_LIMIT_EDIT), Global::wifiifo.KEY);
			SendMessage(hWnd, WM_COMMAND, (WPARAM) IDC_CHECKBT_PAW, lParam);
		}
		if (Global::IsAdmin)
		{
			
			if (AreThereFindWirelessNetworkAdapter() != S_OK)
			{
				EnableWindow(hBTOpen, FALSE);
				EnableWindow(hBTClose, FALSE);
				EnableWindow(hOnK, FALSE);
			}
			EnableWindow(hRunAs, FALSE);
			SetWindowText(hRunAs, L"已是管理员权限");
			
		}
		else
		{
			SetWindowText(hRunAs, L"获取管理员权限");
			HWND hInfo = CreateWindowEx(WndStyle::dwStEx, WC_STATIC, L"WiFi助手需要提升权限,请点击\"获取管理员权限\"按钮", WndStyle::dwSt, 110, 365, 200, 40, hWnd, nullptr, hInst, nullptr);
			SendMessage(hInfo, WM_SETFONT, (WPARAM) hFont, lParam);
			EnableWindow(hBTOpen, FALSE);
			EnableWindow(hBTClose, FALSE);
			EnableWindow(hOnK, FALSE);
		}
		SetWindowText(hEditSsid, Global::wifiifo.SSID);
		SetWindowText(hLimit, Global::wifiifo.KEY);
		
	}
	break;
	case WM_SYSCOMMAND:
	{
		int wsId = LOWORD(wParam);
		int wsEvent = HIWORD(wParam);
		switch (wsId)
		{
		case IDM_SYS_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDC_BT_ONEAUTO:
		{
			if (!Global::IsAdmin)
			{
				MessageBox(hWnd, TEXT("Please Run as Administrator!"), TEXT("Warning throw"), MB_OK | MB_ICONWARNING);
				break;
			}
			WCHAR Ermsg[512] = { 0 };
			LRESULT lr = WirelessBearerNetworkInvokeOneKeyStart(&Global::wifiifo);
			if (lr != S_OK)
			{
				GetErrorMessageString(lr, Ermsg);
				MessageBox(hWnd, Ermsg, L"Error throw", MB_YESNO | MB_ICONERROR);
			}
			else{
				SetWindowText(GetDlgItem(hWnd, IDC_EDIT_SSID), Global::wifiifo.SSID);
				SetWindowText(GetDlgItem(hWnd, IDC_LIMIT_EDIT), Global::wifiifo.KEY);
				MessageBox(hWnd, L"One key to start a successful virtual WiFi!", L"News Tip", MB_OK | MB_ICONINFORMATION);
			}
		}
			break;
		case IDC_CHECKBT_PAW:
		{
			HWND hEdKey = GetDlgItem(hWnd, IDC_LIMIT_EDIT);
			if (Button_GetCheck(GetDlgItem(hWnd, IDC_CHECKBT_PAW)) == BST_CHECKED)
			{
				WCHAR text[32] = { 0 };
				GetWindowText(hEdKey, text, 32);
				SendMessage(GetDlgItem(hWnd, IDC_LIMIT_EDIT), WM_CLOSE, wParam, lParam);
				HWND hLimit = CreateWindowEx(WndStyle::dwEditEx, LimitEdit, L"", WndStyle::dwEdit, 110, 120, 200, 25, hWnd, HMENU(IDC_LIMIT_EDIT), hInst, nullptr);
				SetWindowText(hLimit, text);
				SendMessage(hLimit, EM_SETLIMITTEXT, (WPARAM) 32, lParam);
				SendMessage(hLimit, WM_SETFONT, (WPARAM) hFont, lParam);
				SendMessage(hEdKey, WVR_REDRAW, wParam, lParam);
			}
			else
			{
				WCHAR text[32] = { 0 };
				GetWindowText(hEdKey, text, 32);
				SendMessage(GetDlgItem(hWnd, IDC_LIMIT_EDIT), WM_CLOSE, wParam, lParam);
				HWND hLimit = CreateWindowEx(WndStyle::dwEditEx, LimitEdit, L"", WndStyle::dwEdit | ES_PASSWORD, 110, 120, 200, 25, hWnd, HMENU(IDC_LIMIT_EDIT), hInst, nullptr);
				SetWindowText(hLimit, text);
				SendMessage(hLimit, EM_SETLIMITTEXT, (WPARAM) 32, lParam);
			}
		}
			break;
		case IDC_BT_CLOSE:
		{
			//MessageBox(hWnd, L"CloseWiFi", L"Check", MB_OK);
			//WirelessNetworkInvokeAPIStop();
			//EnableWindow(GetDlgItem(hWnd, IDC_BT_CLOSE), FALSE);
			WCHAR Errorclose[512] = { 0 };
			LRESULT lrest = WirelessNetworkInvokeAPIStop();
			if (lrest != S_OK)
			{
				GetErrorMessageString(lrest, Errorclose);
				MessageBox(hWnd, Errorclose, L"Error throw", MB_OKCANCEL | MB_ICONERROR);
			}
			else
			{
				MessageBox(hWnd, L"Virtual WiFi has been closed!", L"News Tip", MB_OK | MB_ICONINFORMATION);
			}
		}
			break;
		case IDC_BT_OPEN:
		{
							//MessageBox(hWnd, L"OpenWiFi", L"Check", MB_OK);
			if (!Global::IsAdmin)
			{
				MessageBox(hWnd, TEXT("Please Run as Administrator!"), TEXT("Warning throw"), MB_OK | MB_ICONWARNING);
				break;
			}
			WCHAR Errormsg[512] = { 0 };
			GetWindowText(GetDlgItem(hWnd, IDC_EDIT_SSID), Global::wifiifo.SSID, 32);
			GetWindowText(GetDlgItem(hWnd, IDC_LIMIT_EDIT), Global::wifiifo.KEY, 32);
			LRESULT hr = WirelessNetworkInvokeAPIStart(&Global::wifiifo);
			if (hr != S_OK)
			{
				GetErrorMessageString(hr, Errormsg);
				MessageBox(hWnd, Errormsg, L"Error throw", MB_OKCANCEL | MB_ICONERROR);
			}
			else{
				MessageBox(hWnd, L"Virtual WiFi has been activated!", L"News Tip", MB_OK | MB_ICONINFORMATION);
			}
		}
		break;
		case IDC_BT_CANCELTIMESHUT:
		{
			 AssistantAbortShutdown();
		}
		break;
		case IDC_BT_SETTIMESHUT:
		{
			WCHAR cDt[4] = { 0 };
			WCHAR cHt[3] = { 0 };
			WCHAR cMt[3] = { 0 };
			GetWindowText(GetDlgItem(hWnd, IDC_EDIT_TIMEDAY), cDt, 4);
			GetWindowText(GetDlgItem(hWnd, IDC_EDIT_TIMEHOR), cHt, 3);
			GetWindowText(GetDlgItem(hWnd, IDC_EDIT_TIMEMIN), cMt, 3);
			//MessageBox(hWnd, cDt, cHt, MB_OK);
			SPTIME* spt = new SPTIME(cDt, cHt, cMt);
			UINT time = spt->GetTotalSecond();
			if (AssistantTimeShutdown(hWnd, time) != TRUE)
			{
				delete spt;
				break;
			}
			delete spt;
		}
		break;
		case IDC_UPDOWN_TIME:
		{
								//
								//MessageBox(hWnd, L"Up&Down", L"Check", MB_OK);
		}
			break;
		case IDC_BT_RUNAS:
		{
			//MessageBox(NULL, L"DQDQ", L"DE@D", MB_OK);
			if (!Global::IsAdmin)
			{
				UACSelfElevation(hWnd, Global::nShow);
				break;
			}
		}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_NOTIFY:
	{
					  //NM_UPDOWN
	}
		break;
	case WM_SYSKEYDOWN:
		switch (wParam)
		{
		case VK_F1:
			if (GetKeyState(VK_MENU) < 0)
				SendMessage(hWnd, WM_SYSCOMMAND, (WPARAM) IDM_SYS_ABOUT,lParam);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
    case WM_CTLCOLORSTATIC:
	  {
		::SetBkMode((HDC)wParam, OPAQUE);
		::SetBkColor((HDC)wParam, ::GetSysColor(COLOR_WINDOW));
		::SetTextColor((HDC)wParam, ::GetSysColor(COLOR_WINDOWTEXT));
		return (LRESULT)::GetStockObject(WHITE_BRUSH);
	   }
	case WM_DESTROY:
	{
		//Process 
		if (Button_GetCheck(GetDlgItem(hWnd, IDC_CHECKBT_PAW)) == BST_CHECKED)
			Global::IsShowPsd = true;
		else
			Global::IsShowPsd = false;
		PostQuitMessage(0);
	}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

BOOL VECTORCALL InitGlobalValue()
{
	WiFiAssistantConfig wifiAsc;
	wifiAsc.InitXML(Global::wifixml);
	wifiAsc.GetWiFiConfig(&Global::wifiifo);
	//wifiAsc.XMLWrite(&Global::wifiifo);
	return TRUE;
}

BOOL VECTORCALL SavaGlobalValue()
{
	Global::wifiifo.IsShowPwd = Global::IsShowPsd;
	WiFiAssistantConfig wifiAsc;
	wifiAsc.InitXML(Global::wifixml);
	wifiAsc.XMLWrite(&Global::wifiifo);
	return TRUE;
}