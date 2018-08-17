/*************************************************************************************************************************************
* TimeBox Super class
*
*
*
**************************************************************************************************************************************/
#include "stdafx.h"
#include "TimeBox.h"
#include <assert.h>

class TBE{
public:
	int num;
	int Id;
	int Limit;
	TBE() :num(0), Id(0), Limit(0){}
	TBE(int a, int b, int c) :num(a), Id(b), Limit(c){}
	TBE& operator+=(const TBE &tbe){
		Id += tbe.Id;
		num += tbe.num;
		Limit += tbe.Limit;
		return *this;
	}
	TBE& operator =(TBE &tbe){
		Id = tbe.Id;
		num = tbe.num;
		Limit = tbe.Limit;
		return *this;
	}
};




LRESULT(CALLBACK* OldEditWndProc2)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK TimeBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PASTE:
		break;
	case TE_SETMAXNUM:

		break;
	default:
		break;
	}
	return CallWindowProc(OldEditWndProc2,hWnd, message, wParam, lParam);
}
BOOL RegisterTimeBoxEdit()
{
	WNDCLASSEX TimeBoxEdit;
	ZeroMemory(&TimeBoxEdit, sizeof(TimeBoxEdit));
	TimeBoxEdit.cbSize = sizeof(WNDCLASSEX);
	GetClassInfoEx(0, TEXT("EDIT"), &TimeBoxEdit);
	OldEditWndProc2 = TimeBoxEdit.lpfnWndProc;
	TimeBoxEdit.lpfnWndProc = TimeBoxProc;
	//PreEditEx.lpszMenuName=
	TimeBoxEdit.lpszClassName = TEXT("TimeBoxEdit");
	RegisterClassEx(&TimeBoxEdit);
	return TRUE;
}