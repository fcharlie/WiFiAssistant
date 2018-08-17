////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WirelessErrorType
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ERROR_INTERNAL
#include "stdafx.h"
#include"WirelessErrorType.h"



void WINAPI GetErrorMessageString(LRESULT hr,PWSTR pstr)
{
	switch (hr)
	{
	case ERRORTYPE_SUCCESS:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"No Error!");
		break;
	case NO_INTENET_ONLINE:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"No network connection!");
		break;
	case NO_WIRELESS_DEVICE:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"No wireless network device driver!");
		break;
	case SYSTEM_NOT_SUPPORT:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"Operating systems are not supported!");
		break;
	case WLAN_API_VERLOW_NOT_SUPPORT:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"Wlan API version is low not support!");
		break;
	case WLAN_HOSTED_CANNOT_INIT:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"Wlan Hosted Network can not Initialize!");
		break;
	case INETSHARD_CONNECTION_ERROR:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"Network sharing can not be achieved!");
		break;
	case CLOSE_WIFIHOSTED_ERROR:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"WirelessHostedNetwork can not close!");
		break;
	case SET_SECONDKEY_ERROR:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"Failed to set user key!");
		break;
	case WLANHOSTED_FORCE_START_ERROR:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"Wlan can not be forced open!");
		break;
	case HOSTEDNETWORK_DISABLE_BY_GROUPPOLICY:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"Hosted Network is disabled by group policy on a domain");
		break;
	case WLANHANDLEOPEN_ALLOCATE_MEMORY:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"Failed to allocate memory to create the client context!");
		break;
	case WLANHANDLEOPEN_ERROR_INVALID_PARAMETER:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"pdwNegotiatedVersion is NULL, phClientHandle is NULL, or pReserved is not NULL!");
		break;
	case WHO_ERROR_REMOTE_SESSION_LIMIT_EXCEEDED:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"Too many handles have been issued by the server");
		break;
	case SERVICE_NOT_START:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"The service did not start.");
		break;
	case COM_COMPONENT_FAILED_TO_INITIALIZE:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"COM component failed to initialize.");
		break;
	case SC_HANDLE_OPEN_ERROR:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"Service Manager fails to open.");
		break;
	case WLANSVC_START_ERROR:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"Wireless Auto Configuration service failed to start.");
		break;
	case WLANSVC_STOP_ERROR:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"Stop Service Error!");
		break;
	default:
		wcscpy_s(pstr, MAX_ERROR_STRING, L"Unknown Error!");
		break;
	}
}