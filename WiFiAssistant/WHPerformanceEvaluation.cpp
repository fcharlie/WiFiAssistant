/************************************************************************************************************************
* Performance evaluation of WiFi hotspots
* Note: WiFi
*
*
**************************************************************************************************************************/
#include "stdafx.h"
#include <intrin.h>
#include <sysinfoapi.h>
#include <malloc.h>
#include "WHPerformanceEvaluation.h"

namespace WhPEInternal{
	const WHPE::AccessInt IChannel = 13;
	const WHPE::AccessInt IAccessA = 8;
	const WHPE::AccessInt IAccessB = 14;
	const WHPE::AccessInt IAccessC = 32;
}

//1GByte const
const unsigned long long CGB =0x40000000;

class ProcessScore{
	USHORT NumberOfCore;
	USHORT NumberOfSignal;
	USHORT PScore;
public:
	ProcessScore(LPSYSTEM_INFO Procs){
		this->NumberOfSignal=(USHORT)Procs->dwNumberOfProcessors;
		//__cpuid
		//__cpuidex
		//GetLogicalProcessorInformation()
		//GetLogicalProcessorInformationEx
		BOOL done = FALSE;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
		DWORD returnLength = 0;
		DWORD logicalProcessorCount = 0;
		DWORD numaNodeCount = 0;
		DWORD processorCoreCount = 0;
		DWORD processorL1CacheCount = 0;
		DWORD processorL2CacheCount = 0;
		DWORD processorL3CacheCount = 0;
		DWORD processorPackageCount = 0;
		DWORD byteOffset = 0;
		PCACHE_DESCRIPTOR Cache;
		UNREFERENCED_PARAMETER(Cache);
		GetLogicalProcessorInformation(buffer, &returnLength);
	}
	USHORT GetProcessScore(){
		return this->PScore;
	}
};

class MemoryScore{
private:
	UINT64 Size;
	USHORT uload;
	double fscore;
	USHORT Score;
public:
	MemoryScore(MEMORYSTATUSEX* me)
	{
		fscore = 0;
		float isz = 0;
		this->uload=(USHORT)me->dwMemoryLoad;
		this->Size = me->ullAvailPhys;
		if (this->Size <= CGB&&this->Size>=0)
		{
			isz = 20;
		}
		if (this->Size > CGB&&this->Size <= 2 * CGB)
		{
			isz = 40;
		}
		if (this->Size > 2 * CGB&&this->Size <= 3 * CGB)
		{
			isz = 50;
		}
		if (this->Size > 3 * CGB&&this->Size <= 4 * CGB)
		{
			isz = 60;
		}
		if (this->Size > 4 * CGB&&this->Size <= 5 * CGB)
		{
			isz = 70;
		}
		if (this->Size > 5 * CGB&&this->Size <= 8 * CGB)
		{
			isz = 80;
		}
		if (this->Size > 8 * CGB&&this->Size <= 16 * CGB)
		{
			isz = 90;
		}
		if (this->Size > 16 * CGB)
		{
			isz = 99;
		}
		this->fscore = (double)(this->uload*3 + isz*7)/10;
		this->Score = (ushort)this->fscore;
	}
	USHORT GetMemoryScore()
	{
		return Score;
	}
};

typedef struct _HardScore{
	ushort process;
	ushort memory;
	ushort netadapter;
}HardScore;

// p*0.4+m*0.3+n*0.3
// memory 30% 
ushort getHardwareEvaluation()
{
	HardScore hds = {0};
	//ushort mems;
	SYSTEM_INFO sinfo;
	GetSystemInfo(&sinfo);
	MEMORYSTATUSEX mer;
	mer.dwLength = sizeof(mer);
	GlobalMemoryStatusEx(&mer);
	MemoryScore* mes = new MemoryScore(&mer);
	hds.memory = mes->GetMemoryScore();
	delete mes;
	return (ushort)(hds.process*4+hds.memory*3+hds.netadapter*3)/10;
}

ushort getSoftwareSystemEnvaluation()
{
	return 0;
}

ushort getBandwithEnvaluation()
{
	return 0u;
}

ushort getWirelessDeviceEnvaluation()
{
	return 0u;
}

ushort getBandAndDeviceEnvaluation()
{
	return 0u;
}
ushort getHardAndSoftEnvaluation()
{
	return 0u;
}

ushort getDefaultEnvaluation()
{
	return 0u;
}

BOOL GetModuleEvaluationScore(WHPE::PWHModule whm)
{
	switch (whm->scm)
	{
	case WHPE::SysComposedModule::Hardware:
		whm->Score = getHardwareEvaluation();
		break;
	case WHPE::SysComposedModule::Bandwidth:
		whm->Score = getBandwithEnvaluation();
		break;
	case WHPE::SysComposedModule::SoftwareSystem:
		whm->Score = getSoftwareSystemEnvaluation();
		break;
	case WHPE::SysComposedModule::WirelessDevice:
		whm->Score = getWirelessDeviceEnvaluation();
		break;
	case WHPE::BandAndDevice:
		whm->Score = getBandAndDeviceEnvaluation();
		break;
	case WHPE::HardAndSoft:
		whm->Score = getHardAndSoftEnvaluation();
		break;
	default:
		whm->Score = getDefaultEnvaluation();
		break;
	}
	return TRUE;
}

BOOL GetModuleEvaluationScoreTotal()
{
	return TRUE;
}

BOOL GetWiFiHotPerformanceEvaluationShortBoard(WHPE::WHModule* whm)
{
	//WHPE::SysComposedModule Modul [4];
	WHPE::WHModule whd[WHPMODULE_NUM];
	whd[0].scm = WHPE::SysComposedModule::Hardware;
	whd[1].scm = WHPE::SysComposedModule::Bandwidth;
	whd[2].scm = WHPE::SysComposedModule::SoftwareSystem;
	whd[3].scm = WHPE::SysComposedModule::WirelessDevice;
	for (int i = 0; i < 4; i++){
		if (!GetModuleEvaluationScore(&whd[i]))
			return FALSE;
	}
	whm->Score = whd[0].Score;
	whm->scm = whd[0].scm;
	for (int j = 1; j < 4; j++)
	{
		if (whd[j].Score <= whm->Score)
		{
			whm->Score = whd[j].Score;
			whm->scm = whd[j].scm;
		}
	}
	return TRUE;
}

BOOL EvaluationOptimalAccessNum(WHPE::AccessInt* acn)
{
	return TRUE;
}