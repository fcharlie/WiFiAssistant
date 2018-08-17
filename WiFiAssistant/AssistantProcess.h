/************************************************************************************************************************
* WiFi Assistant Process Header
* Note: WiFi
*
*
**************************************************************************************************************************/
#ifndef ASSISTANTPROCESS_H
#define ASSISTANTPROCESS_H
#include <winnt.h>
#include <string>
//
#ifndef __cplusplus
//C++ nullptr is Built-in type
#ifndef nullptr
#define nullptr ((void *)0)
#endif
#endif

#define MAX_UNC_PATH 32768

#if defined(_MSC_VER)&&_MSC_VER >= 1800
#define VECTORCALL __vectorcall
#else
#define VECTORCALL
#endif

#ifdef __cplusplus
namespace WiFiAssistant{
#else
typedef struct _WiFiAssistant
{
#endif
	enum WiFiMode{
		WiFiAllow,
		WiFiDisAllow
	};
#ifdef __cplusplus
}
#else
}WiFiAssistant;
#endif

#define SETTING_NODE L"Settings"
#define WIFISSID_NODE L"WiFiSSID"
#define WIFIKEY_NODE  L"WiFiKey"
#define DISPLAYKEY_NODE L"Displaykey"
#define VERSIONMARK_NODE L"Versionmark"
#define APPNAME_NODE L"Appname"

typedef struct _WLAN_INFOW{
	WCHAR SSID [32];
	WCHAR KEY [32];
	bool IsShowPwd;
}WiFiNetInfoW;

//Check System and Check Wireless  Network Adapter
LRESULT AreThereFindWirelessNetworkAdapter();

LRESULT InternetConnectionSharingManager(bool IsClose);
LRESULT WirelessBearerNetworkInvokeOneKeyStart(WiFiNetInfoW* wifiinfo);

LRESULT WirelessNetworkInvokeAPIStart(WiFiNetInfoW* wifiinfo);
LRESULT WirelessNetworkInvokeAPIStop();

BOOL VECTORCALL WiFiAssistantCredentialRead(WiFiNetInfoW *wifiinfo);
BOOL VECTORCALL WiFiAssistantCredentialWrite(WiFiNetInfoW &wifiinfo);

BOOL WINAPI UACSelfElevation(HWND hWnd, int nCmdShow);

#ifdef __cplusplus
class WiFiAssistantConfig{
private:
	WiFiNetInfoW iwifiinfo;
	HRESULT XMLReader();
public:
	bool InitXML(std::wstring &wifixml);
	bool InitXML();
	bool GetWiFiConfig(WiFiNetInfoW* winfo);
	HRESULT XMLWrite(WiFiNetInfoW* winfo);
	WiFiAssistantConfig();
	~WiFiAssistantConfig();
	std::wstring wxml;
	std::wstring waInit;
};
#endif

#endif