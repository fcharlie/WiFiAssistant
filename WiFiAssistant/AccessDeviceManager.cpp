/************************************************************************************************************************
* WiFi Assistant AccessDeviceManager
* Note: WiFi
*
*
**************************************************************************************************************************/

//API+
//WlanHostedNetworkForceStart
//WlanHostedNetworkInitSettings
//WlanHostedNetworkQueryProperty
//WlanHostedNetworkQuerySecondaryKey
//WlanHostedNetworkQueryStatus
//WlanHostedNetworkRefreshSecuritySettings
//WlanHostedNetworkSetProperty
//WlanHostedNetworkSetSecondaryKey
//WlanHostedNetworkStartUsing
//WlanHostedNetworkStopUsing
//WlanRegisterVirtualStationNotification

#include "stdafx.h"
#include "AccessDeviceManager.h"
#include "AssistantProcess.h"
#include <vector>

typedef unsigned short uint16_t;

typedef struct _AccessResource{
	unsigned short Id;
	bool State;
	//bool IsLock;
}AccessResource;

class AccessResourceMgr{
public:
	AccessResource* iDevice;
	unsigned short iSize;
	AccessResourceMgr(unsigned short Size)
	{
		this->iSize=Size;
		iDevice = new AccessResource[Size];
	}
	~AccessResourceMgr()
	{
		//for (unsigned short i = 0; i < iSize; i++)
		//	delete this->iDevice [i];
		delete[] this->iDevice;
	}
	bool InitiazeARMgr()
	{
		for (uint16_t j = 0; j < iSize; j++)
		{
			iDevice[j].State = false;
			//iDevice[j].IsLock = false;
			iDevice[j].Id = j;
		}
		return true;
	}
};

class ARMgrSTL{
public:
	uint16_t Size;
	ARMgrSTL(uint16_t size)
	{
		this->Size = size;
		std::vector<AccessResource> vars(size);
	}
};