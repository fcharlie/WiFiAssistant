#include "stdafx.h"
#include <algorithm> 
#include "AssistantExtension.h"


SPTIME::SPTIME(PWSTR pDay, PWSTR pHour, PWSTR pMin)
{
	wcscpy_s(this->cday, pDay);
	wcscpy_s(this->chour, pHour);
	wcscpy_s(this->cmin, pMin);
}

SPTIME &SPTIME::operator=(SPTIME spt)
{
	wcscpy_s(this->cday, spt.cday);
	wcscpy_s(this->chour, spt.chour);
	wcscpy_s(this->cmin, spt.cmin);
	return *this;
}

UINT SPTIME::GetTotalSecond()
{
	return (_wtoi(this->cday) * 24 * 3600 + _wtoi(this->chour) * 3600 + _wtoi(this->cmin) * 60);
}

//InitiateSystemShutdown
//AbortSystemShutdown


BOOL CALLBACK AssistantTimeShutdown(HWND hWnd,UINT time)
{
	if (time<60 || time>2592000)
	{
		int result=MessageBox(hWnd, L"Time set is too long or too short\nAre you sure set?", L"Time set Warning", MB_YESNO | MB_ICONWARNING);
		if (result == IDNO)
			return FALSE;
	}
	WCHAR times[20];
	swprintf_s(times, L"%d分钟后将会关机", time/60);
	//MessageBox(hWnd, times, L"Time Test", MB_OK);
	HANDLE hToken;              // handle to process token 
	TOKEN_PRIVILEGES tkp;       // pointer to token structure 

	BOOL fResult;               // system shutdown flag 

	// Get the current process token handle so we can get shutdown 
	// privilege. 

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return FALSE;

	// Get the LUID for shutdown privilege. 

	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
		&tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get shutdown privilege for this process. 

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
		(PTOKEN_PRIVILEGES) NULL, 0);

	// Cannot test the return value of AdjustTokenPrivileges. 

	if (GetLastError() != ERROR_SUCCESS)
		return FALSE;

	// Display the shutdown dialog box and start the countdown. 

	fResult = InitiateSystemShutdown(
		NULL,    // shut down local computer 
		times,   // message for user
		time,      // time-out period, in seconds 
		FALSE,   // ask user to close apps 
		FALSE);   // reboot after shutdown 

	if (!fResult)
		return FALSE;

	// Disable shutdown privilege. 

	tkp.Privileges[0].Attributes = 0;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
		(PTOKEN_PRIVILEGES) NULL, 0);

	return TRUE;
}

BOOL CALLBACK AssistantAbortShutdown()
{
	HANDLE hToken;              // handle to process token 
	TOKEN_PRIVILEGES tkp;       // pointer to token structure 

	// Get the current process token handle  so we can get shutdown 
	// privilege. 

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return FALSE;

	// Get the LUID for shutdown privilege. 

	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
		&tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get shutdown privilege for this process. 

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
		(PTOKEN_PRIVILEGES) NULL, 0);

	if (GetLastError() != ERROR_SUCCESS)
		return FALSE;

	// Prevent the system from shutting down. 

	if (!AbortSystemShutdown(NULL))
		return FALSE;

	// Disable shutdown privilege. 

	tkp.Privileges[0].Attributes = 0;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
		(PTOKEN_PRIVILEGES) NULL, 0);
	return TRUE;
}


BOOL VECTORCALL WiFiAssistantTimeTaskmgr(PTIMEEX itime)
{

	
	return TRUE;
}

INT VECTORCALL GetSecondTotal(PTIMEEX Newtime)
{
	SYSTEMTIME stm;
	INT TotalSec=0;
	GetSystemTime(&stm);
	if (Newtime->day == NULL)
		Newtime->day = stm.wDay;
	TotalSec = (INT) (Newtime->sec - stm.wSecond) + (INT) (Newtime->min - stm.wMinute) * 60 + (INT) (Newtime->hor - stm.wHour) * 3600 +
		(INT) (Newtime->day - stm.wDay) * 86400;
	if (TotalSec < 0)
		return -1;
	else
		return TotalSec;
}