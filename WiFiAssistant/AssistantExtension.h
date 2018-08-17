#ifndef ASSISTANT_EXTENSION_H
#define ASSISTANT_EXTENSION_H
#include <time.h>

#ifndef VECTORCALL
#if defined(_MSC_VER)&&_MSC_VER >= 1800
#define VECTORCALL __vectorcall
#else
#define VECTORCALL
#endif
#endif

#ifndef __cplusplus
//C++ nullptr is Built-in type
#ifndef nullptr
#define nullptr ((void *)0)
#endif
#endif

typedef unsigned short uint16_t;

typedef struct _TIMEEX{
	uint16_t sec;
	uint16_t min;
	uint16_t hor;
	uint16_t day;
	uint16_t mon;
	uint16_t year;
}TIMEEX,*PTIMEEX;

class SPTIME{
private:
	WCHAR cday[4];
	WCHAR chour[3];
	WCHAR cmin[3];
public:
	UINT GetTotalSecond();
	SPTIME(PWSTR pDay, PWSTR pHour, PWSTR pMin);
	SPTIME &operator= (SPTIME spt);
};

BOOL CALLBACK AssistantTimeShutdown(HWND hWnd,UINT time);

BOOL CALLBACK AssistantAbortShutdown();


#endif