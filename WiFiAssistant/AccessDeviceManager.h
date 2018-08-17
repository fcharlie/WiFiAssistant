/************************************************************************************************************************
* WiFi Assistant AccessDeviceManager
* Note: WiFi
*
*
**************************************************************************************************************************/
#ifndef ACCESSDEVICEMANAGER_H
#define ACCESSDEVICEMANAGER_H
//#include <objbase.h>

#ifndef NO_ACHIEVE
#define NO_ACHIEVE 1
#endif

#ifndef __cplusplus
//C++ nullptr is Built-in type
#ifndef nullptr
#define nullptr ((void *)0)
#endif
#endif

class AccessDevice{
public:
	unsigned short Id;
	//Access Device IId;
	GUID IId;
	AccessDevice();
	~AccessDevice();
};

typedef struct _DeviceModule{
	unsigned short Id;
	//GUID IId;
	GUID IId;
}DeviceModule;

typedef struct _DeviceList{
	DeviceModule dm;
	struct _DeviceList *Pre;
	struct _DeviceList *Next;
}DeviceList;

class AccessDeviceMgr{
public:
	BOOL ListDevice();
	/*DeleDevice ade.dm->nullptr Close Signed*/
	BOOL DeleteDevice(AccessDevice* ade);
	BOOL UpdateDeviceInfo();
};

#endif