#include "stdafx.h"

#include "AssistantProcess.h"

#include <ole2.h>
#include <xmllite.h>
#include <stdio.h>
#include <shlwapi.h>
#include <ShlObj.h>
#include "vconfig.h"
#pragma comment(lib,"xmllite.lib")
#pragma comment(lib,"shlwapi.lib")



#define CHKHR(stmt)             do { hr = (stmt); if (FAILED(hr)) goto CleanUp; } while(0)
#define HR(stmt)                do { hr = (stmt); goto CleanUp; } while(0)
#define SAFE_RELEASE(I)         do { if (I){ I->Release(); } I = NULL; } while(0)

//#pragma comment(lib,"Shell32.lib")
#pragma warning(disable : 4127)  // conditional expression is constant

// Xml file sava as ANSI
const CHAR XmlConfigTemplate[] = 
"<?xml version =\"1.0\" encoding=\"UTF-8\"?>\r\n\
 <Settings>\r\n\
 <Appname>WiFiAssistant</Appname>\r\n\
 <Versionmark>1.0.0.0</Versionmark>\r\n\
 <WiFiSSID>VirtualWiFiAdapter</WiFiSSID>\r\n\
<WiFiKey>XHQ8NC3MCJRQXB6</WiFiKey>\r\n\
<Displaykey>1</Displaykey>\r\n\
</Settings>\r\n";

const CHAR InitializeModule [] =
";WiFiAssistant Initialize Module\r\n\
[Init]\r\n\
NumberOfRuns=0\r\n";

#ifndef __cplusplus
extern "C++"{
#endif

//#ifdef __cplusplus
//#undef __cplusplus
//#define __cplusplus 201103L
//#endif

WiFiAssistantConfig::WiFiAssistantConfig()
{

}

WiFiAssistantConfig::~WiFiAssistantConfig()
{

}

bool WiFiAssistantConfig::InitXML()
{
	std::wstring txml;
	return InitXML(txml);
}

bool WiFiAssistantConfig::InitXML(std::wstring &wifixml)
{
	WCHAR DocFolderShort[MAX_PATH] ;
	WCHAR DocFolderUNC[MAX_UNC_PATH] = {0};
	memset(this->iwifiinfo.KEY, 0, 64);
	memset(this->iwifiinfo.SSID, 0, 64);
	//WCHAR AssistantRoot [MAX_PATH] = { 0 };
	memset(DocFolderShort, 0, MAX_PATH);
	LPITEMIDLIST pidl = nullptr;
	SHGetSpecialFolderLocation(nullptr, CSIDL_PERSONAL, &pidl);
	if (pidl   &&   SHGetPathFromIDList(pidl, DocFolderUNC))
		GetShortPathName(DocFolderUNC, DocFolderShort, _MAX_PATH);

	std::wstring wifiast = DocFolderShort;
	std::wstring waini;
	wifiast=wifiast+L"\\WiFiAssistant";
	waini = wifiast + L"\\WAInitializeModule.ini";
	if (_waccess_s(wifiast.c_str(), 0) != 0)
		CreateDirectory(wifiast.c_str(), nullptr);
	else
		wifiast = wifiast + L"\\WiFiAssistant.xml";
	DWORD nBytes;
	
	if (_waccess_s(wifiast.c_str(), 0) != 0)
	{
		HANDLE hWiFiXml = CreateFile(wifiast.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hWiFiXml == INVALID_HANDLE_VALUE)
			return false;
		else
		{
			//DWORD a = 0xFEFF;
			//WriteFile(hWiFiXml,&a,sizeof(a),&nBytes, nullptr);
			WriteFile(hWiFiXml, XmlConfigTemplate, sizeof(XmlConfigTemplate)-1, &nBytes, nullptr);
		}
		CloseHandle(hWiFiXml);
	}
	///InitializeModule
	if (_waccess_s(waini.c_str(), 0) != 0)
	{
		HANDLE hIniM = CreateFile(waini.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hIniM == INVALID_HANDLE_VALUE)
			return false;
		else
		{
			//DWORD a = 0xFEFF;
			//WriteFile(hWiFiXml,&a,sizeof(a),&nBytes, nullptr);
			WriteFile(hIniM, InitializeModule, sizeof(InitializeModule) -1, &nBytes, nullptr);
		}
		CloseHandle(hIniM);
	}
	this->waInit = waini;
	wifixml = wifiast;
	this->wxml = wifiast;
	//memset(DocFolderShort, 0, MAX_PATH);
	return true;
}

HRESULT WriteAttributes(IXmlReader* pReader)
{
	const WCHAR* pwszPrefix;
	const WCHAR* pwszLocalName;
	const WCHAR* pwszValue;
	HRESULT hr = pReader->MoveToFirstAttribute();

	if (S_FALSE == hr)
		return hr;
	if (S_OK != hr)
	{
		//wprintf(L"Error moving to first attribute, error is %08.8lx", hr);
		return hr;
	}
	else
	{
		while (TRUE)
		{
			if (!pReader->IsDefault())
			{
				UINT cwchPrefix;
				if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
				{
					//wprintf(L"Error getting prefix, error is %08.8lx", hr);
					return hr;
				}
				if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
				{
					//wprintf(L"Error getting local name, error is %08.8lx", hr);
					return hr;
				}
				if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
				{
					//wprintf(L"Error getting value, error is %08.8lx", hr);
					return hr;
				}
				if (cwchPrefix > 0)
					//wprintf(L"Attr: %s:%s=\"%s\" \n", pwszPrefix, pwszLocalName, pwszValue);
					;
				else
					;
					//wprintf(L"Attr: %s=\"%s\" \n", pwszLocalName, pwszValue);
			}

			if (S_OK != pReader->MoveToNextAttribute())
				break;
		}
	}
	return hr;
}

HRESULT WiFiAssistantConfig::XMLReader()
{
	//Create IXmlReader IStream

	IStream *pXmlStream=nullptr;
	IXmlReader *pReader=nullptr;
	XmlNodeType xmlnode;
	std::wstring xmllist=L"XMLFind:\\\\<";
	const WCHAR* pwszPrefix;
	const WCHAR* pwszLocalName;
	const WCHAR* pwszValue;
	UINT cwchPrefix;
	std::wstring wsd, wsk, wIsdy;
	HRESULT hr = S_OK;
	if (FAILED(hr = SHCreateStreamOnFile(this->wxml.c_str(), STGM_READ, &pXmlStream)))
		HR(hr);
	if (FAILED(hr = CreateXmlReader(__uuidof(IXmlReader), (void**) &pReader, NULL)))
		HR(hr);
	if (FAILED(hr = pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit)))
		HR(hr);
	if (FAILED(hr = pReader->SetInput(pXmlStream)))
		HR(hr);
	while (S_OK == pReader->Read(&xmlnode))
	{
		switch (xmlnode)
		{
		case XmlNodeType_Element:
			pReader->GetPrefix(&pwszPrefix, &cwchPrefix);
			pReader->GetLocalName(&pwszLocalName, NULL);
			xmllist = xmllist + pwszLocalName + L"&>";
			//MessageBox(nullptr, pwszLocalName, L"xx", MB_OK);
			break;
		case XmlNodeType_EndElement:
			break;
		case XmlNodeType_Text:
			pReader->GetValue(&pwszValue, NULL);
			xmllist = xmllist + pwszValue + L"<";
			//MessageBox(nullptr, pwszValue, L"xx", MB_OK);
			break;
		case XmlNodeType_Whitespace:
			//pReader->GetValue(&pwszValue, NULL);
			//MessageBox(nullptr, pwszValue, L"xx", MB_OK);
			break;
		default:
			break;
		}
	}
	//pReader->MoveToAttributeByName(L"Displaykey", L"");
	//pReader->GetValue(&pwszValue, NULL);
	//MessageBox(nullptr, xmllist.c_str(), L"xx", MB_OK);
	std::wstring::size_type a,a2,a3, b,b2,b3, c,c2,c3;
	//WCHAR xx [50] = { 0 };
	a=xmllist.find(L"WiFiSSID&>");
	a3 = xmllist.find(L">", a);
	a2 = xmllist.find_first_of(L"<", a);
	wsd = xmllist.substr(a3+1, a2-a3-1);
	b=xmllist.find(L"WiFiKey&>");
	b3 = xmllist.find(L">", b);
	b2 = xmllist.find_first_of(L"<", b);
	wsk = xmllist.substr(b3 + 1, b2 - b3 - 1);
	c=xmllist.find(L"Displaykey&>");
	c3 = xmllist.find(L">", c);
	c2 = xmllist.find_first_of(L"<", c);
	wIsdy = xmllist.substr(c3 + 1, c2 - c3 - 1);
	if (a != std::wstring::npos&&b != std::wstring::npos&&c != std::wstring::npos)
	{
		wcscpy_s(this->iwifiinfo.SSID, wsd.c_str());
		wcscpy_s(this->iwifiinfo.KEY, wsk.c_str());
		int IspwdInt = _wtoi(wIsdy.c_str());
		if (IspwdInt)
			this->iwifiinfo.IsShowPwd = true;
		else
			this->iwifiinfo.IsShowPwd = false;
	}
	else
	{
		hr = 9;
	}
CleanUp:
	SAFE_RELEASE(pReader);
	SAFE_RELEASE(pXmlStream);
	return hr;
}
HRESULT WiFiAssistantConfig::XMLWrite(WiFiNetInfoW *wfw)
{
	HRESULT hr = S_OK;
	WCHAR isc[2]=L"\0";
	if (wfw->IsShowPwd == true)
	{
		isc[0] = L'1';
		isc[1] = '\0';
	}
	else
	{
		isc[0] = '0';
		isc[1] = '\0';
	}
	IXmlWriter *pWriter=nullptr;
	IStream *pOutFileStream = nullptr;
	if (FAILED(hr = SHCreateStreamOnFile(this->wxml.c_str(), STGM_CREATE | STGM_WRITE, &pOutFileStream)))
		HR(hr);
	if (FAILED(hr = CreateXmlWriter(__uuidof(IXmlWriter), (void**) &pWriter, NULL)))
		HR(hr);
	if (FAILED(hr = pWriter->SetOutput(pOutFileStream)))
		HR(hr);
	if (FAILED(hr = pWriter->SetProperty(XmlWriterProperty_Indent, TRUE)))
		HR(hr);
	if (FAILED(hr = pWriter->WriteStartDocument(XmlStandalone_Omit)))
		HR(hr);
	if (FAILED(hr = pWriter->WriteStartElement(NULL, L"Settings", NULL)))
		HR(hr);
	if (FAILED(hr = pWriter->WriteWhitespace(L"\n")))
		HR(hr);
	if (FAILED(hr = pWriter->WriteStartElement(NULL, L"Appname", NULL)))
		HR(hr);
	if (FAILED(hr = pWriter->WriteString(L"WiFiAssistant")))
		HR(hr);
	if (FAILED(hr = pWriter->WriteEndElement()))
		HR(hr);
	if (FAILED(hr = pWriter->WriteWhitespace(L"\n")))
		HR(hr);
	if (FAILED(hr = pWriter->WriteStartElement(NULL, L"Versionmark", NULL)))
		HR(hr);
	if (FAILED(hr = pWriter->WriteString(VERSION_STRING)))
		HR(hr);
	if (FAILED(hr = pWriter->WriteEndElement()))
		HR(hr);
	if (FAILED(hr = pWriter->WriteWhitespace(L"\n")))
		HR(hr);
	if (FAILED(hr = pWriter->WriteComment(L"This WiFi SSID and Key")))
		HR(hr);
	if (FAILED(hr = pWriter->WriteWhitespace(L"\n")))
		HR(hr);
	if (FAILED(hr = pWriter->WriteStartElement(NULL, L"WiFiSSID", NULL)))
		HR(hr);
	if (FAILED(hr = pWriter->WriteString(wfw->SSID)))
		HR(hr);
	if (FAILED(hr = pWriter->WriteEndElement()))
		HR(hr);
	if (FAILED(hr = pWriter->WriteWhitespace(L"\n")))
		HR(hr);
	if (FAILED(hr = pWriter->WriteStartElement(NULL, L"WiFiKey", NULL)))
		HR(hr);
	if (FAILED(hr = pWriter->WriteString(wfw->KEY)))
		HR(hr);
	if (FAILED(hr = pWriter->WriteEndElement()))
		HR(hr);
	if (FAILED(hr = pWriter->WriteWhitespace(L"\n")))
		HR(hr);
	if (FAILED(hr = pWriter->WriteStartElement(NULL, L"Displaykey", NULL)))
		HR(hr);
	if (FAILED(hr = pWriter->WriteString(isc)))
		HR(hr);
	if (FAILED(hr = pWriter->WriteEndElement()))
		HR(hr);
	if (FAILED(hr = pWriter->WriteWhitespace(L"\n")))
		HR(hr);
	if (FAILED(hr = pWriter->WriteEndElement()))
		HR(hr);
	if (FAILED(hr = pWriter->WriteWhitespace(L"\n")))
		HR(hr);
	//if (FAILED(hr = pWriter->WriteElementString(NULL, L"WiFiSSID", NULL, wfw->SSID)))
	//	HR(hr);
	//if (FAILED(hr = pWriter->WriteElementString(NULL, L"WiFiKey", NULL, wfw->KEY)))
	//	HR(hr);
	//if (FAILED(hr = pWriter->WriteElementString(NULL, L"Displaykey", NULL,isc)))
	//	HR(hr);
CleanUp:
	SAFE_RELEASE(pWriter);
	SAFE_RELEASE(pOutFileStream);
	return hr;
}

bool WiFiAssistantConfig::GetWiFiConfig(WiFiNetInfoW* winfo)
{
	if (this->XMLReader()!=S_OK)
		return false;
	winfo->IsShowPwd = this->iwifiinfo.IsShowPwd;
	wcscpy_s(winfo->KEY, this->iwifiinfo.KEY);
	wcscpy_s(winfo->SSID, this->iwifiinfo.SSID);
	return true;
}

/*Read Value from WiFiAssistant.xml*/
BOOL VECTORCALL WiFiAssistantCredentialRead(WiFiNetInfoW *wifiinfo)
{
	WiFiAssistantConfig wac;
	wac.InitXML();
	wac.GetWiFiConfig(wifiinfo);
	//wac.~WiFiAssistantConfig();
	return TRUE;
}
/*Write Value to WiFiAssistant.xml*/
BOOL VECTORCALL WiFiAssistantCredentialWrite(WiFiNetInfoW &wifiinfo)
{
	WiFiAssistantConfig wac;
	wac.InitXML();
	wac.XMLWrite(&wifiinfo);
	return TRUE;
}


BOOL WINAPI UACSelfElevation(HWND hWnd,int nCmdShow)
{
	wchar_t szPath[MAX_PATH];
	if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)))
	{
		// Launch itself as administrator.
		SHELLEXECUTEINFO sei = { sizeof(sei) };
		sei.lpVerb = L"runas";
		sei.lpFile = szPath;
		sei.hwnd = hWnd;
		sei.lpParameters = L"-uac";
		sei.nShow = nCmdShow;
		if (!ShellExecuteEx(&sei))
		{
			return FALSE;
		}
		extern bool IsUacSelf;
		IsUacSelf = true;
		PostQuitMessage(0);
		return TRUE;
	}
		return FALSE;
}


#ifndef __cplusplus
}
#endif