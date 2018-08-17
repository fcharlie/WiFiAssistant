/************************************************************************************************************************
* Performance evaluation of WiFi hotspots
* Note: WiFi
*
*
**************************************************************************************************************************/
#ifndef WHPERFORMANCEEVALUATION_H
#define WHPERFORMANCEEVALUATION_H

#ifndef __cplusplus
//C++ nullptr is Built-in type
#ifndef nullptr
#define nullptr ((void *)0)
#endif
#endif

#define WHPMODULE_NUM    4
typedef unsigned short ushort;
namespace WHPE{
	typedef unsigned short Idm,IScore,AccessInt;

	enum SysComposedModule{
		Hardware,
		Bandwidth,
		WirelessDevice,
		SoftwareSystem,
		HardAndSoft,
		BandAndDevice,
	};

	typedef struct _WHModule{
		SysComposedModule scm;
		IScore Score;
	}WHModule,*PWHModule;
}

#endif