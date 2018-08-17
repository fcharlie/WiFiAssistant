#include "stdafx.h"
//#pragma warning(disable:4267)
//#include <string>
//#include <Windows.h>
//#include <shellapi.h>
#include <WinSock2.h>
#include <IPTypes.h>
#include <WMIUtils.h>
#include <objbase.h>
#include <NetCon.h>
#include <locale.h>
#include <winnetwk.h>
#include <netlistmgr.h>
#include <ShObjIdl.h>
#include <IPHlpApi.h>
#include <wlanapi.h>
#include <Sensapi.h>
#include <ctime>
#include "AssistantProcess.h"
#include "WirelessErrorType.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib, "Wlanapi.lib")
#pragma comment(lib,"Sensapi.lib")
#pragma comment (lib,"Mpr.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib,"Rpcrt4.lib")

bool GetRandStringUserOrPwd(PWSTR wstr, UINT Size)
{
	const std::wstring templetstr= L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz@#$%^&*()-=_+,./;\"'<>?~|";
	UINT i, lstr;
	UINT k;
	std::wstring produce;
	lstr = (UINT)templetstr.size();
	srand((unsigned int) GetTickCount());
	for (i = 1; i < Size; i++)
	{
		k = rand() % lstr;
		produce += templetstr.substr(k,1);
		
	}
	wcscpy_s(wstr, Size, produce.c_str());
	//MessageBox(NULL, produce.c_str(), L"Test", MB_OK);
	return true;
}

HRESULT GetWlanErrorHResult(DWORD rror)
{
	HRESULT hr;
	switch (rror)
	{
	case ERROR_INVALID_PARAMETER:
		hr= WLANHANDLEOPEN_ERROR_INVALID_PARAMETER;
		break;
	case ERROR_NOT_ENOUGH_MEMORY:
		hr = WLANHANDLEOPEN_ALLOCATE_MEMORY;
		break;
	case ERROR_REMOTE_SESSION_LIMIT_EXCEEDED:
		hr = WHO_ERROR_REMOTE_SESSION_LIMIT_EXCEEDED;
		break;
	case 1062:
		hr = SERVICE_NOT_START;
		break;
	default:
		hr = 99;
		break;
	}
	return hr;
}
//Open WlanSvc
LRESULT OpenServiceInvokeUtilities()
{
	auto hr=S_OK;
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCM)
		return SC_HANDLE_OPEN_ERROR;
	SC_HANDLE hSrc=OpenService(hSCM,TEXT("WlanSvc"), SERVICE_ALL_ACCESS);
	SERVICE_STATUS svrST;
	if (!hSrc)
	{
		CloseServiceHandle(hSCM);
		return WLANSVC_START_ERROR;
	}
	ZeroMemory(&svrST, sizeof(svrST));
	if (QueryServiceStatus(hSrc, &svrST) == FALSE)
	{
		CloseServiceHandle(hSCM);
		CloseServiceHandle(hSrc);
		return WLANSVC_START_ERROR;
	}
	if (svrST.dwCurrentState != SERVICE_RUNNING)
	{
		if (StartService(hSrc, NULL, NULL) == FALSE){
			CloseServiceHandle(hSCM);
			CloseServiceHandle(hSrc);
			hr = WLANSVC_START_ERROR;
			//QueryServiceLockStatus()
		}
	}
	CloseServiceHandle(hSCM);
	CloseServiceHandle(hSrc);
	
	return hr;
}
/*AreThereFindWirelessNetworkAdapter Function
/Check that this computer has a Wireless Network Adapter
/IF Find WirelessNetworkAdapter return TRUE else return FALSE*/
LRESULT AreThereFindWirelessNetworkAdapter()
{
	LRESULT hr = S_OK;
	/*Check Service to Start*/
	hr = InternetConnectionSharingManager(false);
	if (hr != S_OK)
		return hr;
	if (OpenServiceInvokeUtilities() != S_OK)
		return WLANSVC_START_ERROR;
	PWLAN_INTERFACE_INFO_LIST wlist=nullptr;
	HANDLE phClientHandle;
	DWORD dwMaxClient = 2;  
	DWORD dwCurVersion = 0;
	DWORD dwResult = 0;
	//BOOL iRet = 0;
	WCHAR GuidString[40] = { 0 };
	PWLAN_INTERFACE_INFO pIfInfo = nullptr;
	dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &phClientHandle);
	if (dwResult != ERROR_SUCCESS)
		return S_FALSE;
	if (WLAN_API_VERSION_MAJOR(dwCurVersion)<WLAN_API_VERSION_2_0)
	{
		WlanCloseHandle(phClientHandle, NULL);
		return WLAN_API_VERLOW_NOT_SUPPORT;
	}
	//PWLAN_INTERFACE_INFO_LIST wifilist;
	WlanEnumInterfaces(phClientHandle, NULL, &wlist);
	if (wlist->dwNumberOfItems <= 0)
	{
		//MessageBox(nullptr, L"Hello", L"Null", MB_OK);
		return 3;
	}
	return hr;
}

BOOL FindCommunicationNetworkAdapter()
{
	//InternetGetConnectedState
	//IsNetworkAlive
	DWORD flags;
	BOOL iRet = IsNetworkAlive(&flags);
	if (GetLastError()!=ERROR_SUCCESS||!iRet==TRUE)
		return FALSE;
	return TRUE;
}

//ICS Service Manager
LRESULT InternetConnectionSharingManager(bool IsClose)
{
	LRESULT result = 0L;
	auto hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCM)
		return SC_HANDLE_OPEN_ERROR;
	SC_HANDLE hSrc = OpenService(hSCM, TEXT("SharedAccess"), SERVICE_ALL_ACCESS);
	SERVICE_STATUS svrST;
	if (!hSrc)
	{
		CloseServiceHandle(hSCM);
		return WLANSVC_START_ERROR;
	}
	ZeroMemory(&svrST, sizeof(svrST));
	if (QueryServiceStatus(hSrc, &svrST) == FALSE)
	{
		CloseServiceHandle(hSCM);
		CloseServiceHandle(hSrc);
		return WLANSVC_START_ERROR;
	}
	if (svrST.dwCurrentState != SERVICE_RUNNING&&IsClose==false)
	{
		if (StartService(hSrc, NULL, NULL) == FALSE){
			CloseServiceHandle(hSCM);
			CloseServiceHandle(hSrc);
			result = WLANSVC_START_ERROR;
			//QueryServiceLockStatus()
		}
	}
	if (svrST.dwCurrentState == SERVICE_RUNNING&&IsClose == true)
	{
		if (ControlService(hSrc, SERVICE_CONTROL_STOP, NULL) == FALSE){
			CloseServiceHandle(hSCM);
			CloseServiceHandle(hSrc);
			return WLANSVC_STOP_ERROR;
		}
	}
	CloseServiceHandle(hSCM);
	CloseServiceHandle(hSrc);
	return result;
}

LRESULT NetSharingManagerFindAndSet()
{
	CoInitialize(NULL);
	// init security to enum RAS connections
	CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	INetSharingManager * pNSM = NULL;
	HRESULT hr = ::CoCreateInstance(__uuidof(NetSharingManager),
		NULL,
		CLSCTX_ALL,
		__uuidof(INetSharingManager),
		(void**) &pNSM);
	INetConnectionManager *pNCMgr=NULL;
	/*PPPOE*/
	const WCHAR Adapterdevice[] = L"Microsoft Virtual WiFi Miniport Adapter";
	INetConnection * pNC = NULL; // fill this out for part 2 below
	INetSharingEveryConnectionCollection * pNSECC = NULL;
	hr = pNSM->get_EnumEveryConnection(&pNSECC);
	if (!pNSECC)
	{
		hr = INETSHARD_CONNECTION_ERROR;
		return hr;
	}
	else {
		// enumerate connections
		IEnumVARIANT * pEV = NULL;
		IUnknown * pUnk = NULL;
		hr = pNSECC->get__NewEnum(&pUnk);
		if (pUnk) {
			hr = pUnk->QueryInterface(__uuidof(IEnumVARIANT),
				(void**) &pEV);
			pUnk->Release();
		}
		if (pEV) {
			VARIANT v;
			VariantInit(&v);
			BOOL bFoundIt = FALSE;
			INetSharingConfiguration * pNSC = NULL;
			INetSharingConfiguration * pVWifiNSC = NULL;
			NETCON_PROPERTIES* pVWifi = NULL;
			while (S_OK == pEV->Next(1, &v, NULL)) {
				if (V_VT(&v) == VT_UNKNOWN) {
					V_UNKNOWN(&v)->QueryInterface(__uuidof(INetConnection),
						(void**) &pNC);
					if (pNC) {
						NETCON_PROPERTIES* pNP = NULL;
						pNC->GetProperties(&pNP);
						if (pNP->Status == NCS_CONNECTED){
							hr = pNSM->get_INetSharingConfigurationForINetConnection(pNC, &pNSC);
							hr = pNSC->EnableSharing(ICSSHARINGTYPE_PUBLIC);
							pNSC->Release();
						}
						if (_wcsicmp(pNP->pszwDeviceName, Adapterdevice) == 0){
							pVWifi = pNP;
							hr = pNSM->get_INetSharingConfigurationForINetConnection(pNC, &pVWifiNSC);
							pVWifiNSC->EnableSharing(ICSSHARINGTYPE_PRIVATE);
						}
					}
				}
			}
			INetSharingPortMapping * pNSPM = NULL;
			BSTR pszwName = pVWifi->pszwName;
			pVWifiNSC->Release();
			if (pNSPM) {
				hr = pNSPM->Enable();
				INetSharingPortMappingProps* pMapping = NULL;
				pNSPM->get_Properties(&pMapping);
				BSTR result = NULL;
				long port = 0;
				pMapping->get_Name(&result);
				pMapping->get_ExternalPort(&port);
				pNSPM->Release();
			}
		}
	}
	return S_OK;
}

/*If Run WiFiAssistant First Your can Invoke WirelessBearerNetworkInvokeOneKeyStart() Auto Setting WiFi and run;*/
LRESULT WirelessBearerNetworkInvokeOneKeyStart(WiFiNetInfoW* wifiinfo)
{
	LRESULT hr=S_OK;
	WCHAR ssid[11] = { L"\0" };
	WCHAR key[11] = { L"\0" };
	bool ret1, ret2;
	ret1=GetRandStringUserOrPwd(ssid, 11);
	if (!ret1)
		return 2;
	wcscpy_s(wifiinfo->SSID, ssid);
	Sleep(500);
	ret2=GetRandStringUserOrPwd(key, 11);
	if (!ret2)
		return 2;
	wcscpy_s(wifiinfo->KEY, key);
	//MessageBox(NULL, wifiinfo->KEY, wifiinfo->SSID, MB_OK);
	hr=WirelessNetworkInvokeAPIStart(wifiinfo);
	return hr;
}
LRESULT WirelessHostedNetworkServiceToStart()
{
	//NotifyServiceStatusChange()
	//SharedAccess
	return S_OK;
}

//Start VirtualWiFi
//WlanHostedNetworkRefreshSecuritySettings
//WlanRegisterNotification
LRESULT WirelessNetworkInvokeAPIStart(WiFiNetInfoW* wifiinfo)
{
	LRESULT hr = S_OK;
	/*Check Service to Start*/
	if (!FindCommunicationNetworkAdapter())
		return NO_INTENET_ONLINE;
	hr = InternetConnectionSharingManager(false);
	if (hr != S_OK)
		return hr;
	if (OpenServiceInvokeUtilities() != S_OK)
		return WLANSVC_START_ERROR;

	CHAR ckey[32] = { 0 };
	//DWORD dError = 0;
	BOOL bIsAllow = TRUE;
	DWORD dwError = 0;
	DWORD dwServiceVersion = 0;
	HANDLE hClient;
	dwError=WlanOpenHandle(
		WLAN_API_VERSION,
		NULL,               // reserved
		&dwServiceVersion,
		&hClient
		);
	if (ERROR_SUCCESS !=dwError )
	{
		WCHAR er[50] = {0};
		swprintf_s(er, L"%d", dwError);
		MessageBox(NULL, er, L"Error", MB_OK);
		hr = GetWlanErrorHResult(dwError);
		return hr;
	}

	// check service version
	if (WLAN_API_VERSION_MAJOR(dwServiceVersion) < WLAN_API_VERSION_MAJOR(WLAN_API_VERSION_2_0))
	{
		WlanCloseHandle(hClient, NULL);
	}

	

	WLAN_HOSTED_NETWORK_REASON dwFailedReason;
	if (WlanHostedNetworkInitSettings(hClient, &dwFailedReason, NULL) != ERROR_SUCCESS)
		return WLAN_HOSTED_CANNOT_INIT;
	DWORD dwReturnValue = WlanHostedNetworkSetProperty(hClient,
		wlan_hosted_network_opcode_enable,
		sizeof(BOOL),
		&bIsAllow,
		&dwFailedReason,
		NULL);

	WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS whncs;
	memset(&whncs, 0, sizeof(WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS));
	

	
	whncs.dwMaxNumberOfPeers = 8;
	//whncs.hostedNetworkSSID.ucSSID
	WideCharToMultiByte(CP_ACP,
		0,
		wifiinfo->SSID,
		(int)wcslen(wifiinfo->SSID),
		(LPSTR)whncs.hostedNetworkSSID.ucSSID,
		32,
		NULL,
		NULL);
	whncs.hostedNetworkSSID.uSSIDLength = (ULONG) strlen((const char*) whncs.hostedNetworkSSID.ucSSID);
	dwReturnValue = WlanHostedNetworkSetProperty(hClient,
		wlan_hosted_network_opcode_connection_settings,
		sizeof(WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS),
		&whncs,
		&dwFailedReason,
		NULL);

	WideCharToMultiByte(CP_ACP,
		0,
		wifiinfo->KEY,
		(int)wcslen(wifiinfo->KEY),
		ckey,
		32,
		NULL,
		NULL);
	dwReturnValue = WlanHostedNetworkSetSecondaryKey(hClient,
		(DWORD)(strlen((const char*) ckey) + 1),
		(PUCHAR)ckey,
		TRUE,
		FALSE,
		&dwFailedReason,
		NULL);
	if (ERROR_SUCCESS != dwReturnValue)
	{
		return SET_SECONDKEY_ERROR;
	}
	//strcpy_s((CHAR*)whncs.hostedNetworkSSID.ucSSID,32,cssid);
	//WlanHostedNetworkQueryStatus(hClient, , NULL);

	dwReturnValue = WlanHostedNetworkForceStart(hClient, &dwFailedReason, NULL);
	if (ERROR_SUCCESS != dwReturnValue)
	{
		if (wlan_hosted_network_reason_interface_unavailable == dwFailedReason)
		{
			return HOSTEDNETWORK_DISABLE_BY_GROUPPOLICY;
		}
		return WLANHOSTED_FORCE_START_ERROR;
	}
	hr = NetSharingManagerFindAndSet();
	WlanCloseHandle(hClient,NULL);
	return hr;
}

//Stop Virtual WiFi
LRESULT WirelessNetworkInvokeAPIStop()
{
	LRESULT hr = S_OK;
	DWORD dwError = 0;
	DWORD dwServiceVersion = 0;
	HANDLE hClient = NULL;
	if (ERROR_SUCCESS != (dwError = WlanOpenHandle(
		WLAN_API_VERSION,
		NULL,               // reserved
		&dwServiceVersion,
		&hClient
		)))
	{
		return WLAN_HOSTED_CANNOT_INIT;
	}
	WLAN_HOSTED_NETWORK_REASON dwFailedReason;
	DWORD dwResult=WlanHostedNetworkForceStop(hClient,&dwFailedReason, NULL);
	if (dwResult != ERROR_SUCCESS)
	{
		return CLOSE_WIFIHOSTED_ERROR;
	}
	return hr;
}

LRESULT WirelessHostedNetworkToQueryStatus()
{
	return S_OK;
}

//WlanHostedNetworkQuerySecondaryKey