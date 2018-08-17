/*************************************************************************************************************************************
* LimitEdit Super class 
*
*
*
**************************************************************************************************************************************/
#ifndef LIMITEDIT_H
#define LIMITEDIT_H

const TCHAR LimitEdit [] = TEXT("LimitEdit");

HWND CreateLimitEditW(LPWSTR text, RECT rect, HWND hWndParat, const LONG_PTR Id, HINSTANCE hInst, LPVOID lpParam, bool Ispwd);

class LimitEdit{
public:
	LimitEdit();
	~LimitEdit();
};

#endif